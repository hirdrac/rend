//
// Shadow.hh
// Copyright (C) 2021 Richard Bradley
//
// Shadow calculation functions
//

#pragma once
#include "Color.hh"
#include "Types.hh"


// **** Types ****
class Scene;
class Ray;

struct LightResult {
  Color energy;
  Vec3 dir;
  Flt distance;
};

using ShadowFn =
  int(*)(const Scene& s, const Ray& r, const Vec3& globalPt, LightResult& light);


// **** Functions ****
int CastShadow(
  const Scene& s, const Ray& r, const Vec3& globalPt, LightResult& light);
int CastNoShadow(
  const Scene& s, const Ray& r, const Vec3& globalPt, LightResult& light);
