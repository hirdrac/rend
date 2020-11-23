//
// Timer.hh
// Copyright (C) 2020 Richard Bradley
//
// Elaplsed time messuring class
//

#pragma once
#include <cstdint>
#include <sys/time.h>


// **** Types ****
class TimeStamp
{
 public:
  // Constructors
  TimeStamp() { clear(); }
  TimeStamp(timeval& tv) : _val(tv) { }

  // Member Functions
  void clear()       { timerclear(&_val); }
  int  set()         { return gettimeofday(&_val, nullptr); }
  bool isSet() const { return timerisset(&_val); }

  long sec()  const { return _val.tv_sec; }
  long usec() const { return _val.tv_usec; }

  double  elapsedSec(const TimeStamp& endTime) const;
  int64_t elapsedMilliSec(const TimeStamp& endTime) const;

 private:
  timeval _val;
};


class Timer
{
 public:
  // Member Functions
  void clear();

  int start();
  int start(const TimeStamp& ts);

  int stop();
  int stop(const TimeStamp& ts);

  double elapsedSec() const;
  double elapsedSec(const TimeStamp& endTime) const;

  int64_t elapsedMilliSec() const;
  int64_t elapsedMilliSec(const TimeStamp& endTime) const;

 private:
  TimeStamp _start;
  long _elapsedSec = 0;
  long _elapsedUSec = 0;

  void addToElapsed(long sec, long usec);
};
