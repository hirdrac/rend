//
// Shader.cc
// Copyright (C) 2021 Richard Bradley
//

#include "Shader.hh"
#include "Scene.hh"
#include "Transform.hh"
#include <sstream>


// **** ShaderColor Class ****
std::string ShaderColor::desc() const
{
  std::ostringstream os;
  os << "<RGB " << _color << '>';
  return os.str();
}

Color ShaderColor::evaluate(
  const Scene& s, const Ray& r, const HitInfo& h, const EvaluatedHit& eh) const
{
  return _color;
}


// **** Functions ****
int InitShader(Scene& s, Shader& sh, const Transform* t)
{
  // Transform by parent trans
  Transform* trans = sh.trans();
  if (trans) {
    trans->global = trans->local;
    if (t) { trans->global *= t->global; }
  }

  ++s.shader_count;
  return sh.init(s);
}
