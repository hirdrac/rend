//
// ObjectPtr.hh
// Copyright (C) 2023 Richard Bradley
//

#pragma once
#include <memory>
#include <type_traits>

class Object;
using ObjectPtr = std::shared_ptr<Object>;

template<class T, typename... Args> requires std::is_base_of_v<Object,T>
[[nodiscard]] inline std::shared_ptr<T> makeObject(Args... args) {
  return std::make_shared<T>(args...);
}
