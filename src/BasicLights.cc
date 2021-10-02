//
// BasicLights.cc
// Copyright (C) 2021 Richard Bradley
//

#include "BasicLights.hh"
#include "Shader.hh"
#include "Scene.hh"


// **** Sun Class ****
int Sun::init(Scene& s)
{
  _finalDir = _trans.vectorLocalToGlobal(dir, 0);
  return 0;
}

bool Sun::luminate(JobState& js, const Scene& s, const Ray& r,
                   const EvaluatedHit& eh, LightResult& result) const
{
  const Vec3 unit_dir = -_finalDir;
  const Flt angle = DotProduct(eh.normal, unit_dir);
  if (!IsPositive(angle)) { return false; }

  if (s.shadow) {
    Ray sray;
    sray.base       = eh.global_pt;
    sray.dir        = unit_dir;
    sray.min_length = s.ray_moveout;
    sray.max_length = VERY_LARGE;
    sray.time       = r.time;
    sray.depth      = 0;

    if (s.castShadowRay(js, sray)) { return false; }
  }

  result.dir = unit_dir;
  result.distance = VERY_LARGE;
  result.angle = angle;
  result.energy = _energy->evaluate(js, s, r, eh);
  return true;
}


// **** PointLight Class ****
int PointLight::init(Scene& s)
{
  _finalPos = _trans.pointLocalToGlobal(pos, 0);
  return 0;
}

bool PointLight::luminate(JobState& js, const Scene& s, const Ray& r,
                          const EvaluatedHit& eh, LightResult& result) const
{
  const Vec3 dir = _finalPos - eh.global_pt;
  const Flt len = dir.length();
  const Vec3 unit_dir = dir * (1.0 / len);
  const Flt angle = DotProduct(eh.normal, unit_dir);
  if (!IsPositive(angle)) { return false; }

  if (s.shadow) {
    Ray sray;
    sray.base       = eh.global_pt;
    sray.dir        = unit_dir;
    sray.min_length = s.ray_moveout;
    sray.max_length = len;
    sray.time       = r.time;
    sray.depth      = 0;

    if (s.castShadowRay(js, sray)) { return false; }
  }

  result.dir = unit_dir;
  result.distance = len;
  result.angle = angle;
  result.energy = _energy->evaluate(js, s, r, eh);
  return true;
}


// **** SpotLight Class ****
int SpotLight::init(Scene& s)
{
  _finalPos = _trans.pointLocalToGlobal(pos, 0);
  _finalDir = _trans.vectorLocalToGlobal(dir, 0);
  return 0;
}

bool SpotLight::luminate(JobState& js, const Scene& s, const Ray& r,
                         const EvaluatedHit& eh, LightResult& result) const
{
  // FIX - finish SpotLight::luminate()
  return false;
}
