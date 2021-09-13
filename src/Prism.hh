//
// Prism.hh
// Copyright (C) 2021 Richard Bradley
//
// (Right) Prism
// (n-sided top/base, rectangle sides)
// https://en.wikipedia.org/wiki/Prism_(geometry)
//

#pragma once
#include "Object.hh"
#include <vector>


class Prism final : public Primitive
{
 public:
  // SceneItem Functions
  std::string desc() const override;
  int setSides(int s) override { _sides = s; return 0; }

  // Object Functions
  int init(Scene& s) override;
  BBox bound(const Matrix* t = nullptr) const override;
  int intersect(const Ray& r, HitList& hit_list) const override;

  // Primitive Functions
  Flt hitCost() const override;
  Vec3 normal(const Ray& r, const HitInfo& h) const override;

 private:
  std::vector<Vec2> _plane; // A,B,(C=0) plane normals (D=1.0)
  std::vector<Vec3> _normal; // transformed cached normals
  Flt _halfSideLenSqr;
  int _sides = 0;
};
