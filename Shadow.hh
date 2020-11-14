//
// Shadow.hh - revision 9 (2019/1/1)
// Copyright(C) 2019 by Richard Bradley
//
// Shadow calculation functions
//

#ifndef Shadow_hh
#define Shadow_hh

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

#endif
