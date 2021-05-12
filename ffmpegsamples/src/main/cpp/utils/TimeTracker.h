//
// Created by Admin on 2020/11/20.
//

#ifndef DEMOC_TIMETRACKER_H
#define DEMOC_TIMETRACKER_H


class TimeTracker {

private:
    static long long startTime;
public:

    static void trackBegin();

    static void trackOver();
};


#endif //DEMOC_TIMETRACKER_H
