//
// Created by Admin on 2020/11/20.
//

#include "TimeTracker.h"
#include "CustomGLUtils.h"

long long TimeTracker::startTime = 0L;

void TimeTracker::trackBegin(){
    LOGCATE("time ---------------------------------track time begin");
    startTime = GetSysCurrentTime();
}

void TimeTracker::trackOver(){
    long long end = GetSysCurrentTime() - startTime;
    LOGCATE("time ---------------------------------track time over---------- total cost:%lld",end);
}