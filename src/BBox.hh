//
// BBox.hh
// Copyright (C) 2021 Richard Bradley
//
// Axis-aligned bounding box
//

#pragma once
#include "Types.hh"


class BBox
{
 public:
  Vec3 pmin, pmax;

  BBox() { reset(); }
  BBox(const Vec3& pt) : pmin(pt), pmax(pt) { }
  BBox(const BBox& b1, const BBox& b2);
  BBox(const Vec3* pt_list, int pt_count);
  BBox(const Vec3* pt_list, int pt_count, const Matrix& t);

  // Member Functions
  inline void reset();
  [[nodiscard]] Flt weight() const;

  void fit(const Vec3& pt);
  void fit(const BBox& box);

  void intersect(const BBox& box);

  [[nodiscard]] inline bool empty() const;

  [[nodiscard]] Vec3 center() const { return (pmin + pmax) * .5; }

  [[nodiscard]] Vec3 baseX() const {
    return {pmin.x, (pmin.y + pmax.y) * .5, (pmin.z + pmax.z) * .5}; }
  [[nodiscard]] Vec3 baseY() const {
    return {(pmin.x + pmax.x) * .5, pmin.y, (pmin.z + pmax.z) * .5}; }
  [[nodiscard]] Vec3 baseZ() const {
    return {(pmin.x + pmax.x) * .5, (pmin.y + pmax.y) * .5, pmin.z}; }

  [[nodiscard]] Vec3 topX() const {
    return {pmax.x, (pmin.y + pmax.y) * .5, (pmin.z + pmax.z) * .5}; }
  [[nodiscard]] Vec3 topY() const {
    return {(pmin.x + pmax.x) * .5, pmax.y, (pmin.z + pmax.z) * .5}; }
  [[nodiscard]] Vec3 topZ() const {
    return {(pmin.x + pmax.x) * .5, (pmin.y + pmax.y) * .5, pmax.z}; }

  [[nodiscard]] Flt lengthX() const { return (pmax.x - pmin.x); }
  [[nodiscard]] Flt lengthY() const { return (pmax.y - pmin.y); }
  [[nodiscard]] Flt lengthZ() const { return (pmax.z - pmin.z); }


  enum Spot { CENTER, BASE_X, BASE_Y, BASE_Z, TOP_X, TOP_Y, TOP_Z };

  [[nodiscard]] Vec3 operator()(Spot s) const {
    switch (s) {
      case BASE_X: return baseX();
      case BASE_Y: return baseY();
      case BASE_Z: return baseZ();
      case TOP_X:  return topX();
      case TOP_Y:  return topY();
      case TOP_Z:  return topZ();
      default:     return center();
    }
  }
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
