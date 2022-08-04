//
// Ray.hh
// Copyright (C) 2022 Richard Bradley
//
// Definition of ray class and functions
//

#pragma once
#include "Types.hh"


// **** Types ****
class Ray
{
 public:
  // base ray properties
  Vec3 base{INIT_NONE};
  Vec3 dir{INIT_NONE};
  Flt  min_length;
  Flt  max_length;
  Flt  time;
  int  depth;

  // Member Functions
  void moveOut(Flt amount) { base += dir * amount; }

  [[nodiscard]] bool inRange(Flt t) const {
    return (t >= min_length) && (t < max_length); }
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
