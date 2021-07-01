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
  const Scene& s, const Ray& r, const HitInfo& h, const Vec3& normal,
  const Vec3& map, LightResult& result) const
{
  result.dir = dir;
  result.distance = VERY_LARGE;
  return energy->evaluate(s, r, h, normal, map, result.energy);
}


// **** PointLight Class ****
int PointLight::luminate(
  const Scene& s, const Ray& r, const HitInfo& h, const Vec3& normal,
  const Vec3& map, LightResult& result) const
{
  result.dir = UnitVec(pos - h.global_pt);
  result.distance = PointDistance(pos, h.global_pt);
  energy->evaluate(s, r, h, normal, map, result.energy);
  return shadow_fn(
    s, r, h.global_pt, result.dir, result.distance, result.energy);
}


// **** SpotLight Class ****
int SpotLight::luminate(
  const Scene& s, const Ray& r, const HitInfo& h, const Vec3& normal,
  const Vec3& map, LightResult& result) const
{
  // FIX - finish SpotLight::luminate()
  return -1;
}
