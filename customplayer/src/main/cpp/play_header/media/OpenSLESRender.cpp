//
// Created by Admin on 2020/9/18.
//

#include "OpenSLESRender.h"
#include "utils/utils.h"

void OpenSLESRender::Init() {
    LOGCATE("OpenSLESRender::Init started");

    int result = -1;
    do {
        result = CreateEngine();
        if(result != SL_RESULT_SUCCESS)
        {
            LOGCATE("OpenSLESRender::Init CreateEngine fail. result=%d", result);
            break;
        }

        result = CreateOutputMixer();
        if(result != SL_RESULT_SUCCESS)
        {
            LOGCATE("OpenSLESRender::Init CreateOutputMixer fail. result=%d", result);
            break;
        }

        result = CreateAudioPlayer();
        if(result != SL_RESULT_SUCCESS)
        {
            LOGCATE("OpenSLESRender::Init CreateAudioPlayer fail. result=%d", result);
            break;
        }

        m_thread = new std::thread(CreateSLWaitingThread, this);

    } while (false);

    if(result != SL_RESULT_SUCCESS) {
        LOGCATE("OpenSLESRender::Init fail. result=%d", result);
        UnInit();
    }

    LOGCATE("OpenSLESRender::Init success");

}

void OpenSLESRender::RenderAudioFrame(uint8_t *pData, int dataSize) {
//    LOGCATE("OpenSLESRender::RenderAudioFrame pData=%p, dataSize=%d", pData, dataSize);
    if(m_AudioPlayerPlay) {
        if (pData != nullptr && dataSize > 0) {

            //temp resolution, when queue size is too big.
            while(GetAudioFrameQueueSize() >= MAX_QUEUE_BUFFER_SIZE && !m_Exit)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(16));
            }

            std::unique_lock<std::mutex> lock(m_Mutex);
            AudioFrame *audioFrame = new AudioFrame(pData, dataSize);
            m_AudioFrameQueue.push(audioFrame);
            m_Cond.notify_all();
            lock.unlock();
        }
    }

}

void OpenSLESRender::UnInit() {
    LOGCATE("OpenSLESRender::UnInit");

    if (m_AudioPlayerPlay) {
        (*m_AudioPlayerPlay)->SetPlayState(m_AudioPlayerPlay, SL_PLAYSTATE_STOPPED);
        m_AudioPlayerPlay = nullptr;
    }

    std::unique_lock<std::mutex> lock(m_Mutex);
    m_Exit = true;
    m_Cond.notify_all();
    lock.unlock();

    if (m_AudioPlayerObj) {
        (*m_AudioPlayerObj)->Destroy(m_AudioPlayerObj);
        m_AudioPlayerObj = nullptr;
        m_BufferQueue = nullptr;
    }

    if (m_OutputMixObj) {
        (*m_OutputMixObj)->Destroy(m_OutputMixObj);
        m_OutputMixObj = nullptr;
    }

    if (m_EngineObj) {
        (*m_EngineObj)->Destroy(m_EngineObj);
        m_EngineObj = nullptr;
        m_EngineEngine = nullptr;
    }

    lock.lock();
    for (int i = 0; i < m_AudioFrameQueue.size(); ++i) {
        AudioFrame *audioFrame = m_AudioFrameQueue.front();
        m_AudioFrameQueue.pop();
        delete audioFrame;
    }
    lock.unlock();

    if(m_thread != nullptr)
    {
        m_thread->join();
        delete m_thread;
        m_thread = nullptr;
    }

//    AudioGLRender::ReleaseInstance();

}

int OpenSLESRender::CreateEngine() {
    SLresult result = SL_RESULT_SUCCESS;
    do {
        result = slCreateEngine(&m_EngineObj, 0, nullptr, 0, nullptr, nullptr);
        if(result != SL_RESULT_SUCCESS)
        {
            LOGCATE("OpenSLESRender::CreateEngine slCreateEngine fail. result=%d", result);
            break;
        }

        result = (*m_EngineObj)->Realize(m_EngineObj, SL_BOOLEAN_FALSE);
        if(result != SL_RESULT_SUCCESS)
        {
            LOGCATE("OpenSLESRender::CreateEngine Realize fail. result=%d", result);
            break;
        }

        result = (*m_EngineObj)->GetInterface(m_EngineObj, SL_IID_ENGINE, &m_EngineEngine);
        if(result != SL_RESULT_SUCCESS)
        {
            LOGCATE("OpenSLESRender::CreateEngine GetInterface fail. result=%d", result);
            break;
        }

    } while (false);
    return result;
}

int OpenSLESRender::CreateOutputMixer() {
    SLresult result = SL_RESULT_SUCCESS;
    do {
        const SLInterfaceID mids[1] = {SL_IID_ENVIRONMENTALREVERB};
        const SLboolean mreq[1] = {SL_BOOLEAN_FALSE};

        result = (*m_EngineEngine)->CreateOutputMix(m_EngineEngine, &m_OutputMixObj, 1, mids, mreq);
        if(result != SL_RESULT_SUCCESS)
        {
            LOGCATE("OpenSLESRender::CreateOutputMixer CreateOutputMix fail. result=%d", result);
            break;
        }

        result = (*m_OutputMixObj)->Realize(m_OutputMixObj, SL_BOOLEAN_FALSE);
        if(result != SL_RESULT_SUCCESS)
        {
            LOGCATE("OpenSLESRender::CreateOutputMixer CreateOutputMix fail. result=%d", result);
            break;
        }

    } while (false);

    return result;
}

int OpenSLESRender::CreateAudioPlayer() {
    SLDataLocator_AndroidSimpleBufferQueue android_queue = {SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE, 2};
    SLDataFormat_PCM pcm = {
            SL_DATAFORMAT_PCM,//format type
            (SLuint32)2,//channel count
            SL_SAMPLINGRATE_44_1,//44100hz
            SL_PCMSAMPLEFORMAT_FIXED_16,// bits per sample
            SL_PCMSAMPLEFORMAT_FIXED_16,// container size
            SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT,// channel mask
            SL_BYTEORDER_LITTLEENDIAN // endianness
    };
    SLDataSource slDataSource = {&android_queue, &pcm};

    SLDataLocator_OutputMix outputMix = {SL_DATALOCATOR_OUTPUTMIX, m_OutputMixObj};
    SLDataSink slDataSink = {&outputMix, nullptr};

    const SLInterfaceID ids[3] = {SL_IID_BUFFERQUEUE, SL_IID_EFFECTSEND, SL_IID_VOLUME};
    const SLboolean req[3] = {SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE};

    SLresult result;

    do {

        result = (*m_EngineEngine)->CreateAudioPlayer(m_EngineEngine, &m_AudioPlayerObj, &slDataSource, &slDataSink, 3, ids, req);
        if(result != SL_RESULT_SUCCESS)
        {
            LOGCATE("OpenSLESRender::CreateAudioPlayer CreateAudioPlayer fail. result=%d", result);
            break;
        }

        result = (*m_AudioPlayerObj)->Realize(m_AudioPlayerObj, SL_BOOLEAN_FALSE);
        if(result != SL_RESULT_SUCCESS)
        {
            LOGCATE("OpenSLESRender::CreateAudioPlayer Realize fail. result=%d", result);
            break;
        }

        result = (*m_AudioPlayerObj)->GetInterface(m_AudioPlayerObj, SL_IID_PLAY, &m_AudioPlayerPlay);
        if(result != SL_RESULT_SUCCESS)
        {
            LOGCATE("OpenSLESRender::CreateAudioPlayer GetInterface fail. result=%d", result);
            break;
        }

        result = (*m_AudioPlayerObj)->GetInterface(m_AudioPlayerObj, SL_IID_BUFFERQUEUE, &m_BufferQueue);
        if(result != SL_RESULT_SUCCESS)
        {
            LOGCATE("OpenSLESRender::CreateAudioPlayer GetInterface fail. result=%d", result);
            break;
        }

        result = (*m_BufferQueue)->RegisterCallback(m_BufferQueue, AudioPlayerCallback, this);
        if(result != SL_RESULT_SUCCESS)
        {
            LOGCATE("OpenSLESRender::CreateAudioPlayer RegisterCallback fail. result=%d", result);
            break;
        }

        result = (*m_AudioPlayerObj)->GetInterface(m_AudioPlayerObj, SL_IID_VOLUME, &m_AudioPlayerVolume);
        if(result != SL_RESULT_SUCCESS)
        {
            LOGCATE("OpenSLESRender::CreateAudioPlayer GetInterface fail. result=%d", result);
            break;
        }

    } while (false);

    return result;
}

void OpenSLESRender::StartRender() {

    while (GetAudioFrameQueueSize() < MAX_QUEUE_BUFFER_SIZE && !m_Exit) {
        std::unique_lock<std::mutex> lock(m_Mutex);
        m_Cond.wait_for(lock, std::chrono::milliseconds(10));
        //m_Cond.wait(lock);
        lock.unlock();
    }

    (*m_AudioPlayerPlay)->SetPlayState(m_AudioPlayerPlay, SL_PLAYSTATE_PLAYING);
    AudioPlayerCallback(m_BufferQueue, this);
}

void OpenSLESRender::HandleAudioFrameQueue() {
//    LOGCATE("OpenSLESRender::HandleAudioFrameQueue QueueSize=%lu", m_AudioFrameQueue.size());
    if (m_AudioPlayerPlay == nullptr) return;

    while (GetAudioFrameQueueSize() < MAX_QUEUE_BUFFER_SIZE && !m_Exit) {
        std::unique_lock<std::mutex> lock(m_Mutex);
        m_Cond.wait_for(lock, std::chrono::milliseconds(10));
    }

    std::unique_lock<std::mutex> lock(m_Mutex);
    AudioFrame *audioFrame = m_AudioFrameQueue.front();
    if (nullptr != audioFrame && m_AudioPlayerPlay) {
        SLresult result = (*m_BufferQueue)->Enqueue(m_BufferQueue, audioFrame->data, (SLuint32) audioFrame->dataSize);
        if (result == SL_RESULT_SUCCESS) {
//            AudioGLRender::GetInstance()->UpdateAudioFrame(audioFrame);
            m_AudioFrameQueue.pop();
            delete audioFrame;
        }

    }
    lock.unlock();

}

void OpenSLESRender::CreateSLWaitingThread(OpenSLESRender *OpenSLESRender) {
    OpenSLESRender->StartRender();
}

void OpenSLESRender::AudioPlayerCallback(SLAndroidSimpleBufferQueueItf bufferQueue, void *context) {
    OpenSLESRender *OpenSLESRender = static_cast<class OpenSLESRender *>(context);
    OpenSLESRender->HandleAudioFrameQueue();
}

int OpenSLESRender::GetAudioFrameQueueSize() {
    std::unique_lock<std::mutex> lock(m_Mutex);
    return m_AudioFrameQueue.size();
}

void OpenSLESRender::ClearAudioCache() {
    std::unique_lock<std::mutex> lock(m_Mutex);
    for (int i = 0; i < m_AudioFrameQueue.size(); ++i) {
        AudioFrame *audioFrame = m_AudioFrameQueue.front();
        m_AudioFrameQueue.pop();
        delete audioFrame;
    }

}