//
// ShaderPtr.hh
// Copyright (C) 2023 Richard Bradley
//

#pragma once
#include <memory>
#include <type_traits>

class Shader;
using ShaderPtr = std::shared_ptr<Shader>;

template<class T, typename... Args> requires std::is_base_of_v<Shader,T>
[[nodiscard]] inline std::shared_ptr<T> makeShader(Args... args) {
  return std::make_shared<T>(args...);
}
