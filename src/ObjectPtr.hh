//
// ObjectPtr.hh
// Copyright (C) 2021 Richard Bradley
//

#pragma once
#include <memory>
#include <type_traits>

class Object;
using ObjectPtr = std::shared_ptr<Object>;

template<class T, typename... Args>
inline std::shared_ptr<T> makeObject(Args... args) {
  static_assert(std::is_base_of_v<Object,T>);
  return std::make_shared<T>(args...);
}
