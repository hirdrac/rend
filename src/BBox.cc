//
// BBox.cc
// Copyright (C) 2021 Richard Bradley
//

#include "BBox.hh"
#include "Transform.hh"
#include <algorithm>


// **** BBox class ****
BBox::BBox(const BBox& b1, const BBox& b2)
{
  pmin.x = std::min(b1.pmin.x, b2.pmin.x);
  pmin.y = std::min(b1.pmin.y, b2.pmin.y);
  pmin.z = std::min(b1.pmin.z, b2.pmin.z);

  pmax.x = std::max(b1.pmax.x, b2.pmax.x);
  pmax.y = std::max(b1.pmax.y, b2.pmax.y);
  pmax.z = std::max(b1.pmax.z, b2.pmax.z);
}

void BBox::reset()
{
  pmin = { VERY_LARGE,  VERY_LARGE,  VERY_LARGE};
  pmax = {-VERY_LARGE, -VERY_LARGE, -VERY_LARGE};
}

Flt BBox::weight() const
{
  if (empty()) { return 0.0; }

  const Flt x = pmax.x - pmin.x;
  const Flt y = pmax.y - pmin.y;
  const Flt z = pmax.z - pmin.z;
  return (x * (y + z)) + (y * z);
}

void BBox::fit(const Vec3& p)
{
  pmin.x = std::min(pmin.x, p.x);
  pmin.y = std::min(pmin.y, p.y);
  pmin.z = std::min(pmin.z, p.z);

  pmax.x = std::max(pmax.x, p.x);
  pmax.y = std::max(pmax.y, p.y);
  pmax.z = std::max(pmax.z, p.z);
}

void BBox::fit(const BBox& box)
{
  pmin.x = std::min(pmin.x, box.pmin.x);
  pmin.y = std::min(pmin.y, box.pmin.y);
  pmin.z = std::min(pmin.z, box.pmin.z);

  pmax.x = std::max(pmax.x, box.pmax.x);
  pmax.y = std::max(pmax.y, box.pmax.y);
  pmax.z = std::max(pmax.z, box.pmax.z);
}

void BBox::fit(const Transform& t, const Vec3* pt_list, int pt_count)
{
  for (int i = 0; i < pt_count; ++i) {
    fit(t.pointLocalToGlobal(pt_list[i], 0));
  }
}

void BBox::intersect(const BBox& box)
{
  pmin.x = std::max(pmin.x, box.pmin.x);
  pmin.y = std::max(pmin.y, box.pmin.y);
  pmin.z = std::max(pmin.z, box.pmin.z);

  pmax.x = std::min(pmax.x, box.pmax.x);
  pmax.y = std::min(pmax.y, box.pmax.y);
  pmax.z = std::min(pmax.z, box.pmax.z);
}
