#include "timer.h"

Timer::Timer()
{
    index = 0;
}

void Timer::setIndex(int ind)
{
    index = ind;
}

int Timer::getIndex()
{
    return index;
}

void Timer::sentTimerIndex()
{
    emit sentIndex(index);
}

Timer::~Timer()
{

}

