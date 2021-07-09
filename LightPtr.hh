//
// LightPtr.hh
// Copyright (C) 2021 Richard Bradley
//

#pragma once
#include <memory>

class Light;
using LightPtr = std::shared_ptr<Light>;

template<class T, typename... Args>
inline std::shared_ptr<T> makeLight(Args... args) {
  return std::make_shared<T>(args...);
}
