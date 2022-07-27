//
// Shader.cc
// Copyright (C) 2022 Richard Bradley
//

#include "Shader.hh"
#include "Scene.hh"
#include "Transform.hh"
#include "StringUtility.hh"
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
  int x = eh.side % int(_sideShaders.size());
  return _sideShaders[std::size_t(x)]->evaluate(js, s, r, eh);
}


// **** Functions ****
int InitShader(Scene& s, Shader& sh, const Transform* tr)
{
  Transform* trans = sh.trans();
  if (trans) { trans->init(tr); tr = trans; }

  ++s.shader_count;
  return sh.init(s, tr);
}
