//
// Ray.hh
// Copyright (C) 2021 Richard Bradley
//
// Definition of ray class and functions
//

#pragma once
#include "Types.hh"


// **** Types ****
class Transform;
class HitCache;
class StatInfo;

class Ray
{
 public:
  // base ray properties
  Vec3 base;
  Vec3 dir; // NOTE: dir should be normalized.  While not required for object
            //   intersections, lighting & reflection calculations require it.
  Flt  min_length;
  Flt  max_length;
  Flt  time;
  int  depth;

  // job specific buffers
  HitCache* freeCache = nullptr;
  StatInfo* stats = nullptr;

  // Member Functions
  void moveOut(Flt amount) { base += dir * amount; }
};


// **** Functions ****
int CalcTransmitResult(
  Vec3& transmit, const Vec3& incident, const Vec3& normal,
  Flt i_index, Flt t_index);


// **** Inlined Functions ****
[[nodiscard]] constexpr Vec3 CalcReflect(
  const Vec3& incident, const Vec3& normal)
{
  // NOTE: output is normalized if both inputs are
  return (normal * (2.0 * DotProduct(-incident, normal))) + incident;

  // alternate version
  //return incident - (normal * (2.0 * DotProduct(incident, normal)));
}

[[nodiscard]] constexpr Vec3 CalcHitPoint(
  const Vec3& base, const Vec3& dir, Flt t)
{
  return base + (dir * t);
}
