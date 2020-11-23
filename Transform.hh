//
// Transform.hh
// Copyright (C) 2020 Richard Bradley
//
// definition of motion/tranformation class
//

#pragma once
#include "Types.hh"
#include <iosfwd>

// ADD - animation


// **** Types ****
class Transform
{
public:
  Matrix local, global, global_inv;

  // Constructor
  Transform() { clear(); }

  // Member Functions
  int init();
  void clear();

  const Matrix& Local(Flt time) const { return local; }
  const Matrix& Global(Flt time) const { return global; }
  const Matrix& GlobalInv(Flt time) const { return global_inv; }
    // FIXME - time ignored for now, eventually time parameter
    // will allow for animation support

  inline Vec3 normalLocalToGlobal(const Vec3& normal, Flt time) const;
};


// **** Functions ****
std::ostream& operator<<(std::ostream& out, const Transform& t);


// **** Inline Implementation ****
inline Vec3 Transform::normalLocalToGlobal(const Vec3& normal, Flt time) const
{
  // global normal = local normal * transpose(inverse(global transform))
  return UnitVec(MultVectorTrans(normal, GlobalInv(time)));
}
