//
// Shadow.cc
// Copyright (C) 2020 Richard Bradley
//
// Implementation of shadow module
//

#include "Shadow.hh"
#include "Ray.hh"
#include "Scene.hh"


// **** Functions ****
int CastShadow(
  const Scene& s, const Ray& r, const Vec3& globalPt,
  const Vec3& lightDir, Flt lightDist, Color& energy)
{
  Ray feeler;
  feeler.base       = globalPt;
  feeler.dir        = lightDir;
  feeler.max_length = lightDist;
  feeler.time       = r.time;
  feeler.depth      = 1;
  feeler.freeCache  = r.freeCache;
  feeler.moveOut();

  Color result;
  s.traceShadowRay(feeler, result);
  energy *= result;
  return 0;
}

int CastNoShadow(
  const Scene& s, const Ray& r, const Vec3& globalPt,
  const Vec3& lightDir, Flt lightDist, Color& energy)
{
  return 0;
}