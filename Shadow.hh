//
// Shadow.hh
// Copyright (C) 2020 Richard Bradley
//
// Shadow calculation functions
//

#pragma once
#include "Types.hh"


// **** Types ****
class Scene;
class Ray;
class Color;

typedef int (*ShadowFn)
(const Scene& s, const Ray& r, const Vec3& globalPt,
 const Vec3& lightDir, Flt lightDist, Color& energy);


// **** Functions ****
int CastShadow(
  const Scene& s, const Ray& r, const Vec3& globalPt,
  const Vec3& lightDir, Flt lightDist, Color& energy);
int CastNoShadow(
  const Scene& s, const Ray& r, const Vec3& globalPt,
  const Vec3& lightDir, Flt lightDist, Color& energy);
