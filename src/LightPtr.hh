//
// LightPtr.hh
// Copyright (C) 2021 Richard Bradley
//

#pragma once
#include <memory>
#include <type_traits>

class Light;
using LightPtr = std::shared_ptr<Light>;

template<class T, typename... Args>
inline std::shared_ptr<T> makeLight(Args... args) {
  static_assert(std::is_base_of_v<Light,T>);
  return std::make_shared<T>(args...);
}
