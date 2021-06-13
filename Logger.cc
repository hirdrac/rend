//
// Logger.cc
// Copyright (C) 2021 Richard Bradley
//

#include "Logger.hh"
#include <string>
#include <string_view>
#include <iostream>
#include <fstream>
#include <cstdio>
#include <ctime>
#include <unistd.h>
#include <sys/types.h>
#if __has_include(<sys/syscall.h>)
#include <sys/syscall.h>
#endif


// **** Helper Functions ****
namespace {
#ifdef SYS_gettid
  // Linux specific thread id
  // NOTE: 'gettid()' is available in glibc 2.30
  //   for earlier versions, this function is needed
  [[nodiscard]] static inline pid_t get_threadid() {
    return pid_t(syscall(SYS_gettid)); }
#else
  // thread ID logging disabled
  [[nodiscard]] static inline pid_t get_threadid() { return 0; }
#endif

  const pid_t mainThreadID = get_threadid();

  void logTime(std::ostream& os)
  {
    std::time_t t = std::time(nullptr);
    std::tm* td = std::localtime(&t);
    char str[32];
    int len = snprintf(str, sizeof(str), "%d-%02d-%02d %02d:%02d:%02d ",
		       td->tm_year + 1900, td->tm_mon + 1, td->tm_mday,
		       td->tm_hour, td->tm_min, td->tm_sec);
    os.write(str, std::streamsize(len));
  }

  [[nodiscard]] std::string fileTime()
  {
    std::time_t t = std::time(nullptr);
    std::tm* td = std::localtime(&t);
    char str[32];
    int len = snprintf(str, sizeof(str), "-%d%02d%02d_%02d%02d%02d",
		       td->tm_year + 1900, td->tm_mon + 1, td->tm_mday,
		       td->tm_hour, td->tm_min, td->tm_sec);
    return std::string(str, std::size_t(len));
  }
}


// **** LoggerImpl class ****
class LoggerImpl
{
 public:
  virtual ~LoggerImpl() { }
  virtual void log(const char* str, std::streamsize len) = 0;
  virtual void rotate() { }

  void log(std::string_view s) { log(s.data(), std::streamsize(s.size())); }
};

namespace
{
  class OStreamLogger : public LoggerImpl
  {
   public:
    OStreamLogger(std::ostream& os) : _os(os) { }

    void log(const char* str, std::streamsize len) override
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
    FileLogger(std::string_view file)
      : _filename(file), _os(_filename, std::ios_base::app) { }

    void log(const char* str, std::streamsize len) override
    {
      _os.write(str, len);
      _os.flush();
    }

    void rotate() override
    {
      auto x = _filename.rfind('.');
      std::string nf = _filename.substr(0,x) + fileTime();
      if (x != std::string::npos) {
	nf += _filename.substr(x);
      }

      _os.close();
      std::rename(_filename.c_str(), nf.c_str());
      _os = std::ofstream(_filename);
    }

   private:
    std::string _filename;
    std::ofstream _os;
  };

  constexpr std::string_view levelStr(Logger::Level l)
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
Logger::Logger() : _level(INFO)
{
}

Logger::~Logger()
{
  // NOTE: empty destructor needed here because LoggerImpl is not
  //   visible externally
}

void Logger::setOStream(std::ostream& os)
{
  _impl.reset(new OStreamLogger(os));
}

void Logger::setFile(std::string_view fileName)
{
  _impl.reset(new FileLogger(fileName));
}

void Logger::logMsg(const std::ostringstream& os)
{
  if (!_impl) { setOStream(std::cerr); }
  _impl->log(os.str());
}

void Logger::rotate()
{
  if (_impl) { _impl->rotate(); }
}

void Logger::setHeader(std::ostream& os, const Data& d)
{
  logTime(os);
  os << levelStr(d.level);
}

void Logger::setFooter(std::ostream& os, const Data& d)
{
  pid_t tid = get_threadid();
  os << " (";
  if (tid != mainThreadID) { os << get_threadid() << ' '; }
  os << d.file << ':' << d.line << ")\n";
}
