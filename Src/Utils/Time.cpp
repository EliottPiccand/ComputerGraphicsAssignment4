#include "Utils/Time.h"

Instant Time::now()
{
    return now_;
}

Duration Time::elapsed()
{
    return now_ - Instant();
}

void Time::initialize()
{
    paused = false;
    now_ = Instant();
    delta_time_ = 0.0f;
}

void Time::update(float delta_time)
{
    delta_time_ = delta_time;

    if (paused)
        return;

    now_.seconds_ += delta_time;
    
    time_of_day_ += delta_time / DAY_TIME.toSeconds();
    while (time_of_day_ >= 1.0f)
    {
        time_of_day_ -= 1.0f;
    }
}

float Time::getDeltaTime()
{
    return paused ? 0.0f : delta_time_;
}

float Time::getDeltaTimeNoPause()
{
    return delta_time_;
}

float Time::getTimeOfDay()
{
    return time_of_day_;
}

void Time::setTimeOfDay(float time_of_day)
{
    time_of_day_ = time_of_day;
}
