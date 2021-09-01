//
// Prism.cc
// Copyright (C) 2021 Richard Bradley
//

#include "Prism.hh"
#include "Intersect.hh"
#include "BBox.hh"
#include "Stats.hh"
#include <sstream>


std::string Prism::desc() const
{
  std::ostringstream os;
  os << "<Prism-" << _sides << ">";
  return os.str();
}

int Prism::init(Scene& s)
{
  if (_sides < 3 || _sides > 360) { return -1; }

  _plane.clear();
  _plane.reserve(std::size_t(_sides));
  for (int i = 0; i < _sides; ++i) {
    // side plane normal (D=1.0)
    const Flt angle = ((math::PI<Flt>*2.0) / Flt(_sides)) * Flt(i);
    _plane.push_back({std::sin(angle), std::cos(angle)});
  }

  _normal.clear();
  _normal.reserve(std::size_t(_sides + 2));
  for (const Vec2& n : _plane) {
    _normal.push_back(_trans.normalLocalToGlobal({n.x, n.y, 0}, 0));
  }

  _normal.push_back(_trans.normalLocalToGlobal({0, 0, 1}, 0));
  _normal.push_back(_trans.normalLocalToGlobal({0, 0,-1}, 0));

  _halfSideLenSqr = Sqr(std::tan(math::PI<Flt>/Flt(_sides)));
  return 0;
}

BBox Prism::bound(const Matrix* t) const
{
  const Matrix& m = t ? *t : _trans.final();
  BBox box;

  const Flt len = std::sqrt(1.0 + _halfSideLenSqr); // length of hypotenuse
  for (int i = 0; i < _sides; ++i) {
    const Flt a = ((math::PI<Flt>*2.0) / Flt(_sides)) * (Flt(i)+.5);
    const Flt x = std::sin(a) * len;
    const Flt y = std::cos(a) * len;
    box.fit(MultPoint({x,y, 1}, m));
    box.fit(MultPoint({x,y,-1}, m));
  }

  return box;
}

Flt Prism::hitCost() const
{
  return (_cost >= 0.0) ? _cost : 1.0 + (.2 * _sides);
}

int Prism::intersect(const Ray& r, HitList& hit_list) const
{
  ++r.stats->prism.tried;

  const Vec3 dir = _trans.rayLocalDir(r);
  const Vec3 base = _trans.rayLocalBase(r);

  Flt near_h = VERY_LARGE, far_h = -VERY_LARGE;
  int near_s = -1, far_s = -1;

  for (int i = 0; i < _sides; ++i) {
    const Vec2& n = _plane[std::size_t(i)];
    const Flt vd = (n.x * dir.x) + (n.y * dir.y); // DotProduct(n, dir);
    if (vd == 0.0) { continue; } // ray parallel to plane

    const Flt h = (1.0 - ((n.x * base.x) + (n.y * base.y))) / vd;
    const Flt pt_z = base.z + (dir.z * h);
    if (pt_z < -1.0 || pt_z > 1.0) { continue; } // miss

    const Flt pt_x = base.x + (dir.x * h);
    const Flt pt_y = base.y + (dir.y * h);
    const Flt lenSqr = Sqr(pt_x - n.x) + Sqr(pt_y - n.y);
    if (lenSqr < _halfSideLenSqr) {
      if (h < near_h) { near_h = h; near_s = i; }
      if (h > far_h) { far_h = h; far_s = i; }
      if (near_s != far_s) { break; } // 2 sides hit
    }
  }

  if (dir.z != 0) {
    if (near_s == far_s) { // 0 or 1 hit
      // end 1
      const Flt h = -(base.z - 1.0) / dir.z;
      const Flt pt_x = base.x + (dir.x * h);
      const Flt pt_y = base.y + (dir.y * h);
      bool inside = true;
      for (int i = 0; i < _sides; ++i) {
        const Vec2& n = _plane[std::size_t(i)];
        const Flt v = (n.x * pt_x) + (n.y * pt_y) - 1.0;
        if (v > 0) { inside = false; break; }
      }

      if (inside) {
        if (h < near_h) { near_h = h; near_s = _sides; }
        if (h > far_h) { far_h = h; far_s = _sides; }
      }
    }

    if (near_s == far_s) { // 0 or 1 hit
      // end 2
      const Flt h = -(base.z + 1.0) / dir.z;
      const Flt pt_x = base.x + (dir.x * h);
      const Flt pt_y = base.y + (dir.y * h);
      bool inside = true;
      for (int i = 0; i < _sides; ++i) {
        const Vec2& n = _plane[std::size_t(i)];
        const Flt v = (n.x * pt_x) + (n.y * pt_y) - 1.0;
        if (v > 0) { inside = false; break; }
      }

      if (inside) {
        if (h < near_h) { near_h = h; near_s = _sides+1; }
        if (h > far_h) { far_h = h; far_s = _sides+1; }
      }
    }
  }

  if (near_s == far_s || far_h < r.min_length) { return 0; }

  ++r.stats->prism.hit;
  if (hit_list.csg()) {
    hit_list.addHit(
      this, near_h, CalcHitPoint(base, dir, near_h), near_s, HIT_ENTER);
    hit_list.addHit(
      this, far_h, CalcHitPoint(base, dir, far_h), far_s, HIT_EXIT);
    return 2;
  }

  if (near_h < r.min_length) { near_h = far_h; near_s = far_s; }
  hit_list.addHit(
    this, near_h, CalcHitPoint(base, dir, near_h), near_s, HIT_NORMAL);
  return 1;
}

Vec3 Prism::normal(const Ray& r, const HitInfo& h) const
{
  return _normal[std::size_t(h.side)];
}
