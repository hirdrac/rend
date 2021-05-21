//
// Logger.cc
// Copyright (C) 2021 Richard Bradley
//

#include "Logger.hh"
#include <string>
#include <iostream>
#include <fstream>
#include <cstdio>
#include <ctime>


// **** Helper Functions ****
#include <unistd.h>
#include <sys/types.h>
#if __has_include(<sys/syscall.h>)
#include <sys/syscall.h>
#endif

namespace {
#ifdef SYS_gettid
  // Linux specific thread id
  // NOTE: 'gettid()' is available in glibc 2.30
  //   for earlier versions, this function is needed
  [[nodiscard]] inline pid_t get_threadid() {
    return pid_t(syscall(SYS_gettid)); }
#else
  // thread ID logging disabled
  [[nodiscard]] inline pid_t get_threadid() { return 0; }
#endif

  const pid_t mainThreadID = get_threadid();

  std::string logTime()
  {
    std::time_t t = std::time(nullptr);
    std::tm tdata;
    localtime_r(&t, &tdata);
    char str[32];
    int len = snprintf(str, sizeof(str), "%d-%02d-%02d %02d:%02d:%02d ",
		       tdata.tm_year + 1900, tdata.tm_mon + 1, tdata.tm_mday,
		       tdata.tm_hour, tdata.tm_min, tdata.tm_sec);
    return std::string(str, len);
  }

  std::string fileTime()
  {
    std::time_t t = std::time(nullptr);
    std::tm tdata;
    localtime_r(&t, &tdata);
    char str[32];
    int len = snprintf(str, sizeof(str), "-%d%02d%02d_%02d%02d%02d",
		       tdata.tm_year + 1900, tdata.tm_mon + 1, tdata.tm_mday,
		       tdata.tm_hour, tdata.tm_min, tdata.tm_sec);
    return std::string(str, len);
  }
}


// **** LoggerImpl class ****
class LoggerImpl
{
 public:
  virtual ~LoggerImpl() { }
  virtual void log(const char* str, int len) = 0;
  virtual void rotate() { }
};

namespace
{
  class OStreamLogger : public LoggerImpl
  {
   public:
    OStreamLogger(std::ostream& os) : _os(os) { }

    void log(const char* str, int len) override
    {
      _os.write(str, len);
      _os.flush();
    }

   private:
    std::ostream& _os;
  };

  class FileLogger : public LoggerImpl
  {
   public:
    FileLogger(const std::string& file)
      : _os(file, std::ios_base::app), _filename(file) { }

    void log(const char* str, int len) override
    {
      _os.write(str, len);
      _os.flush();
    }

    void rotate() override
    {
      auto x = _filename.rfind('.');
      std::string nf;
      if (x == std::string::npos) {
	nf = _filename + fileTime();
      } else {
	nf = _filename.substr(0,x) + fileTime() + _filename.substr(x);
      }

      _os.close();
      std::rename(_filename.c_str(), nf.c_str());
      _os = std::ofstream(_filename);
    }

   private:
    std::ofstream _os;
    std::string _filename;
  };

  [[nodiscard]] constexpr const char* levelStr(Logger::Level l)
  {
    switch (l) {
      case Logger::TRACE: return "[TRACE] ";
      case Logger::INFO:  return "[INFO] ";
      case Logger::WARN:  return "[WARN] ";
      case Logger::ERROR: return "[ERROR] ";
      case Logger::FATAL: return "[FATAL] ";
      default:            return "[UNKNOWN] ";
    }
  }
}


// **** Logger class ****
Logger Logger::_defaultLogger;

Logger::Logger() : _level(INFO)
{
  setOStream(std::cerr);
}

void Logger::setOStream(std::ostream& os)
{
  _impl.reset(new OStreamLogger(os));
}

void Logger::setFile(const std::string& fileName)
{
  _impl.reset(new FileLogger(fileName));
}

void Logger::log(Level l, const std::string& msg, const char* file, int line)
{
  std::ostringstream os;
  pid_t tid = get_threadid();
  os << logTime() << levelStr(l) << msg << " (";
  if (tid != mainThreadID) { os << "t=" << tid << ' '; }
  os << file << ':' << line << ")\n";
  std::string s = os.str();
  _impl->log(s.c_str(), s.length());
}

void Logger::log(Level l, const std::ostringstream& os,
		 const char* file, int line)
{
  log(l, os.str(), file, line);
}

void Logger::rotate()
{
  _impl->rotate();
}
