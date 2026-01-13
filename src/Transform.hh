//
// Transform.hh
// Copyright (C) 2026 Richard Bradley
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
  Matrix base;  // configured transform for item

  Transform() { clear(); }

  // Member Functions
  int init(const Transform* parent = nullptr);
  void clear();

  [[nodiscard]] const Matrix& final() const { return _final; }

  [[nodiscard]] bool noParent() const { return _noParent; }
  void setNoParent(bool v) { _noParent = v; }

  [[nodiscard]] inline Vec3 normalLocalToGlobal(const Vec3& n) const;
  [[nodiscard]] inline Vec3 pointLocalToGlobal(const Vec3& pos) const;
  [[nodiscard]] inline Vec2 pointLocalToGlobalXY(const Vec3& pos) const;
  [[nodiscard]] inline Flt pointLocalToGlobalX(const Vec3& pos) const;
  [[nodiscard]] inline Vec3 vectorLocalToGlobal(const Vec3& dir) const;
    // TODO: add time parameter to support motion blur/animation

  [[nodiscard]] inline Vec3 rayLocalDir(const Ray& r) const;
  [[nodiscard]] inline Vec3 rayLocalBase(const Ray& r) const;

 private:
  Matrix _final;  // base transform adjusted by parent transform
  Matrix _finalInv;
  bool _noParent = false;
};


// **** Inline Implementation ****
Vec3 Transform::normalLocalToGlobal(const Vec3& n) const
{
  // global normal = local normal * transpose(inverse(global transform))
  return unitVec(multVectorTrans(n, _finalInv));
}

Vec3 Transform::pointLocalToGlobal(const Vec3& pos) const
{
  return multPoint(pos, _final);
}

Vec2 Transform::pointLocalToGlobalXY(const Vec3& pos) const
{
  return multPointXY(pos, _final);
}

Flt Transform::pointLocalToGlobalX(const Vec3& pos) const
{
  return multPointX(pos, _final);
}

Vec3 Transform::vectorLocalToGlobal(const Vec3& dir) const
{
  return multVector(dir, _final);
}

Vec3 Transform::rayLocalDir(const Ray& r) const
{
  return multVector(r.dir, _finalInv);
}

Vec3 Transform::rayLocalBase(const Ray& r) const
{
  return multPoint(r.base, _finalInv);
}
