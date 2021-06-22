//
// Print.hh
// Copyright (C) 2021 Richard Bradley
//
// simplified stdout/stderr output
//

#pragma once
#include <iostream>


// print()
template<typename... Args>
inline void print(const Args&... args)
{
  ((std::cout << args),...);
}

// println()
template<typename... Args>
inline void println(const Args&... args)
{
  ((std::cout << args),...);
  std::cout.put('\n');
}

// print_err()
template<typename... Args>
inline void print_err(const Args&... args)
{
  ((std::cerr << args),...);
}

// println_err()
template<typename... Args>
inline void println_err(const Args&... args)
{
  ((std::cerr << args),...);
  std::cerr.put('\n');
}

// print_os()
template<typename... Args>
inline void print_os(std::ostream& os, const Args&... args)
{
  ((os << args),...);
}

// println_os()
template<typename... Args>
inline void println_os(std::ostream& os, const Args&... args)
{
  ((os << args),...);
  os.put('\n');
}
