//
// StringUtility.hh
// Copyright (C) 2024 Richard Bradley
//

#pragma once
#include <string>
#include <sstream>


template<class... Args>
[[nodiscard]] inline std::string concat(const Args&... args)
{
  std::ostringstream os;
  ((os << args),...);
  return os.str();
}
