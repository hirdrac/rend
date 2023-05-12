//
// ShaderPtr.hh
// Copyright (C) 2023 Richard Bradley
//

#pragma once
#include <memory>
#include <concepts>

class Shader;
using ShaderPtr = std::shared_ptr<Shader>;

template<std::derived_from<Shader> T, typename... Args>
[[nodiscard]] inline std::shared_ptr<T> makeShader(Args... args) {
  return std::make_shared<T>(args...);
}
