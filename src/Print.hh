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
inline void print(std::ostream& out, Args&&... args)
{
  (out << ... << args);
}

template<typename... Args>
inline void print(Args&&... args)
{
  print(std::cout, args...);
}

// println()
template<typename... Args>
inline void println(std::ostream& out, Args&&... args)
{
  (out << ... << args) << '\n';
}

template<typename... Args>
inline void println(Args&&... args)
{
  println(std::cout, args...);
}


// std error output
template<typename... Args>
inline void print_err(Args&&... args)
{
  print(std::cerr, args...);
}

template<typename... Args>
inline void println_err(Args&&... args)
{
  println(std::cerr, args...);
}
