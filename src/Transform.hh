//
// Transform.hh
// Copyright (C) 2022 Richard Bradley
//
// scene item transform state
//

#pragma once
#include "Ray.hh"
#include "Types.hh"


// **** Types ****
class Transform
{
 public:
  Matrix base{INIT_NONE};  // configured transform for item

  Transform() { clear(); }

  // Member Functions
  int init(const Transform* parent = nullptr);
  void clear();

  [[nodiscard]] const Matrix& final() const { return _final; }

  [[nodiscard]] bool noParent() const { return _noParent; }
  void setNoParent(bool v) { _noParent = v; }

  [[nodiscard]] inline Vec3 normalLocalToGlobal(const Vec3& n, Flt time) const;
  [[nodiscard]] inline Vec3 pointLocalToGlobal(const Vec3& pos, Flt time) const;

  [[nodiscard]] inline Vec2 pointLocalToGlobalXY(
    const Vec3& pos, Flt time) const;
  [[nodiscard]] inline Flt pointLocalToGlobalX(
    const Vec3& pos, Flt time) const;

  [[nodiscard]] inline Vec3 vectorLocalToGlobal(const Vec3& dir, Flt time) const;

  [[nodiscard]] inline Vec3 rayLocalDir(const Ray& r) const;
  [[nodiscard]] inline Vec3 rayLocalBase(const Ray& r) const;

    // FIXME: time/r.time ignored for now.
    //   eventually time parameter will allow for motion blur

 private:
  Matrix _final{INIT_NONE}; // base transform adjusted by parent transform
  Matrix _finalInv{INIT_NONE};
  bool _noParent = false;
};


// **** Inline Implementation ****
Vec3 Transform::normalLocalToGlobal(const Vec3& n, Flt time) const
{
  // global normal = local normal * transpose(inverse(global transform))
  return UnitVec(MultVectorTrans(n, _finalInv));
}

Vec3 Transform::pointLocalToGlobal(const Vec3& pos, Flt time) const
{
  return MultPoint(pos, _final);
}

Vec2 Transform::pointLocalToGlobalXY(const Vec3& pos, Flt time) const
{
  return MultPointXY(pos, _final);
}

Flt Transform::pointLocalToGlobalX(const Vec3& pos, Flt time) const
{
  return MultPointX(pos, _final);
}

Vec3 Transform::vectorLocalToGlobal(const Vec3& dir, Flt time) const
{
  return MultVector(dir, _final);
}

Vec3 Transform::rayLocalDir(const Ray& r) const
{
  return MultVector(r.dir, _finalInv);
}

Vec3 Transform::rayLocalBase(const Ray& r) const
{
  return MultPoint(r.base, _finalInv);
}
