//
// ShaderPtr.hh
// Copyright (C) 2021 Richard Bradley
//

#pragma once
#include <memory>

class Shader;
using ShaderPtr = std::shared_ptr<Shader>;

template<class T, typename... Args>
inline std::shared_ptr<T> makeShader(Args... args) {
  return std::make_shared<T>(args...);
}