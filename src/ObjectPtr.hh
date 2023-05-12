//
// ObjectPtr.hh
// Copyright (C) 2023 Richard Bradley
//

#pragma once
#include <memory>
#include <concepts>

class Object;
using ObjectPtr = std::shared_ptr<Object>;

template<std::derived_from<Object> T, typename... Args>
[[nodiscard]] inline std::shared_ptr<T> makeObject(Args... args) {
  return std::make_shared<T>(args...);
}
