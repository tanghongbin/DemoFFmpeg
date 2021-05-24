//
// Created by Admin on 2021/5/21.
//

#ifndef DEMOFFMPEG_MESSAGE_H
#define DEMOFFMPEG_MESSAGE_H

#include <stdlib.h>
#include <string>
#include "utils.h"

class Message{

public:

    int type;
    int arg1;
    int arg2;
    std::string msg;

    ~Message(){
//        LOGCATE("此消息已被删除");
    }

    static Message* obtain(int type,int arg1,int arg2,std::string msg){
        Message* obj = new Message;
        obj->type =type;
        obj->arg1 = arg1;
        obj->arg2 = arg2;
        obj->msg = msg;
        return obj;
    }

    static Message* obtain(int type,int arg1,int arg2){
        return obtain(type,arg1,arg2,"");
    }
    void recycle(){
        delete this;
    }
};

#endif //DEMOFFMPEG_MESSAGE_H
