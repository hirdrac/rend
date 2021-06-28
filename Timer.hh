//
// Timer.hh
// Copyright (C) 2021 Richard Bradley
//
// Elapsed time measuring class
//

// FIXME - replace linux time system calls with std::chrono

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
  void clear() { timerclear(&_val); }
  int  set()   { return gettimeofday(&_val, nullptr); }
  [[nodiscard]] bool isSet() const { return timerisset(&_val); }

  [[nodiscard]] long sec()  const { return _val.tv_sec; }
  [[nodiscard]] long usec() const { return _val.tv_usec; }

  [[nodiscard]] double  elapsedSec(const TimeStamp& endTime) const;
  [[nodiscard]] int64_t elapsedMilliSec(const TimeStamp& endTime) const;

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

  [[nodiscard]] double elapsedSec() const;
  [[nodiscard]] double elapsedSec(const TimeStamp& endTime) const;

  [[nodiscard]] int64_t elapsedMilliSec() const;
  [[nodiscard]] int64_t elapsedMilliSec(const TimeStamp& endTime) const;

 private:
  TimeStamp _start;
  long _elapsedSec = 0;
  long _elapsedUSec = 0;

  void addToElapsed(long sec, long usec);
};
