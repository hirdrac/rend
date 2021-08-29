//
// MapShaders.cc
// Copyright (C) 2021 Richard Bradley
//

#include "MapShaders.hh"
#include <algorithm>
#include <cmath>
#include <cassert>


// **** MapShader Class ****
int MapShader::addShader(const ShaderPtr& sh, SceneItemFlag flag)
{
  assert(sh != nullptr);
  if (_child || flag != FLAG_NONE) { return -1; }

  _child = sh;
  return 0;
}

int MapShader::init(Scene& s)
{
  return _child ? InitShader(s, *_child) : -1;
}


// **** General Map Shaders ****
Color MapGlobalShader::evaluate(
  const Scene& s, const Ray& r, const EvaluatedHit& eh) const
{
  EvaluatedHit eh2{eh.global_pt, eh.normal, eh.global_pt, eh.side};
  return _child->evaluate(s, r, eh2);
}


// **** Object Map Shaders ****
Color MapConeShader::evaluate(
  const Scene& s, const Ray& r, const EvaluatedHit& eh) const
{
  EvaluatedHit eh2;
  eh2.global_pt = eh.global_pt;
  eh2.normal = eh.normal;
  if (eh.side == 1) {
    // base
    eh2.map = {-(eh.map.x), eh.map.y, -1.0};
  } else {
    // side
    const Vec2 dir = UnitVec(Vec2{eh.map.x, eh.map.y});
    const Flt x = std::clamp(dir.x, -1.0 + VERY_SMALL, 1.0 - VERY_SMALL);
    const Flt u = (std::acos(x) * (2.0/PI)) - 1.0;
    eh2.map = {(eh.map.y >= 0.0) ? u : -u, eh.map.z, 0.0};
  }
  eh2.side = eh.side;

  return _child->evaluate(s, r, eh2);
}

Color MapCubeShader::evaluate(
  const Scene& s, const Ray& r, const EvaluatedHit& eh) const
{
  EvaluatedHit eh2;
  eh2.global_pt = eh.global_pt;
  eh2.normal = eh.normal;
  switch (eh.side) {
    case 0:  eh2.map = {-eh.map.z,  eh.map.y, 0.0}; break;
    case 1:  eh2.map = { eh.map.z,  eh.map.y, 0.0}; break;
    case 2:  eh2.map = { eh.map.x, -eh.map.z, 0.0}; break;
    case 3:  eh2.map = { eh.map.x,  eh.map.z, 0.0}; break;
    case 4:  eh2.map = { eh.map.x,  eh.map.y, 0.0}; break;
    case 5:  eh2.map = {-eh.map.x,  eh.map.y, 0.0}; break;
    default: eh2.map = eh.map; break;
  }
  eh2.side = eh.side;

  return _child->evaluate(s, r, eh2);
}

Color MapCylinderShader::evaluate(
  const Scene& s, const Ray& r, const EvaluatedHit& eh) const
{
  EvaluatedHit eh2;
  eh2.global_pt = eh.global_pt;
  eh2.normal = eh.normal;
  if (eh.side == 0) {
    // side hit
    const Flt x =
      std::clamp(eh.map.x, -1.0 + VERY_SMALL, 1.0 - VERY_SMALL);
    const Flt u = (std::acos(x) * (2.0/PI)) - 1.0;
    eh2.map = {(eh.map.y >= 0.0) ? u : -u, eh.map.z, 0.0};
  } else {
    // end cap hit
    eh2.map = eh.map;
  }
  eh2.side = eh.side;

  return _child->evaluate(s, r, eh2);
}

Color MapParaboloidShader::evaluate(
  const Scene& s, const Ray& r, const EvaluatedHit& eh) const
{
  EvaluatedHit eh2{
    eh.global_pt, eh.normal,
    {(eh.map.z > 0.0) ? eh.map.x : -eh.map.x, eh.map.y, 0.0},
    eh.side
  };
  return _child->evaluate(s, r, eh2);
}

Color MapSphereShader::evaluate(
  const Scene& s, const Ray& r, const EvaluatedHit& eh) const
{
  EvaluatedHit eh2{
    eh.global_pt, eh.normal,
    {(eh.map.z > 0.0) ? eh.map.x : -eh.map.x, eh.map.y, 0.0},
    eh.side
  };
  return _child->evaluate(s, r, eh2);
}

Color MapTorusShader::evaluate(
  const Scene& s, const Ray& r, const EvaluatedHit& eh) const
{
  EvaluatedHit eh2{
    eh.global_pt, eh.normal,
    {(eh.map.y >= 0.0) ? eh.map.x : -eh.map.x, -eh.map.z, 0.0},
    eh.side
  };
  return _child->evaluate(s, r, eh2);
}
