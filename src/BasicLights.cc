//
// BasicLights.cc
// Copyright (C) 2026 Richard Bradley
//

#include "BasicLights.hh"
#include "Shader.hh"
#include "Scene.hh"
#include "RegisterLight.hh"


// **** Sun Class ****
REGISTER_LIGHT_CLASS(Sun,"sun");

int Sun::init(Scene& s)
{
  _finalDir = unitVec(dir);
  return 0;
}

bool Sun::luminate(JobState& js, const Scene& s, const Ray& r,
                   const EvaluatedHit& eh, LightResult& result) const
{
  const Vec3 unit_dir = -_finalDir;
  const Flt angle = dotProduct(eh.normal, unit_dir);
  if (!IsPositive(angle)) { return false; }

  if (s.shadow) {
    Ray sray {
      .base       = eh.global_pt,
      .dir        = unit_dir,
      .min_length = s.ray_moveout
    };

    if (s.castShadowRay(js, sray)) { return false; }
  }

  result.dir = unit_dir;
  result.distance = VERY_LARGE;
  result.angle = angle;
  result.energy = _energy->evaluate(js, s, r, eh);
  return true;
}


// **** PointLight Class ****
REGISTER_LIGHT_CLASS(PointLight,"light");

int PointLight::init(Scene& s)
{
  _finalPos = _trans.pointLocalToGlobal({0,0,0});
  if (!IsPositive(_radius)) { _samples = 1; }
  else { _samples = std::max(_samples, 1); }
  return 0;
}

bool PointLight::luminate(JobState& js, const Scene& s, const Ray& r,
                          const EvaluatedHit& eh, LightResult& result) const
{
  const Vec3 dir = _finalPos - eh.global_pt;
  const Flt len = dir.length();
  const Vec3 unit_dir = dir * (1.0 / len);
  const Flt angle = dotProduct(eh.normal, unit_dir);
  if (!IsPositive(angle)) { return false; }

  if (s.shadow) {
    Ray sray {
      .base       = eh.global_pt,
      .dir        = unit_dir,
      .min_length = s.ray_moveout,
      .max_length = len
    };

    if (s.castShadowRay(js, sray)) { return false; }
  }

  result.dir = unit_dir;
  result.distance = len;
  result.angle = angle;
  result.energy = _energy->evaluate(js, s, r, eh);
  return true;
}


// **** SpotLight Class ****
REGISTER_LIGHT_CLASS(SpotLight,"spotlight");

int SpotLight::init(Scene& s)
{
  _finalPos = _trans.pointLocalToGlobal({0,0,0});
  _finalDir = _trans.vectorLocalToGlobal(dir);
  return 0;
}

bool SpotLight::luminate(JobState& js, const Scene& s, const Ray& r,
                         const EvaluatedHit& eh, LightResult& result) const
{
  // FIXME: finish SpotLight::luminate()
  return false;
}
