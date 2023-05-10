//
// LightPtr.hh
// Copyright (C) 2023 Richard Bradley
//

#pragma once
#include <memory>
#include <type_traits>

class Light;
using LightPtr = std::shared_ptr<Light>;

template<class T, typename... Args> requires std::is_base_of_v<Light,T>
[[nodiscard]] inline std::shared_ptr<T> makeLight(Args... args) {
  return std::make_shared<T>(args...);
}
