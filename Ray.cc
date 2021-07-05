//
// Ray.cc
// Copyright (C) 2021 Richard Bradley
//
// Implementation of ray class and functions
//

#include "Ray.hh"
#include "Transform.hh"


// **** Ray Class ****
void Ray::globalToLocal(const Transform& t, Vec3& new_base, Vec3& new_dir) const
{
  const Matrix& global_inv = t.GlobalInv(time);
  new_dir = MultVector(dir, global_inv);
  new_base = MultPoint(base, global_inv);
}


// **** Functions ****
int CalcTransmitResult(
  Vec3& transmit, const Vec3& incident, const Vec3& normal,
  Flt i_index, Flt t_index)
{
  // alpha
  Flt alpha = i_index / t_index;

  // beta
  Flt dot = DotProduct(-normal, incident);
  Flt root = 1.0 + (Sqr(alpha) * (Sqr(dot) - 1.0));
  if (root < 0) {
    return -1;  // internal refraction
  }

  Flt beta = (alpha * dot) - std::sqrt(root);

  // result
  transmit = UnitVec((incident * alpha) + (normal * beta));
  return 0;
}
