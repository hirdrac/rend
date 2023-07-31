//
// Print.hh
// Copyright (C) 2023 Richard Bradley
//
// simplified stdout/stderr output
// (note that these functions are not equivalent to C++23 print/println)
//

#pragma once
#include <iostream>


// print()
template<typename... Args>
inline void print(std::ostream& out, const Args&... args)
{
  ((out << args),...);
}

template<typename... Args>
inline void print(const Args&... args)
{
  print(std::cout, args...);
}

// println()
template<typename... Args>
inline void println(std::ostream& out, const Args&... args)
{
  ((out << args),...);
  out.put('\n');
}

template<typename... Args>
inline void println(const Args&... args)
{
  println(std::cout, args...);
}


// std error output
template<typename... Args>
inline void print_err(const Args&... args)
{
  print(std::cerr, args...);
}

template<typename... Args>
inline void println_err(const Args&... args)
{
  println(std::cerr, args...);
}
