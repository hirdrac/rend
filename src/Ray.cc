//
// Ray.cc
// Copyright (C) 2026 Richard Bradley
//

#include "Ray.hh"


// **** Functions ****
bool CalcTransmitResult(
  Vec3& transmit, const Vec3& incident, const Vec3& normal,
  Flt i_index, Flt t_index)
{
  // alpha
  const Flt alpha = i_index / t_index;

  // beta
  const Flt dot = dotProduct(-normal, incident);
  const Flt root = 1.0 + (sqr(alpha) * (sqr(dot) - 1.0));
  if (root < 0) {
    return false;  // internal refraction
  }

  const Flt beta = (alpha * dot) - std::sqrt(root);

  // result
  transmit = unitVec((incident * alpha) + (normal * beta));
  return true;
}
