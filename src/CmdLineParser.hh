//
// CmdLineParser.hh
// Copyright (C) 2023 Richard Bradley
//
// Helper class for parsing command line arguments.
//
// Option arguments must follow one of these forms:
//  -x             single letter option flag
//  -x<int>        single letter option with integer
//  -x=<value>     single letter option with value
//  -x <value>     single letter option with value
//  --xxx          long option flag
//  --xxx=<value>  long option with value
//  --xxx <value>  long option with value
//
// Combining single letter options (ex: "-l -a" -> "-la") currently not
// supported.
//
// An argument of '--' can be used to specify the end of option arguments
// (i.e. every argument following '--' will be treated as a non-option
// argument even if it starts with '-' or '--').
//

#pragma once
#include <string_view>
#include <type_traits>
#include <charconv>
#include <cctype>


class CmdLineParser
{
 public:
  CmdLineParser(int argc, const char* const* argv)
    : _argv{argv}, _argc{argc} { next(); }

  [[nodiscard]] explicit operator bool() const { return _current < _argc; }
  CmdLineParser& operator++() { next(); return *this; }

  [[nodiscard]] std::string_view arg() const { return _arg; }

  bool get(auto& val) const { return convertVal(_arg, val); }

  [[nodiscard]] bool option() const {
    return !_optionsDone && _arg.size() >= 2 && _arg[0] == '-';
  }

  [[nodiscard]] bool option(char shortName, std::string_view longName) const {
    if (!_optionsDone) {
      // short name option check (-x)
      if (shortName != '\0' && _arg.size() == 2 && _arg[0] == '-'
          && _arg[1] == shortName) { return true; }

      // long name option check (--xxx)
      const std::size_t len = longName.size() + 2;
      if (!longName.empty() && _arg.size() == len && _arg.substr(0,2) == "--"
          && _arg.substr(2) == longName) { return true; }
    }
    return false;
  }

  [[nodiscard]] bool option(
    char shortName, std::string_view longName, auto& value)
  {
    if (!option()) { return false; }

    if (shortName != '\0' && _arg.size() > 2 && _arg[1] == shortName) {
      if (_arg[2] == '=') {
        // short name option with value in same arg string (-x=<value>)
        return convertVal(_arg.substr(3), value);
      } else {
        // short name option with integer (-x<int>)
        bool allDigit = true;
        const std::string_view numStr = _arg.substr(2);
        for (auto ch : numStr) { allDigit &= std::isdigit(ch); }
        return allDigit ? convertVal(numStr, value) : false;
      }
    }

    const std::size_t len = longName.size() + 2;
    if (!longName.empty() && _arg.size() > len && _arg[1] == '-'
        && _arg.substr(2,len-2) == longName && _arg[len] == '=') {
      // long name option with value in same arg string (--xxx=<value>)
      return convertVal(_arg.substr(len+1), value);
    }

    if (!option(shortName, longName)) { return false; }

    // option value in next arg string
    std::string_view opVal;
    if (_current < (_argc-1)) {
      opVal = _argv[_current+1];
      if (!_optionsDone && opVal == "--") { opVal = {}; }
    }

    if (!convertVal(opVal, value)) { return false; }

    next();
    return true;
  }

 private:
  const char* const* _argv;
  int _argc;

  int _current = 0;
  std::string_view _arg;
  bool _optionsDone = false;

  void next() {
    if (++_current < _argc) {
      _arg = _argv[_current];
      if (!_optionsDone && _arg == "--") { _optionsDone = true; next(); }
    } else {
      _arg = {}; _optionsDone = true;
    }
  }

  template<class T>
  static bool convertVal(std::string_view v, T& result) {
    static_assert(!std::is_same_v<T,bool>);
    if constexpr (std::is_arithmetic_v<T>) {
      if (v.empty()) { return false; }
      auto err = std::from_chars(&v[0], &v[v.size()], result);
      return err.ec == std::errc{};
    } else {
      result = v;
      return true;
    }
  }
};
