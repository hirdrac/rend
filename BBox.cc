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
  _min.x = std::min(b1._min.x, b2._min.x);
  _min.y = std::min(b1._min.y, b2._min.y);
  _min.z = std::min(b1._min.z, b2._min.z);

  _max.x = std::max(b1._max.x, b2._max.x);
  _max.y = std::max(b1._max.y, b2._max.y);
  _max.z = std::max(b1._max.z, b2._max.z);
}

void BBox::reset()
{
  _min.set( VERY_LARGE,  VERY_LARGE,  VERY_LARGE);
  _max.set(-VERY_LARGE, -VERY_LARGE, -VERY_LARGE);
}

Flt BBox::weight() const
{
  if (empty()) { return 0.0; }

  const Flt x = _max.x - _min.x;
  const Flt y = _max.y - _min.y;
  const Flt z = _max.z - _min.z;
  return (x * (y + z)) + (y * z);
}

void BBox::fit(const Vec3& p)
{
  _min.x = std::min(_min.x, p.x);
  _min.y = std::min(_min.y, p.y);
  _min.z = std::min(_min.z, p.z);

  _max.x = std::max(_max.x, p.x);
  _max.y = std::max(_max.y, p.y);
  _max.z = std::max(_max.z, p.z);
}

void BBox::fit(const BBox& box)
{
  _min.x = std::min(_min.x, box._min.x);
  _min.y = std::min(_min.y, box._min.y);
  _min.z = std::min(_min.z, box._min.z);

  _max.x = std::max(_max.x, box._max.x);
  _max.y = std::max(_max.y, box._max.y);
  _max.z = std::max(_max.z, box._max.z);
}

void BBox::fit(const Transform& t, const Vec3* pt_list, int pt_count)
{
  for (int i = 0; i < pt_count; ++i) {
    fit(MultPoint(pt_list[i], t.global));
  }
}

void BBox::intersect(const BBox& box)
{
  _min.x = std::max(_min.x, box._min.x);
  _min.y = std::max(_min.y, box._min.y);
  _min.z = std::max(_min.z, box._min.z);

  _max.x = std::min(_max.x, box._max.x);
  _max.y = std::min(_max.y, box._max.y);
  _max.z = std::min(_max.z, box._max.z);
}
