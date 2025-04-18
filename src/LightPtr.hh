//
// LightPtr.hh
// Copyright (C) 2025 Richard Bradley
//

#pragma once
#include <memory>
#include <concepts>

class Light;
using LightPtr = std::shared_ptr<Light>;

template<std::derived_from<Light> T, class... Args>
[[nodiscard]] std::shared_ptr<T> makeLight(Args... args) {
  return std::make_shared<T>(args...);
}
