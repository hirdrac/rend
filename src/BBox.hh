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

  [[nodiscard]] Vec3 xbase() const {
    return {pmin.x, (pmin.y + pmax.y) * .5, (pmin.z + pmax.z) * .5}; }
  [[nodiscard]] Vec3 ybase() const {
    return {(pmin.x + pmax.x) * .5, pmin.y, (pmin.z + pmax.z) * .5}; }
  [[nodiscard]] Vec3 zbase() const {
    return {(pmin.x + pmax.x) * .5, (pmin.y + pmax.y) * .5, pmin.z}; }

  [[nodiscard]] Vec3 xtop() const {
    return {pmax.x, (pmin.y + pmax.y) * .5, (pmin.z + pmax.z) * .5}; }
  [[nodiscard]] Vec3 ytop() const {
    return {(pmin.x + pmax.x) * .5, pmax.y, (pmin.z + pmax.z) * .5}; }
  [[nodiscard]] Vec3 ztop() const {
    return {(pmin.x + pmax.x) * .5, (pmin.y + pmax.y) * .5, pmax.z}; }

  [[nodiscard]] Flt xlength() const { return (pmax.x - pmin.x); }
  [[nodiscard]] Flt ylength() const { return (pmax.y - pmin.y); }
  [[nodiscard]] Flt zlength() const { return (pmax.z - pmin.z); }


  enum Spot { CENTER, XBASE, YBASE, ZBASE, XTOP, YTOP, ZTOP };

  [[nodiscard]] Vec3 operator()(Spot s) const {
    switch (s) {
      case XBASE: return xbase();
      case YBASE: return ybase();
      case ZBASE: return zbase();
      case XTOP:  return xtop();
      case YTOP:  return ytop();
      case ZTOP:  return ztop();
      default:    return center();
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
