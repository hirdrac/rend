//
// Ray.cc
// Copyright (C) 2022 Richard Bradley
//

#include "Ray.hh"


// **** Functions ****
int CalcTransmitResult(
  Vec3& transmit, const Vec3& incident, const Vec3& normal,
  Flt i_index, Flt t_index)
{
  // alpha
  const Flt alpha = i_index / t_index;

  // beta
  const Flt dot = DotProduct(-normal, incident);
  const Flt root = 1.0 + (Sqr(alpha) * (Sqr(dot) - 1.0));
  if (root < 0) {
    return -1;  // internal refraction
  }

  const Flt beta = (alpha * dot) - std::sqrt(root);

  // result
  transmit = UnitVec((incident * alpha) + (normal * beta));
  return 0;
}
