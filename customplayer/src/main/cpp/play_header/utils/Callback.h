//
// Created by Admin on 2021/12/1.
//

#ifndef DEMOFFMPEG_CUSTOM_CALLBACK_H
#define DEMOFFMPEG_CUSTOM_CALLBACK_H
class Callback {
public:
   virtual void call() = 0;
};

class OnReadPixelListener{
public:
    virtual void readPixelResult(NativeOpenGLImage * data) = 0;
};
#endif //DEMOFFMPEG_CALLBACK_H
