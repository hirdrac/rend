//
// Shader.cc
// Copyright (C) 2024 Richard Bradley
//

#include "Shader.hh"
#include "StringUtil.hh"
#include "RegisterShader.hh"
#include <cassert>


// **** ShaderColor Class ****
std::string ShaderColor::desc() const
{
  return concat("<RGB ", _color, '>');
}

Color ShaderColor::evaluate(
  JobState& js, const Scene& s, const Ray& r, const EvaluatedHit& eh) const
{
  return _color;
}


// **** ShaderSide Class ****
REGISTER_SHADER_CLASS(ShaderSide,"side");

int ShaderSide::addShader(const ShaderPtr& sh, SceneItemFlag flag)
{
  assert(sh != nullptr);
  if (flag != FLAG_NONE) { return -1; }

  _sideShaders.push_back(sh);
  return 0;
}

Color ShaderSide::evaluate(
  JobState& js, const Scene& s, const Ray& r, const EvaluatedHit& eh) const
{
  const int x = eh.side % int(_sideShaders.size());
  return _sideShaders[std::size_t(x)]->evaluate(js, s, r, eh);
}
