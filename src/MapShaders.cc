//
// MapShaders.cc
// Copyright (C) 2021 Richard Bradley
//

#include "MapShaders.hh"
#include "Intersect.hh"


// **** MapShader Class ****
int MapShader::addShader(const ShaderPtr& sh, SceneItemFlag flag)
{
  if (!sh || _child || flag != FLAG_NONE) { return -1; }

  _child = sh;
  return 0;
}

int MapShader::init(Scene& s)
{
  return _child ? InitShader(s, *_child) : -1;
}


// **** ShaderGlobal Class ****
Color ShaderGlobal::evaluate(
  const Scene& s, const Ray& r, const HitInfo& h, const EvaluatedHit& eh) const
{
  EvaluatedHit eh2{eh.global_pt, eh.normal, eh.global_pt};
  return _child->evaluate(s, r, h, eh2);
}


// **** ShaderLocal Class ****
Color ShaderLocal::evaluate(
  const Scene& s, const Ray& r, const HitInfo& h, const EvaluatedHit& eh) const
{
  EvaluatedHit eh2{eh.global_pt, eh.normal, h.local_pt};
  return _child->evaluate(s, r, h, eh2);
}
