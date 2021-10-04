//
// Timer.cc
// Copyright (C) 2021 Richard Bradley
//

#include "Timer.hh"


// **** TimeStamp class ****
// Member Functions
double TimeStamp::elapsedSec(const TimeStamp& endTime) const
{
  long sec  = endTime.sec()  - _val.tv_sec;
  long usec = endTime.usec() - _val.tv_usec;
  return double(sec) + (double(usec) / 1000000.0);
}

int64_t TimeStamp::elapsedMilliSec(const TimeStamp& endTime) const
{
  int64_t sec  = endTime.sec()  - _val.tv_sec;
  int64_t usec = endTime.usec() - _val.tv_usec;
  return (sec * 1000LL) + (usec / 1000LL);
}


// **** Timer class ****
// Private Member Functions
void Timer::addToElapsed(long sec, long usec)
{
  _elapsedSec  += sec;
  _elapsedUSec += usec;

  // make sure that 0 <= usec < 1000000
  if (_elapsedUSec >= 1000000) {
    do {
      _elapsedUSec -= 1000000;
      _elapsedSec  += 1;
    } while (_elapsedUSec >= 1000000);

  } else if (_elapsedUSec < 0) {
    do {
      _elapsedUSec += 1000000;
      _elapsedSec  -= 1;
    } while (_elapsedUSec < 0);
  }
}

// Public Member Functions
void Timer::clear()
{
  _start.clear();
  _elapsedSec  = 0;
  _elapsedUSec = 0;
}

int Timer::start()
{
  if (_start.isSet()) {
    return -1; // already running
  }

  return _start.set();
}

int Timer::start(const TimeStamp& ts)
{
  if (_start.isSet() || !ts.isSet()) {
    return -1;
  }

  _start = ts;
  return 0;
}

int Timer::stop()
{
  if (!_start.isSet()) {
    return -1;
  }

  TimeStamp ts;
  ts.set();

  addToElapsed(ts.sec() - _start.sec(), ts.usec() - _start.usec());
  _start.clear();
  return 0;
}

int Timer::stop(const TimeStamp& ts)
{
  if (!_start.isSet()) {
    return -1;
  }

  addToElapsed(ts.sec() - _start.sec(), ts.usec() - _start.usec());
  _start.clear();
  return 0;
}

double Timer::elapsedSec() const
{
  long sec  = _elapsedSec;
  long usec = _elapsedUSec;

  if (_start.isSet()) {
    // timer is running - get current time for elapsed calculation
    TimeStamp ts;
    ts.set();

    sec  += ts.sec()  - _start.sec();
    usec += ts.usec() - _start.usec();
  }

  return double(sec) + (double(usec) / 1000000.0);
}

double Timer::elapsedSec(const TimeStamp& endTime) const
{
  if (!_start.isSet()) {
    return 0.0; // unable to calculate if timer isn't running
  }

  long sec  = _elapsedSec  + (endTime.sec()  - _start.sec());
  long usec = _elapsedUSec + (endTime.usec() - _start.usec());
  return double(sec) + (double(usec) / 1000000.0);
}

int64_t Timer::elapsedMilliSec() const
{
  long sec  = _elapsedSec;
  long usec = _elapsedUSec;

  if (_start.isSet()) {
    // timer is running - get current time for elapsed calculation
    TimeStamp ts;
    ts.set();

    sec  += ts.sec()  - _start.sec();
    usec += ts.usec() - _start.usec();
  }

  return (int64_t(sec) * 1000LL) + (int64_t(usec) / 1000LL);
}

int64_t Timer::elapsedMilliSec(const TimeStamp& endTime) const
{
  if (!_start.isSet()) {
    return 0LL; // unable to calculate if timer isn't running
  }

  long sec  = _elapsedSec  + (endTime.sec()  - _start.sec());
  long usec = _elapsedUSec + (endTime.usec() - _start.usec());
  return (int64_t(sec) * 1000LL) + (int64_t(usec) / 1000LL);
}
