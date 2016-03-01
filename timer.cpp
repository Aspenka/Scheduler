#include "Timer.h"
#include <assert.h>
#include <limits>
#include <iostream>
#include <stdio.h>
#include <QTimerEvent>
#include <QDateTime>

Timer::Timer(QObject *parent) : QObject(parent)
{

}

void Timer::setSingleShot(bool singleShot)
{
    singShot = singleShot;
}

void Timer::start(TaskPair pair)
{
    cronJob = pair.first;
    taskName = pair.second;
    std::lock_guard<std::mutex> lock(mtxNextExec);
    nextExec = parser.getDateTime(cronJob).toTime_t();
    timerId = startTimer(calcDiffTime());
    assert(timerId);
}

void Timer::stop()
{
    std::lock_guard<std::mutex> lock(mtxNextExec);
    nextExec = -1;
    killTimer(timerId);
}

void Timer::timerEvent(QTimerEvent *event)
{
    if(event->timerId() != timerId)
        return;
    std::lock_guard<std::mutex> lock(mtxNextExec);
    killTimer(timerId);
    timerId = 0;
    if(nextExec > QDateTime::currentDateTime().toTime_t())
    {
        timerId = startTimer(calcDiffTime());
        assert(timerId);
    }
    else
    {
        TaskPair pair(cronJob, taskName);
        emit timeout(pair);
        if(!singShot)
        {
            nextExec = parser.getDateTime(cronJob).toTime_t();
            timerId = startTimer(calcDiffTime());
        }
    }
}

time_t Timer::calcDiffTime()
{
    time_t t = QDateTime::currentDateTime().msecsTo(QDateTime::fromTime_t(0).addSecs(nextExec));
    if(t < 0)
        return 0;
    if(t > INT_MAX)
        return INT_MAX;
    return t;
}

Timer::~Timer()
{

}

