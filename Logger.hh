//
// Logger.hh
// Copyright (C) 2021 Richard Bradley
//

#pragma once
#include <string>
#include <memory>
#include <sstream>
#include <ostream>


// **** Types ****
class LoggerImpl;

class Logger
{
 public:
  enum Level { TRACE, INFO, WARN, ERROR, FATAL };

  Logger();

  // members
  void setOStream(std::ostream& os);
  void setFile(const std::string& fileName);

  [[nodiscard]] Level level() const { return _level; }
  void setLevel(Level l) { _level = l; }

  void log(Level l, const std::string& msg, const char* file, int line);
  void log(Level l, const std::ostringstream& os, const char* file, int line);
  void rotate();

  // static members
  static Logger& defaultLogger() { return _defaultLogger; }

 private:
  std::unique_ptr<LoggerImpl> _impl;
  Level _level;

  static Logger _defaultLogger;

  // disable copy/assignment
  Logger(const Logger&) = delete;
  Logger& operator=(const Logger&) = delete;
};


// **** Macros ****
// string logging macros
#define LOG_STRING_LINE(logger,lvl,x,line) \
  do { if ((lvl)>=(logger).level()) { (logger).log(lvl,x,__FILE__,line); } } while (false)

#define LOG_STRING(logger,lvl,x) LOG_STREAM_LINE(logger,lvl,x,__LINE__)


// stream logging macros
#define LOG_STREAM_LINE(logger,lvl,x,line) \
  do { if ((lvl)>=(logger).level()) { std::ostringstream os; os << x; (logger).log(lvl,os,__FILE__,line); } } while (false)

#define LOG_STREAM(logger,lvl,x) LOG_STREAM_LINE(logger,lvl,x,__LINE__)


// default logger instance logging macros
#define LOG_TRACE(x) LOG_STREAM(Logger::defaultLogger(),Logger::TRACE,x)
#define LOG_INFO(x)  LOG_STREAM(Logger::defaultLogger(),Logger::INFO,x)
#define LOG_WARN(x)  LOG_STREAM(Logger::defaultLogger(),Logger::WARN,x)
#define LOG_ERROR(x) LOG_STREAM(Logger::defaultLogger(),Logger::ERROR,x)
#define LOG_FATAL(x) LOG_STREAM(Logger::defaultLogger(),Logger::FATAL,x)
