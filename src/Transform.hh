//
// Transform.hh
// Copyright (C) 2021 Richard Bradley
//
// object/shader transformation state
//

#pragma once
#include "Types.hh"
#include "Ray.hh"


// **** Types ****
class Transform
{
 public:
  Matrix base;  // configured transform for item

  Transform() { clear(); }

  // Member Functions
  int init(const Transform* parent = nullptr);
  void clear();

  [[nodiscard]] inline Vec3 normalLocalToGlobal(const Vec3& normal, Flt time) const;
  [[nodiscard]] inline Vec3 pointLocalToGlobal(const Vec3& pt, Flt time) const;

  [[nodiscard]] inline Vec3 rayLocalDir(const Ray& r) const;
  [[nodiscard]] inline Vec3 rayLocalBase(const Ray& r) const;

    // FIXME - time/r.time ignored for now.
    //   eventually time parameter will allow for motion blur

 private:
  Matrix _final; // base transform adjusted by parent transform
  Matrix _finalInv;
};


// **** Inline Implementation ****
Vec3 Transform::normalLocalToGlobal(const Vec3& normal, Flt time) const
{
  // global normal = local normal * transpose(inverse(global transform))
  return UnitVec(MultVectorTrans(normal, _finalInv));
}

Vec3 Transform::pointLocalToGlobal(const Vec3& pt, Flt time) const
{
  return MultPoint(pt, _final);
}

Vec3 Transform::rayLocalDir(const Ray& r) const
{
  return MultVector(r.dir, _finalInv);
}

Vec3 Transform::rayLocalBase(const Ray& r) const
{
  return MultPoint(r.base, _finalInv);
}
