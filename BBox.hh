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
  BBox() { reset(); }
  BBox(const Vec3& pt) : _min(pt), _max(pt) { }
  BBox(const BBox& b1, const BBox& b2);

  // Member Functions
  void reset();
  [[nodiscard]] Flt weight() const;

  [[nodiscard]] const Vec3& min() const { return _min; }
  [[nodiscard]] const Vec3& max() const { return _max; }

  void fit(const Vec3& pt);
  void fit(const BBox& box);
  void fit(const Transform& t, const Vec3* pt_list, int pt_count);

  void intersect(const BBox& box);

  [[nodiscard]] bool empty() const {
    return IsGreater(_min.x, _max.x) || IsGreater(_min.y, _max.y)
      || IsGreater(_min.z, _max.z); }

 private:
  Vec3 _min, _max;
};
