//
// StringUtil.hh
// Copyright (C) 2025 Richard Bradley
//

#pragma once
#include <string>
#include <sstream>


template<class... Args>
[[nodiscard]] std::string concat(const Args&... args)
{
  if constexpr (sizeof...(args) == 0) {
    return {};
  } else {
    std::ostringstream os;
    ((os << args),...);
    return os.str();
  }
}
