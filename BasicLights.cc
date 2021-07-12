//
// BasicLights.cc
// Copyright (C) 2021 Richard Bradley
//

#include "BasicLights.hh"
#include "Intersect.hh"
#include "Shader.hh"
#include "Ray.hh"


// **** Sun Class ****
int Sun::luminate(
  const Scene& s, const Ray& r, const HitInfo& h, const EvaluatedHit& eh,
  LightResult& result) const
{
  result.dir = dir;
  result.distance = VERY_LARGE;
  return energy->evaluate(s, r, h, eh, result.energy);
  // FIXME - add shadow support to Sun::luminate()
}


// **** PointLight Class ****
int PointLight::luminate(
  const Scene& s, const Ray& r, const HitInfo& h, const EvaluatedHit& eh,
  LightResult& result) const
{
  result.dir = UnitVec(pos - eh.global_pt);
  result.distance = PointDistance(pos, eh.global_pt);
  energy->evaluate(s, r, h, eh, result.energy);
  return shadow_fn(
    s, r, eh.global_pt, result.dir, result.distance, result.energy);
}


// **** SpotLight Class ****
int SpotLight::luminate(
  const Scene& s, const Ray& r, const HitInfo& h, const EvaluatedHit& eh,
  LightResult& result) const
{
  // FIX - finish SpotLight::luminate()
  return -1;
}
