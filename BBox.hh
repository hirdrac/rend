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
  BBox(const BBox& b1, const BBox& b2);

  // Member Functions
  void reset();
  [[nodiscard]] Flt weight() const;

  void fit(const Vec3& pt);
  void fit(const BBox& box);
  void fit(const Transform& t, const Vec3* pt_list, int pt_count);

  [[nodiscard]] bool empty() const {
    return IsGreater(pmin.x, pmax.x) || IsGreater(pmin.y, pmax.y)
      || IsGreater(pmin.z, pmax.z); }
};
