//
// BBox.hh
// Copyright (C) 2021 Richard Bradley
//
// Axis-aligned bounding box
//

#pragma once
#include "Types.hh"

class Transform;


class BBox
{
 public:
  Vec3 pmin, pmax;

  BBox() { reset(); }
  BBox(const Vec3& pt) : pmin(pt), pmax(pt) { }
  BBox(const BBox& b1, const BBox& b2);
  BBox(const Vec3* pt_list, int pt_count);
  BBox(const Vec3* pt_list, int pt_count, const Transform& t);

  // Member Functions
  inline void reset();
  [[nodiscard]] Flt weight() const;

  void fit(const Vec3& pt);
  void fit(const BBox& box);

  void intersect(const BBox& box);

  [[nodiscard]] inline bool empty() const;

  [[nodiscard]] Vec3 center() const { return (pmin + pmax) / 2.0; }
  [[nodiscard]] Flt lengthX() const { return (pmax.x - pmin.x); }
  [[nodiscard]] Flt lengthY() const { return (pmax.y - pmin.y); }
  [[nodiscard]] Flt lengthZ() const { return (pmax.z - pmin.z); }
};


void BBox::reset()
{
  pmin = { VERY_LARGE,  VERY_LARGE,  VERY_LARGE};
  pmax = {-VERY_LARGE, -VERY_LARGE, -VERY_LARGE};
}

bool BBox::empty() const
{
  return IsGreater(pmin.x, pmax.x) || IsGreater(pmin.y, pmax.y)
    || IsGreater(pmin.z, pmax.z);
}
