//
// BasicLights.cc
// Copyright (C) 2021 Richard Bradley
//

#include "BasicLights.hh"
#include "Intersect.hh"
#include "Shader.hh"


// **** Sun Class ****
int Sun::luminate(
  const Scene& s, const Ray& r, const HitInfo& h, const EvaluatedHit& eh,
  LightResult& result) const
{
  result.dir = -dir;
  result.distance = VERY_LARGE;
  return _energy->evaluate(s, r, h, eh, result.energy);
}


// **** PointLight Class ****
int PointLight::luminate(
  const Scene& s, const Ray& r, const HitInfo& h, const EvaluatedHit& eh,
  LightResult& result) const
{
  result.dir = UnitVec(pos - eh.global_pt);
  result.distance = PointDistance(pos, eh.global_pt);
  return _energy->evaluate(s, r, h, eh, result.energy);
}


// **** SpotLight Class ****
int SpotLight::luminate(
  const Scene& s, const Ray& r, const HitInfo& h, const EvaluatedHit& eh,
  LightResult& result) const
{
  // FIX - finish SpotLight::luminate()
  return -1;
}
