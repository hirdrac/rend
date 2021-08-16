//
// Logger.hh
// Copyright (C) 2021 Richard Bradley
//

#pragma once
#include <string_view>
#include <memory>  // unique_ptr
#include <sstream> // ostringstream
#include <ostream>


// **** Types ****
class LoggerImpl;

class Logger
{
 public:
  enum Level { TRACE, INFO, WARN, ERROR, FATAL };

  Logger();
  ~Logger();

  // members
  void setOStream(std::ostream& os);
  void setFile(std::string_view fileName);
  void rotate();

  [[nodiscard]] Level level() const { return _level; }
  void setLevel(Level lvl) { _level = lvl; }

  // static members
  static Logger& defaultLogger() {
    static Logger instance;
    return instance;
  }

  // log method
  template<typename... Args>
  void log(Level lvl, const char* file, int line, Args... args) {
    std::ostringstream os;
    Data d{file, line, lvl};
    setHeader(os, d);
    logElements(os, d, args...);
  }

 private:
  std::unique_ptr<LoggerImpl> _impl;
  Level _level;

  struct Data { const char* file; int line; Level level; };

  // methods
  void logElements(std::ostringstream& os, const Data& d) {
    // complete log steam
    setFooter(os, d);
    logMsg(os);
  }

  template<typename T, typename... Args>
  void logElements(std::ostringstream& os, const Data& d,
		   const T& x, Args... args) {
    os << x;
    logElements(os, d, args...);
  }

  void setHeader(std::ostream& os, const Data& d);
  void setFooter(std::ostream& os, const Data& d);
  void logMsg(const std::ostringstream& os);

  // disable copy/assignment
  Logger(const Logger&) = delete;
  Logger& operator=(const Logger&) = delete;
};


// **** Macros ****
#define LOGGER_LOG(logger,lvl,...) \
  do { if (Logger& lg(logger); (lvl)>=lg.level()) { lg.log((lvl),__FILE__,__LINE__,__VA_ARGS__); } } while (false)

// default logger instance logging macros
#define LOG_TRACE(...) \
  LOGGER_LOG(Logger::defaultLogger(),Logger::TRACE,__VA_ARGS__)
#define LOG_INFO(...) \
  LOGGER_LOG(Logger::defaultLogger(),Logger::INFO,__VA_ARGS__)
#define LOG_WARN(...) \
  LOGGER_LOG(Logger::defaultLogger(),Logger::WARN,__VA_ARGS__)
#define LOG_ERROR(...) \
  LOGGER_LOG(Logger::defaultLogger(),Logger::ERROR,__VA_ARGS__)
#define LOG_FATAL(...) \
  LOGGER_LOG(Logger::defaultLogger(),Logger::FATAL,__VA_ARGS__)
