//
// MapShaders.cc
// Copyright (C) 2025 Richard Bradley
//

#include "MapShaders.hh"
#include "RegisterShader.hh"
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

int MapShader::init(Scene& s, const Transform* tr)
{
  return _child ? s.initShader(*_child, tr) : -1;
}


// **** MapGlobalShader Class ****
REGISTER_SHADER_CLASS(MapGlobalShader,"map_global");

Color MapGlobalShader::evaluate(
  JobState& js, const Scene& s, const Ray& r, const EvaluatedHit& eh) const
{
  EvaluatedHit eh2{eh.global_pt, eh.normal, eh.global_pt, eh.side};
  return _child->evaluate(js, s, r, eh2);
}


// **** MapCodeShader Class ****
REGISTER_SHADER_CLASS(MapConeShader,"map_cone");

Color MapConeShader::evaluate(
  JobState& js, const Scene& s, const Ray& r, const EvaluatedHit& eh) const
{
  EvaluatedHit eh2;
  eh2.global_pt = eh.global_pt;
  eh2.normal = eh.normal;
  if (eh.side == 1) {
    // base
    eh2.map = {-(eh.map.x), eh.map.y, -1.0};
  } else {
    // side
    const Vec2 dir = UnitVec(eh.map.x, eh.map.y);
    const Flt x = std::clamp(dir.x, -1.0 + VERY_SMALL, 1.0 - VERY_SMALL);
    const Flt u = (std::acos(x) * (2.0/PI)) - 1.0;
    eh2.map = {(eh.map.y >= 0.0) ? u : -u, eh.map.z, 0.0};
  }
  eh2.side = eh.side;

  return _child->evaluate(js, s, r, eh2);
}

// **** MapCubeShader Class ****
REGISTER_SHADER_CLASS(MapCubeShader,"map_cube");

Color MapCubeShader::evaluate(
  JobState& js, const Scene& s, const Ray& r, const EvaluatedHit& eh) const
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

  return _child->evaluate(js, s, r, eh2);
}

// **** MapCylinderShader Class ****
REGISTER_SHADER_CLASS(MapCylinderShader,"map_cylinder");

Color MapCylinderShader::evaluate(
  JobState& js, const Scene& s, const Ray& r, const EvaluatedHit& eh) const
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

  return _child->evaluate(js, s, r, eh2);
}

// **** MapParaboloidShader Class ****
REGISTER_SHADER_CLASS(MapParaboloidShader,"map_paraboloid");

Color MapParaboloidShader::evaluate(
  JobState& js, const Scene& s, const Ray& r, const EvaluatedHit& eh) const
{
  EvaluatedHit eh2{
    eh.global_pt, eh.normal,
    {(eh.map.z > 0.0) ? eh.map.x : -eh.map.x, eh.map.y, 0.0},
    eh.side
  };
  return _child->evaluate(js, s, r, eh2);
}

// **** MapSphereShader Class ****
REGISTER_SHADER_CLASS(MapSphereShader,"map_sphere");

Color MapSphereShader::evaluate(
  JobState& js, const Scene& s, const Ray& r, const EvaluatedHit& eh) const
{
  EvaluatedHit eh2{
    eh.global_pt, eh.normal,
    {(eh.map.z > 0.0) ? eh.map.x : -eh.map.x, eh.map.y, 0.0},
    eh.side
  };
  return _child->evaluate(js, s, r, eh2);
}

// **** MapTorusShader Class ****
REGISTER_SHADER_CLASS(MapTorusShader,"map_torus");

Color MapTorusShader::evaluate(
  JobState& js, const Scene& s, const Ray& r, const EvaluatedHit& eh) const
{
  EvaluatedHit eh2{
    eh.global_pt, eh.normal,
    {(eh.map.y >= 0.0) ? eh.map.x : -eh.map.x, -eh.map.z, 0.0},
    eh.side
  };
  return _child->evaluate(js, s, r, eh2);
}
