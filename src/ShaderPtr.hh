//
// ShaderPtr.hh
// Copyright (C) 2021 Richard Bradley
//

#pragma once
#include <memory>
#include <type_traits>

class Shader;
using ShaderPtr = std::shared_ptr<Shader>;

template<class T, typename... Args>
inline std::shared_ptr<T> makeShader(Args... args) {
  static_assert(std::is_base_of_v<Shader,T>);
  return std::make_shared<T>(args...);
}
