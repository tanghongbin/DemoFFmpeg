//
// Created by Admin on 2020/10/21.
//

#include <cstdlib>
#include <cstring>
#include <pthread.h>
#include <SLES/OpenSLES.h>
#include <utils/OpenSL_IO.h>


// creates the OpenSL ES audio engine
SLresult OpenSL_IO::openSLCreateEngine(OPENSL_STREAM *p)
{
    SLresult result;
    // create engine
    result = slCreateEngine(&(p->engineObject), 0, NULL, 0, NULL, NULL);
    if(result != SL_RESULT_SUCCESS) goto  engine_end;

    // realize the engine
    result = (*p->engineObject)->Realize(p->engineObject, SL_BOOLEAN_FALSE);
    if(result != SL_RESULT_SUCCESS) goto engine_end;

    // get the engine interface, which is needed in order to create other objects
    result = (*p->engineObject)->GetInterface(p->engineObject, SL_IID_ENGINE, &(p->engineEngine));
    if(result != SL_RESULT_SUCCESS) goto  engine_end;

    engine_end:
    return result;
}

// opens the OpenSL ES device for output
SLresult OpenSL_IO::openSLPlayOpen(OPENSL_STREAM *p)
{
    SLresult result;
    SLuint32 sr = p->sr;
    SLuint32  channels = p->outchannels;

    if(channels) {
        // configure audio source
        SLDataLocator_AndroidSimpleBufferQueue loc_bufq = {SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE, 2};

        switch(sr){
            case 8000:
                sr = SL_SAMPLINGRATE_8;
                break;
            case 11025:
                sr = SL_SAMPLINGRATE_11_025;
                break;
            case 16000:
                sr = SL_SAMPLINGRATE_16;
                break;
            case 22050:
                sr = SL_SAMPLINGRATE_22_05;
                break;
            case 24000:
                sr = SL_SAMPLINGRATE_24;
                break;
            case 32000:
                sr = SL_SAMPLINGRATE_32;
                break;
            case 44100:
                sr = SL_SAMPLINGRATE_44_1;
                break;
            case 48000:
                sr = SL_SAMPLINGRATE_48;
                break;
            case 64000:
                sr = SL_SAMPLINGRATE_64;
                break;
            case 88200:
                sr = SL_SAMPLINGRATE_88_2;
                break;
            case 96000:
                sr = SL_SAMPLINGRATE_96;
                break;
            case 192000:
                sr = SL_SAMPLINGRATE_192;
                break;
            default:
                break;

                return -1;
        }

        do{
            const SLInterfaceID ids[] = {SL_IID_VOLUME};
            const SLboolean req[] = {SL_BOOLEAN_FALSE};
            result = (*p->engineEngine)->CreateOutputMix(p->engineEngine, &(p->outputMixObject), 1, ids, req);
            if(result != SL_RESULT_SUCCESS) break;

            // realize the output mix
            result = (*p->outputMixObject)->Realize(p->outputMixObject, SL_BOOLEAN_FALSE);

            unsigned int speakers;
            if(channels > 1)
                speakers = SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT;
            else speakers = SL_SPEAKER_FRONT_CENTER;

            SLDataFormat_PCM format_pcm = {SL_DATAFORMAT_PCM,channels, sr,
                                           SL_PCMSAMPLEFORMAT_FIXED_16, SL_PCMSAMPLEFORMAT_FIXED_16,
                                           speakers, SL_BYTEORDER_LITTLEENDIAN};

            SLDataSource audioSrc = {&loc_bufq, &format_pcm};

            // configure audio sink
            SLDataLocator_OutputMix loc_outmix = {SL_DATALOCATOR_OUTPUTMIX, p->outputMixObject};
            SLDataSink audioSnk = {&loc_outmix, NULL};

            // create audio player
            const SLInterfaceID ids1[] = {SL_IID_ANDROIDSIMPLEBUFFERQUEUE};
            const SLboolean req1[] = {SL_BOOLEAN_TRUE};
            result = (*p->engineEngine)->CreateAudioPlayer(p->engineEngine, &(p->bqPlayerObject), &audioSrc, &audioSnk,
                                                           1, ids1, req1);
            if(result != SL_RESULT_SUCCESS) break;

            // realize the player
            result = (*p->bqPlayerObject)->Realize(p->bqPlayerObject, SL_BOOLEAN_FALSE);
            if(result != SL_RESULT_SUCCESS) break;

            // get the play interface
            result = (*p->bqPlayerObject)->GetInterface(p->bqPlayerObject, SL_IID_PLAY, &(p->bqPlayerPlay));
            if(result != SL_RESULT_SUCCESS) break;

            // get the buffer queue interface
            result = (*p->bqPlayerObject)->GetInterface(p->bqPlayerObject, SL_IID_ANDROIDSIMPLEBUFFERQUEUE,
                                                        &(p->bqPlayerBufferQueue));
            if(result != SL_RESULT_SUCCESS) break;

            // register callback on the buffer queue
            result = (*p->bqPlayerBufferQueue)->RegisterCallback(p->bqPlayerBufferQueue, bqPlayerCallback, p);
            if(result != SL_RESULT_SUCCESS) break;

            // set the player's state to playing
            result = (*p->bqPlayerPlay)->SetPlayState(p->bqPlayerPlay, SL_PLAYSTATE_PLAYING);
        }while(false);

        return result;

    }

    return SL_RESULT_SUCCESS;
}


// Open the OpenSL ES device for input
 SLresult OpenSL_IO::openSLRecOpen(OPENSL_STREAM *p)
{
    SLresult result;
    SLuint32 sr = p->sr;
    SLuint32 channels = p->inchannels;

    if(channels) {
        switch(sr) {
            case 8000:
                sr = SL_SAMPLINGRATE_8;
                break;
            case 11025:
                sr = SL_SAMPLINGRATE_11_025;
                break;
            case 16000:
                sr = SL_SAMPLINGRATE_16;
                break;
            case 22050:
                sr = SL_SAMPLINGRATE_22_05;
                break;
            case 24000:
                sr = SL_SAMPLINGRATE_24;
                break;
            case 32000:
                sr = SL_SAMPLINGRATE_32;
                break;
            case 44100:
                sr = SL_SAMPLINGRATE_44_1;
                break;
            case 48000:
                sr = SL_SAMPLINGRATE_48;
                break;
            case 64000:
                sr = SL_SAMPLINGRATE_64;
                break;
            case 88200:
                sr = SL_SAMPLINGRATE_88_2;
                break;
            case 96000:
                sr = SL_SAMPLINGRATE_96;
                break;
            case 192000:
                sr = SL_SAMPLINGRATE_192;
                break;
            default:
                break;

                return -1;
        }

        // configure audio source
        SLDataLocator_IODevice loc_dev = {SL_DATALOCATOR_IODEVICE, SL_IODEVICE_AUDIOINPUT,
                                          SL_DEFAULTDEVICEID_AUDIOINPUT, NULL};
        SLDataSource audioSrc = {&loc_dev, NULL};

        // configure audio sink
        unsigned int speakers;
        if(channels > 1)
            speakers = SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT;
        else speakers = SL_SPEAKER_FRONT_CENTER;

        SLDataLocator_AndroidSimpleBufferQueue loc_bq = {SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE, 2};
        SLDataFormat_PCM format_pcm = {SL_DATAFORMAT_PCM, channels, sr,
                                       SL_PCMSAMPLEFORMAT_FIXED_16, SL_PCMSAMPLEFORMAT_FIXED_16,
                                       speakers, SL_BYTEORDER_LITTLEENDIAN};
        SLDataSink audioSnk = {&loc_bq, &format_pcm};

        // create audio recorder
        // (requires the RECORD_AUDIO permission)
        const SLInterfaceID id[1] = {SL_IID_ANDROIDSIMPLEBUFFERQUEUE};
        const SLboolean req[1] = {SL_BOOLEAN_TRUE};
        result = (*p->engineEngine)->CreateAudioRecorder(p->engineEngine, &(p->recorderObject), &audioSrc,
                                                         &audioSnk, 1, id, req);
        if (SL_RESULT_SUCCESS != result) goto end_recopen;

        // realize the audio recorder
        result = (*p->recorderObject)->Realize(p->recorderObject, SL_BOOLEAN_FALSE);
        if (SL_RESULT_SUCCESS != result) goto end_recopen;

        // get the record interface
        result = (*p->recorderObject)->GetInterface(p->recorderObject, SL_IID_RECORD, &(p->recorderRecord));
        if (SL_RESULT_SUCCESS != result) goto end_recopen;

        // get the buffer queue interface
        result = (*p->recorderObject)->GetInterface(p->recorderObject, SL_IID_ANDROIDSIMPLEBUFFERQUEUE,
                                                    &(p->recorderBufferQueue));
        if (SL_RESULT_SUCCESS != result) goto end_recopen;

        // register callback on the buffer queue
        result = (*p->recorderBufferQueue)->RegisterCallback(p->recorderBufferQueue, bqRecorderCallback,
                                                             p);
        if (SL_RESULT_SUCCESS != result) goto end_recopen;
        result = (*p->recorderRecord)->SetRecordState(p->recorderRecord, SL_RECORDSTATE_RECORDING);

        end_recopen:
        return result;
    }
    else
        return SL_RESULT_SUCCESS;
}

// close the OpenSL IO and destroy the audio engine
 void OpenSL_IO::openSLDestroyEngine(OPENSL_STREAM *p)
{
    // destroy buffer queue audio player object, and invalidate all associated interfaces
    if (p->bqPlayerObject != NULL) {
        (*p->bqPlayerObject)->Destroy(p->bqPlayerObject);
        p->bqPlayerObject = NULL;
        p->bqPlayerPlay = NULL;
        p->bqPlayerBufferQueue = NULL;
        p->bqPlayerEffectSend = NULL;
    }

    // destroy audio recorder object, and invalidate all associated interfaces
    if (p->recorderObject != NULL) {
        (*p->recorderObject)->Destroy(p->recorderObject);
        p->recorderObject = NULL;
        p->recorderRecord = NULL;
        p->recorderBufferQueue = NULL;
    }

    // destroy output mix object, and invalidate all associated interfaces
    if (p->outputMixObject != NULL) {
        (*p->outputMixObject)->Destroy(p->outputMixObject);
        p->outputMixObject = NULL;
    }

    // destroy engine object, and invalidate all associated interfaces
    if (p->engineObject != NULL) {
        (*p->engineObject)->Destroy(p->engineObject);
        p->engineObject = NULL;
        p->engineEngine = NULL;
    }
}

// open the android audio device for input and/or output
OPENSL_STREAM * OpenSL_IO::android_OpenAudioDevice(int sr, int inchannels, int outchannels, int buffersize)
{
    OPENSL_STREAM *p;
    p = (OPENSL_STREAM *) calloc(sizeof(OPENSL_STREAM),1);

    p->inchannels = inchannels;
    p->outchannels = outchannels;
    p->sr = sr;
    p->inlock = createThreadLock();
    p->outlock = createThreadLock();

    if((p->outBufSamples = buffersize*outchannels) != 0) {
        if((p->outputBuffer[0] = (uint8_t *) calloc(p->outBufSamples, sizeof(uint8_t))) == NULL ||
           (p->outputBuffer[1] = (uint8_t *) calloc(p->outBufSamples, sizeof(uint8_t))) == NULL) {
            android_CloseAudioDevice(p);
            return NULL;
        }
    }

    if((p->inBufSamples = buffersize*inchannels) != 0){
        if((p->inputBuffer[0] = (uint8_t *) calloc(p->inBufSamples, sizeof(uint8_t))) == NULL ||
           (p->inputBuffer[1] = (uint8_t *) calloc(p->inBufSamples, sizeof(uint8_t))) == NULL){
            android_CloseAudioDevice(p);
            return NULL;
        }
    }

    p->currentInputIndex = 0;
    p->currentOutputBuffer  = 0;
    p->currentInputIndex = p->inBufSamples;
    p->currentInputBuffer = 0;

    if(openSLCreateEngine(p) != SL_RESULT_SUCCESS) {
        android_CloseAudioDevice(p);
        return NULL;
    }

    if(openSLRecOpen(p) != SL_RESULT_SUCCESS) {
        android_CloseAudioDevice(p);
        return NULL;
    }

    if(openSLPlayOpen(p) != SL_RESULT_SUCCESS) {
        android_CloseAudioDevice(p);
        return NULL;
    }

    notifyThreadLock(p->outlock);
    notifyThreadLock(p->inlock);

    p->time = 0.;
    return p;
}

// close the android audio device
void OpenSL_IO::android_CloseAudioDevice(OPENSL_STREAM *p)
{
    if (p == NULL)
        return;

    openSLDestroyEngine(p);

    if (p->inlock != NULL) {
        notifyThreadLock(p->inlock);
        destroyThreadLock(p->inlock);
        p->inlock = NULL;
    }

    if (p->outlock != NULL) {
        notifyThreadLock(p->outlock);
        destroyThreadLock(p->outlock);
        p->inlock = NULL;
    }

    if (p->outputBuffer[0] != NULL) {
        free(p->outputBuffer[0]);
        p->outputBuffer[0] = NULL;
    }

    if (p->outputBuffer[1] != NULL) {
        free(p->outputBuffer[1]);
        p->outputBuffer[1] = NULL;
    }

    if (p->inputBuffer[0] != NULL) {
        free(p->inputBuffer[0]);
        p->inputBuffer[0] = NULL;
    }

    if (p->inputBuffer[1] != NULL) {
        free(p->inputBuffer[1]);
        p->inputBuffer[1] = NULL;
    }

    free(p);
}

// returns timestamp of the processed stream
double OpenSL_IO::android_GetTimestamp(OPENSL_STREAM *p)
{
    return p->time;
}

// this callback handler is called every time a buffer finishes recording
void OpenSL_IO::bqRecorderCallback(SLAndroidSimpleBufferQueueItf bq, void *context)
{
    OPENSL_STREAM *p = (OPENSL_STREAM *) context;
    notifyThreadLock(p->inlock);
}

// gets a buffer of size samples from the device
int OpenSL_IO::android_AudioIn(OPENSL_STREAM *p,uint8_t *buffer,int size)
{
    uint8_t *inBuffer;
    int i, bufsamps = p->inBufSamples, index = p->currentInputIndex;
    if(p == NULL || bufsamps ==  0) return 0;

//    LOGCATE("has enter audioin:%p",p->inlock);
    inBuffer = p->inputBuffer[p->currentInputBuffer];
    for(i=0; i < size; i++){
        if (index >= bufsamps) {
            waitThreadLock(p->inlock);
            (*p->recorderBufferQueue)->Enqueue(p->recorderBufferQueue,inBuffer,bufsamps*sizeof(uint8_t));
            p->currentInputBuffer = (p->currentInputBuffer ? 0 : 1);
            index = 0;
            inBuffer = p->inputBuffer[p->currentInputBuffer];
        }
        buffer[i] = (uint8_t)inBuffer[index++];
    }
    p->currentInputIndex = index;
    if(p->outchannels == 0) p->time += (double) size/(p->sr*p->inchannels);
    return i;
}

// this callback handler is called every time a buffer finishes playing
void OpenSL_IO::bqPlayerCallback(SLAndroidSimpleBufferQueueItf bq, void *context)
{
    OPENSL_STREAM *p = (OPENSL_STREAM *) context;
    notifyThreadLock(p->outlock);
}

// puts a buffer of size samples to the device
int OpenSL_IO::android_AudioOut(OPENSL_STREAM *p, uint8_t *buffer,int size)
{
    uint8_t *outBuffer;
    int i, bufsamps = p->outBufSamples, index = p->currentOutputIndex;
    if(p == NULL  || bufsamps ==  0)  return 0;
    outBuffer = p->outputBuffer[p->currentOutputBuffer];

    for(i=0; i < size; i++){
        outBuffer[index++] = (uint8_t)(buffer[i]);
        if (index >= p->outBufSamples) {
            waitThreadLock(p->outlock);
            (*p->bqPlayerBufferQueue)->Enqueue(p->bqPlayerBufferQueue,
                                               outBuffer,bufsamps*sizeof(uint8_t));
            p->currentOutputBuffer = (p->currentOutputBuffer ?  0 : 1);
            index = 0;
            outBuffer = p->outputBuffer[p->currentOutputBuffer];
        }
    }
    p->currentOutputIndex = index;
    p->time += (double) size/(p->sr*p->outchannels);
    return i;
}

//----------------------------------------------------------------------
// thread Locks
// to ensure synchronisation between callbacks and processing code
void* OpenSL_IO::createThreadLock(void)
{
    threadLock  *p;
    p = (threadLock*) malloc(sizeof(threadLock));
    if (p == NULL)
        return NULL;
    memset(p, 0, sizeof(threadLock));
    if (pthread_mutex_init(&(p->m), (pthread_mutexattr_t*) NULL) != 0) {
        free((void*) p);
        return NULL;
    }
    if (pthread_cond_init(&(p->c), (pthread_condattr_t*) NULL) != 0) {
        pthread_mutex_destroy(&(p->m));
        free((void*) p);
        return NULL;
    }
    p->s = (unsigned char)1;

    return p;
}

int OpenSL_IO::waitThreadLock(void *lock)
{
    threadLock  *p;
    int retval = 0;
    p = (threadLock*)lock;
    pthread_mutex_lock(&(p->m));
    while (!p->s) {
        pthread_cond_wait(&(p->c), &(p->m));
    }
    p->s = (unsigned char)0;
    pthread_mutex_unlock(&(p->m));
    return retval;
}

void OpenSL_IO::notifyThreadLock(void *lock)
{
    threadLock *p;
    p = (threadLock*) lock;
    pthread_mutex_lock(&(p->m));
    p->s = (unsigned char)1;
    pthread_cond_signal(&(p->c));
    pthread_mutex_unlock(&(p->m));
}

void OpenSL_IO::destroyThreadLock(void *lock)
{
    threadLock  *p;
    p = (threadLock*) lock;
    if (p == NULL)
        return;
    notifyThreadLock(p);
    pthread_cond_destroy(&(p->c));
    pthread_mutex_destroy(&(p->m));
    free(p);
}
