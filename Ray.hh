//
// Ray.hh
// Copyright (C) 2020 Richard Bradley
//
// Definition of ray class and functions
//

#pragma once
#include "Intersect.hh"
#include "Types.hh"


// **** Types ****
class Transform;

class Ray
{
 public:
  Vec3 base, dir;
  Flt  max_length, time;
  int  depth;
  DList<HitInfo>* freeCache = nullptr;

  // Member Functions
  void globalToLocal(const Transform& t, Vec3& base, Vec3& dir) const;
  void moveOut(Flt amount = .0001) { base += dir * amount; }

  // FIXME - add HitInfo free cache
};


// **** Functions ****
int CalcTransmitResult(
  Vec3& transmit, const Vec3& incident, const Vec3& normal,
  Flt i_index, Flt t_index);


// **** Inlined Functions ****
constexpr Vec3 CalcReflect(const Vec3& incident, const Vec3& normal)
{
  return (normal * (2.0 * DotProduct(-incident, normal))) + incident;
}

constexpr Vec3 CalcHitPoint(const Vec3& base, const Vec3& dir, Flt t)
{
  return base + (dir * t);
}
