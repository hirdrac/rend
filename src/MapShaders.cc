//
// MapShaders.cc
// Copyright (C) 2021 Richard Bradley
//

#include "MapShaders.hh"
#include "Intersect.hh"
#include <algorithm>
#include <cmath>


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


// **** General Map Shaders ****
Color MapGlobalShader::evaluate(
  const Scene& s, const Ray& r, const HitInfo& h, const EvaluatedHit& eh) const
{
  EvaluatedHit eh2{eh.global_pt, eh.normal, eh.global_pt};
  return _child->evaluate(s, r, h, eh2);
}


// **** Object Map Shaders ****
Color MapConeShader::evaluate(
  const Scene& s, const Ray& r, const HitInfo& h, const EvaluatedHit& eh) const
{
  EvaluatedHit eh2;
  eh2.global_pt = eh.global_pt;
  eh2.normal = eh.normal;
  if (h.side == 1) {
    // base
    eh2.map = {-(h.local_pt.x), h.local_pt.y, -1.0};
  } else {
    // side
    const Vec2 dir = UnitVec(Vec2{h.local_pt.x, h.local_pt.y});
    const Flt x = std::clamp(dir.x, -1.0 + VERY_SMALL, 1.0 - VERY_SMALL);
    const Flt u = (std::acos(x) * (2.0/PI)) - 1.0;
    eh2.map = {(h.local_pt.y >= 0.0) ? u : -u, h.local_pt.z, 0.0};
  }

  return _child->evaluate(s, r, h, eh2);
}

Color MapCubeShader::evaluate(
  const Scene& s, const Ray& r, const HitInfo& h, const EvaluatedHit& eh) const
{
  EvaluatedHit eh2;
  eh2.global_pt = eh.global_pt;
  eh2.normal = eh.normal;
  switch (h.side) {
    case 0:  eh2.map = {-h.local_pt.z,  h.local_pt.y, 0.0}; break;
    case 1:  eh2.map = { h.local_pt.z,  h.local_pt.y, 0.0}; break;
    case 2:  eh2.map = { h.local_pt.x, -h.local_pt.z, 0.0}; break;
    case 3:  eh2.map = { h.local_pt.x,  h.local_pt.z, 0.0}; break;
    case 4:  eh2.map = { h.local_pt.x,  h.local_pt.y, 0.0}; break;
    case 5:  eh2.map = {-h.local_pt.x,  h.local_pt.y, 0.0}; break;
    default: eh2.map = h.local_pt; break;
  }

  return _child->evaluate(s, r, h, eh2);
}

Color MapCylinderShader::evaluate(
  const Scene& s, const Ray& r, const HitInfo& h, const EvaluatedHit& eh) const
{
  EvaluatedHit eh2;
  eh2.global_pt = eh.global_pt;
  eh2.normal = eh.normal;
  if (h.side == 0) {
    // side hit
    const Flt x =
      std::clamp(h.local_pt.x, -1.0 + VERY_SMALL, 1.0 - VERY_SMALL);
    const Flt u = (std::acos(x) * (2.0/PI)) - 1.0;
    eh2.map = {(h.local_pt.y >= 0.0) ? u : -u, h.local_pt.z, 0.0};
  } else {
    // end cap hit
    eh2.map = h.local_pt;
  }

  return _child->evaluate(s, r, h, eh2);
}

Color MapParaboloidShader::evaluate(
  const Scene& s, const Ray& r, const HitInfo& h, const EvaluatedHit& eh) const
{
  EvaluatedHit eh2{
    eh.global_pt, eh.normal,
    {(h.local_pt.z > 0.0) ? h.local_pt.x : -h.local_pt.x, h.local_pt.y, 0.0}
  };
  return _child->evaluate(s, r, h, eh2);
}

Color MapSphereShader::evaluate(
  const Scene& s, const Ray& r, const HitInfo& h, const EvaluatedHit& eh) const
{
  EvaluatedHit eh2{
    eh.global_pt, eh.normal,
    {(h.local_pt.z > 0.0) ? h.local_pt.x : -h.local_pt.x, h.local_pt.y, 0.0}
  };
  return _child->evaluate(s, r, h, eh2);
}

Color MapTorusShader::evaluate(
  const Scene& s, const Ray& r, const HitInfo& h, const EvaluatedHit& eh) const
{
  EvaluatedHit eh2{
    eh.global_pt, eh.normal,
    {(h.local_pt.y >= 0.0) ? h.local_pt.x : -h.local_pt.x, -h.local_pt.z, 0.0}
  };
  return _child->evaluate(s, r, h, eh2);
}
