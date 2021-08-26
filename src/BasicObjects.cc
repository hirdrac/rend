//
// BasicObjects.cc
// Copyright (C) 2021 Richard Bradley
//

#include "BasicObjects.hh"
#include "Intersect.hh"
#include "Ray.hh"
#include "Roots.hh"
#include "Stats.hh"
#include "BBox.hh"
#include "HitCostInfo.hh"
#include <algorithm>
#include <cmath>


// shared data
static constexpr Vec3 planeBoundPoints[4] = {
  { 1, 1, 0}, {-1, 1, 0}, { 1,-1, 0}, {-1,-1, 0}};


// **** Disc Class ****
int Disc::init(Scene& s)
{
  if (Primitive::init(s)) { return -1; }

  _normal = _trans.normalLocalToGlobal({0,0,1}, 0);  // cache plane normal
  return 0;
}

BBox Disc::bound(const Matrix* t) const
{
  return BBox(planeBoundPoints, std::size(planeBoundPoints),
              t ? *t : _trans.final());
}

Flt Disc::hitCost() const
{
  return (_cost >= 0.0) ? _cost : CostTable.disc;
}

int Disc::intersect(const Ray& r, HitList& hit_list) const
{
  ++r.stats->disc.tried;

  const Vec3 dir = _trans.rayLocalDir(r);
  if (UNLIKELY(dir.z == 0.0)) {
    return 0;  // parallel with disc plane
  }

  const Vec3 base = _trans.rayLocalBase(r);
  const Flt h = -base.z / dir.z;
  if (h < r.min_length) {
    return 0;  // disc behind ray origin
  }

  const Vec3 pt{base.x + (dir.x * h), base.y + (dir.y * h), 0.0};
  if ((Sqr(pt.x) + Sqr(pt.y)) > 1.0) {
    return 0;
  }

  hit_list.addHit(this, h, pt, 0, HIT_NORMAL);
  ++r.stats->disc.hit;
  return 1;
}

Vec3 Disc::normal(const Ray& r, const HitInfo& h) const
{
  return _normal;
}


// **** Cone Class ****
int Cone::init(Scene& s)
{
  if (Primitive::init(s)) { return -1; }

  _baseNormal = _trans.normalLocalToGlobal({0,0,-1}, 0);
  return 0;
}

int Cone::intersect(const Ray& r, HitList& hit_list) const
{
  ++r.stats->cone.tried;

  const Vec3 dir = _trans.rayLocalDir(r);
  const Vec3 base = _trans.rayLocalBase(r);

  if (UNLIKELY(dir.z == 0.0) && (base.z < -1.0 || base.z > 1.0)) {
    // ray parallel with cone bottom & doesn't hit side
    return 0;
  }

  const Flt a = Sqr(dir.x) + Sqr(dir.y) - (0.25 * Sqr(dir.z));
  const Flt b = (base.x * dir.x) + (base.y * dir.y) +
    (dir.z * (1.0 - base.z) * 0.25);
  const Flt c = Sqr(base.x) + Sqr(base.y) - (0.25 * Sqr(1.0 - base.z));
  const Flt x = Sqr(b) - (a * c);
  if (x < VERY_SMALL) {
    return 0;
  }

  const Flt sqrt_x = std::sqrt(x);

  Flt h[3];
  int side[3];
  int hits = 0;

  const Flt h1 = (-b - sqrt_x) / a;
  const Flt h1z = base.z + (dir.z * h1);
  if (h1z >= -1.0 && h1z <= 1.0) {
    h[hits] = h1;
    side[hits] = 0;
    ++hits;
  }

  const Flt h2 = (-b + sqrt_x) / a;
  const Flt h2z = base.z + (dir.z * h2);
  if (h2z >= -1.0 && h2z <= 1.0) {
    h[hits] = h2;
    side[hits] = 0;
    ++hits;
  }

  if (LIKELY(dir.z != 0.0)) {
    const Flt h0 = -(base.z + 1.0) / dir.z;
    const Flt h0x = base.x + (dir.x * h0);
    const Flt h0y = base.y + (dir.y * h0);
    if ((Sqr(h0x) + Sqr(h0y)) <= 1.0) {
      h[hits] = h0;
      side[hits] = 1;
      ++hits;
    }
  }

  if (hits != 2) { return 0; }

  Flt near_h, far_h;
  int near_side, far_side;
  if (h[0] < h[1]) {
    near_h = h[0]; near_side = side[0];
    far_h = h[1]; far_side = side[1];
  } else {
    near_h = h[1]; near_side = side[1];
    far_h = h[0]; far_side = side[0];
  }

  if (far_h < r.min_length) {
    return 0;
  }

  ++r.stats->cone.hit;
  if (hit_list.csg()) {
    hit_list.addHit(
      this, near_h, CalcHitPoint(base, dir, near_h), near_side, HIT_ENTER);
    hit_list.addHit(
      this, far_h, CalcHitPoint(base, dir, far_h), far_side, HIT_EXIT);
    return 2;
  }

  if (near_h < r.min_length) { near_h = far_h; near_side = far_side; }
  hit_list.addHit(
    this, near_h, CalcHitPoint(base, dir, near_h), near_side, HIT_NORMAL);
  return 1;
}

Vec3 Cone::normal(const Ray& r, const HitInfo& h) const
{
  if (h.side == 1) {
    // base
    return _baseNormal;
  } else {
    // side
    const Vec3 n{h.local_pt.x, h.local_pt.y, (1.0 - h.local_pt.z) / 4.0};
    return _trans.normalLocalToGlobal(n, r.time);
  }
}

Flt Cone::hitCost() const
{
  return (_cost >= 0.0) ? _cost : CostTable.cone;
}


// **** Cube Class ****
int Cube::init(Scene& s)
{
  if (Primitive::init(s)) { return -1; }

  static constexpr Vec3 n[6] = {
    {1, 0, 0}, {-1, 0, 0}, {0, 1, 0}, {0, -1, 0}, {0, 0, 1}, {0, 0, -1}};

  for (int i = 0; i < 6; ++i) {
    _sideNormal[i] = _trans.normalLocalToGlobal(n[i], 0);
  }

  return 0;
}

Flt Cube::hitCost() const
{
  return (_cost >= 0.0) ? _cost : CostTable.cube;
}

int Cube::intersect(const Ray& r, HitList& hit_list) const
{
  ++r.stats->cube.tried;

  const Vec3 dir = _trans.rayLocalDir(r);
  const Vec3 base = _trans.rayLocalBase(r);

  Flt near_h = -VERY_LARGE, far_h = VERY_LARGE;
  int near_side = -1, far_side = -1;

  // X
  if (LIKELY(dir.x != 0.0)) {
    Flt h1 = (-1.0 - base.x) / dir.x;
    Flt h2 = ( 1.0 - base.x) / dir.x;
    if (h1 > h2) { std::swap(h1, h2); }
    near_h = h1; near_side = 1;
    far_h = h2; far_side = 0;

  } else if ((base.x < -1.0) || (base.x > 1.0)) {
    return 0;  // Miss
  }

  // Y & Z
  for (unsigned int i = 1; i < 3; ++i) {
    if (LIKELY(dir[i] != 0.0)) {
      Flt h1 = (-1.0 - base[i]) / dir[i];
      Flt h2 = ( 1.0 - base[i]) / dir[i];
      if (h1 > h2) { std::swap(h1, h2); }
      if (h1 > near_h) { near_h = h1; near_side = (int(i)*2)+1; }
      if (h2 < far_h)  { far_h  = h2; far_side  = (int(i)*2); }
      if (near_h > far_h) {
        return 0;  // Miss
      }
    } else if ((base[i] < -1.0) || (base[i] > 1.0)) {
      return 0;  // Miss
    }
  }

  if (far_h < r.min_length) {
    return 0;
  }

  ++r.stats->cube.hit;
  if (hit_list.csg()) {
    hit_list.addHit(
      this, near_h, CalcHitPoint(base, dir, near_h), near_side, HIT_ENTER);
    hit_list.addHit(
      this, far_h, CalcHitPoint(base, dir, far_h), far_side, HIT_EXIT);
    return 2;
  }

  if (near_h < r.min_length) { near_h = far_h; near_side = far_side; }
  hit_list.addHit(
    this, near_h, CalcHitPoint(base, dir, near_h), near_side, HIT_NORMAL);
  return 1;
}

Vec3 Cube::normal(const Ray& r, const HitInfo& h) const
{
  //static constexpr Vec3 n[6] = {
  //  {1, 0, 0}, {-1, 0, 0}, {0, 1, 0}, {0, -1, 0}, {0, 0, 1}, {0, 0, -1}};
  //return _trans.normalLocalToGlobal(n[h.side], r.time);

  return _sideNormal[h.side];
}


// **** Cylinder Class ****
int Cylinder::init(Scene& s)
{
  if (Primitive::init(s)) { return -1; }

  _endNormal[0] = _trans.normalLocalToGlobal({0, 0,  1}, 0);
  _endNormal[1] = _trans.normalLocalToGlobal({0, 0, -1}, 0);
  return 0;
}

Flt Cylinder::hitCost() const
{
  return (_cost >= 0.0) ? _cost : CostTable.cylinder;
}

int Cylinder::intersect(const Ray& r, HitList& hit_list) const
{
  ++r.stats->cylinder.tried;

  const Vec3 dir = _trans.rayLocalDir(r);
  const Vec3 base = _trans.rayLocalBase(r);

  // Intersect cylinder side
  const Flt a = Sqr(dir.x) + Sqr(dir.y);
  const Flt b = (dir.x * base.x) + (dir.y * base.y);
  const Flt c = Sqr(base.x) + Sqr(base.y) - 1.0;
  const Flt x = Sqr(b) - (a * c);

  Flt near_h = -VERY_LARGE, far_h = VERY_LARGE;
  if (IsPositive(x)) {
    const Flt sqrt_x = std::sqrt(x);
    near_h = (-b - sqrt_x) / a;  // cylinder hit 1
    far_h  = (-b + sqrt_x) / a;  // cylinder hit 2
  } else if (IsNegative(x) || c > 0.0) {
    // cylinder missed completely or
    // ray parallel with cylinder side but outside cylinder
    return 0;
  }

  // Intersect cylinder ends
  int near_side = 0, far_side = 0;
  if (LIKELY(dir.z != 0.0)) {
    Flt h1 = -(base.z - 1.0) / dir.z;  // plane hit 1
    Flt h2 = -(base.z + 1.0) / dir.z;  // plane hit 2
    if (h1 > h2) { std::swap(h1, h2); }
    if (h1 > near_h) { near_h = h1; near_side = 1; }
    if (h2 < far_h)  { far_h  = h2; far_side  = 2; }

    if (near_h > far_h) {
      return 0;  // cylinder missed
    }
  } else if ((base.z < -1.0) || (base.z > 1.0)) {
    return 0; // ray parallel with planes but not between planes
  }

  if (far_h < r.min_length) {
    return 0;  // cylinder completely behind ray origin
  }

  ++r.stats->cylinder.hit;
  if (hit_list.csg()) {
    hit_list.addHit(
      this, near_h, CalcHitPoint(base, dir, near_h), near_side, HIT_ENTER);
    hit_list.addHit(
      this, far_h, CalcHitPoint(base, dir, far_h), far_side, HIT_EXIT);
    return 2;
  }

  if (near_h < r.min_length) { near_h = far_h; near_side = far_side; }
  hit_list.addHit(
    this, near_h, CalcHitPoint(base, dir, near_h), near_side, HIT_NORMAL);
  return 1;
}

Vec3 Cylinder::normal(const Ray& r, const HitInfo& h) const
{
  switch (h.side) {
    default:
    case 0: {  // side
      const Vec3 n{h.local_pt.x, h.local_pt.y, 0.0};
      return _trans.normalLocalToGlobal(n, r.time);
    }

    case 1:  // end 1
      return _endNormal[0];

    case 2:  // end 2
      return _endNormal[1];
  }
}


// **** OpenCone Class ****
int OpenCone::intersect(const Ray& r, HitList& hit_list) const
{
  ++r.stats->open_cone.tried;

  const Vec3 dir = _trans.rayLocalDir(r);
  const Vec3 base = _trans.rayLocalBase(r);

  const Flt a = Sqr(dir.x) + Sqr(dir.y) - (0.25 * Sqr(dir.z));
  const Flt b = (base.x * dir.x) + (base.y * dir.y) +
    (dir.z * (1.0 - base.z) * 0.25);
  const Flt c = Sqr(base.x) + Sqr(base.y) - (0.25 * Sqr(1.0 - base.z));
  const Flt x = Sqr(b) - (a * c);
  if (x < VERY_SMALL) {
    return 0;  // cone missed
  }

  const Flt sqrt_x = std::sqrt(x);
  const Flt h1 = (-b - sqrt_x) / a;
  const Flt h2 = (-b + sqrt_x) / a;

  const Flt near_h = std::min(h1,h2);
  if (near_h >= r.min_length) {
    const Flt pz = base.z + (dir.z * near_h);
    if (pz >= -1.0 && pz <= 1.0) {
      const Flt px = base.x + (dir.x * near_h);
      const Flt py = base.y + (dir.y * near_h);
      hit_list.addHit(this, near_h, {px,py,pz}, 0, HIT_NORMAL);
      ++r.stats->open_cone.hit;
      return 1;
    }
  }

  const Flt far_h = std::max(h1,h2);
  if (far_h >= r.min_length) {
    const Flt pz = base.z + (dir.z * far_h);
    if (pz >= -1.0 && pz <= 1.0) {
      const Flt px = base.x + (dir.x * far_h);
      const Flt py = base.y + (dir.y * far_h);
      hit_list.addHit(this, far_h, {px,py,pz}, 0, HIT_NORMAL);
      ++r.stats->open_cone.hit;
      return 1;
    }
  }

  return 0;
}

Vec3 OpenCone::normal(const Ray& r, const HitInfo& h) const
{
  const Vec3 n{h.local_pt.x, h.local_pt.y, (1.0 - h.local_pt.z) / 4.0};
  return _trans.normalLocalToGlobal(n, r.time);
}

Flt OpenCone::hitCost() const
{
  return (_cost >= 0.0) ? _cost : CostTable.open_cone;
}


// **** OpenCylinder Class ****
int OpenCylinder::intersect(const Ray& r, HitList& hit_list) const
{
  ++r.stats->open_cylinder.tried;

  const Vec3 dir = _trans.rayLocalDir(r);
  const Vec3 base = _trans.rayLocalBase(r);

  const Flt a = Sqr(dir.x) + Sqr(dir.y);
  const Flt b = (dir.x * base.x) + (dir.y * base.y);
  const Flt c = Sqr(base.x) + Sqr(base.y) - 1.0;
  const Flt x = Sqr(b) - (a * c);
  if (x < VERY_SMALL) {
    return 0;  // missed (avoid single intersection case)
  }

  const Flt sqrt_x = std::sqrt(x);

  const Flt near_h = (-b - sqrt_x) / a;
  if (near_h >= r.min_length) {
    const Flt pz = base.z + (dir.z * near_h);
    if ((pz >= -1.0) && (pz <= 1.0)) {
      const Flt px = base.x + (dir.x * near_h);
      const Flt py = base.y + (dir.y * near_h);
      hit_list.addHit(this, near_h, {px,py,pz}, 0, HIT_NORMAL);
      ++r.stats->open_cylinder.hit;
      return 1;
    }
  }

  const Flt far_h = (-b + sqrt_x) / a;
  if (far_h >= r.min_length) {
    const Flt pz = base.z + (dir.z * far_h);
    if ((pz >= -1.0) && (pz <= 1.0)) {
      const Flt px = base.x + (dir.x * far_h);
      const Flt py = base.y + (dir.y * far_h);
      hit_list.addHit(this, far_h, {px,py,pz}, 0, HIT_NORMAL);
      ++r.stats->open_cylinder.hit;
      return 1;
    }
  }

  return 0;
}

Vec3 OpenCylinder::normal(const Ray& r, const HitInfo& h) const
{
  const Vec3 n{h.local_pt.x, h.local_pt.y, 0.0};
  return _trans.normalLocalToGlobal(n, r.time);
}

Flt OpenCylinder::hitCost() const
{
  return (_cost >= 0.0) ? _cost : CostTable.open_cylinder;
}


// **** Paraboloid Class ****
int Paraboloid::intersect(const Ray& r, HitList& hit_list) const
{
  ++r.stats->paraboloid.tried;

  const Vec3 dir = _trans.rayLocalDir(r);
  const Vec3 base = _trans.rayLocalBase(r);

  const Flt a = Sqr(dir.x) + Sqr(dir.y);
  const Flt b = (dir.x * base.x) + (dir.y * base.y) + dir.z * .25;
  const Flt c = Sqr(base.x) + Sqr(base.y) + (base.z - .5);
  const Flt x = Sqr(b) - (a * c);
  if (x < VERY_SMALL) {
    return 0;  // paraboloid missed (avoid single intersection case)
  }

  const Flt sqrt_x = std::sqrt(x);

  const Flt near_h = (-b - sqrt_x) / a;
  if (near_h >= r.min_length) {
    const Flt pz = base.z + (dir.z * near_h);
    if (pz >= -1.0) {
      const Flt px = base.x + (dir.x * near_h);
      const Flt py = base.y + (dir.y * near_h);
      hit_list.addHit(this, near_h, {px,py,pz}, 0, HIT_NORMAL);
      ++r.stats->paraboloid.hit;
      return 1;
    }
  }

  const Flt far_h = (-b + sqrt_x) / a;
  if (far_h >= r.min_length) {
    const Flt pz = base.z + (dir.z * far_h);
    if (pz >= -1.0) {
      const Flt px = base.x + (dir.x * far_h);
      const Flt py = base.y + (dir.y * far_h);
      hit_list.addHit(this, far_h, {px,py,pz}, 0, HIT_NORMAL);
      ++r.stats->paraboloid.hit;
      return 1;
    }
  }

  return 0;
}

Vec3 Paraboloid::normal(const Ray& r, const HitInfo& h) const
{
  const Vec3 n{h.local_pt.x, h.local_pt.y, .125};
  return _trans.normalLocalToGlobal(n, r.time);
}

Flt Paraboloid::hitCost() const
{
  return (_cost >= 0.0) ? _cost : CostTable.paraboloid;
}


// **** Plane Class ****
int Plane::init(Scene& s)
{
  if (Primitive::init(s)) { return -1; }

  _normal = _trans.normalLocalToGlobal({0,0,1}, 0); // cache plane normal
  return 0;
}

BBox Plane::bound(const Matrix* t) const
{
  return BBox(planeBoundPoints, std::size(planeBoundPoints),
              t ? *t : _trans.final());
}

Flt Plane::hitCost() const
{
  return (_cost >= 0.0) ? _cost : CostTable.plane;
}

int Plane::intersect(const Ray& r, HitList& hit_list) const
{
  ++r.stats->plane.tried;

  const Vec3 dir = _trans.rayLocalDir(r);
  if (UNLIKELY(dir.z == 0.0)) {
    return 0;  // parallel with plane
  }

  const Vec3 base = _trans.rayLocalBase(r);
  const Flt h = -base.z / dir.z;
  if (h < r.min_length) {
    return 0;  // plane behind ray origin
  }

  const Flt px = base.x + (dir.x * h);
  if ((px < -1.0) || (px > 1.0)) {
    return 0;
  }

  const Flt py = base.y + (dir.y * h);
  if ((py < -1.0) || (py > 1.0)) {
    return 0;
  }

  hit_list.addHit(this, h, {px,py,0.0}, 0, HIT_NORMAL);
  ++r.stats->plane.hit;
  return 1;
}

Vec3 Plane::normal(const Ray& r, const HitInfo& h) const
{
  return _normal;
}


// **** Sphere Class ****
Flt Sphere::hitCost() const
{
  return (_cost >= 0.0) ? _cost : CostTable.sphere;
}

int Sphere::intersect(const Ray& r, HitList& hit_list) const
{
  ++r.stats->sphere.tried;

  const Vec3 dir = _trans.rayLocalDir(r);
  const Vec3 base = _trans.rayLocalBase(r);

  const Flt a = DotProduct(dir,  dir);
  const Flt b = DotProduct(base, dir);
  const Flt c = DotProduct(base, base) - 1.0;
  const Flt x = Sqr(b) - (a * c);
  if (x < VERY_SMALL) {
    return 0;  // missed (avoid single intersection case)
  }

  // Find hit points on sphere
  const Flt sqrt_x = std::sqrt(x);
  const Flt far_h = (-b + sqrt_x) / a;
  if (far_h < r.min_length) {
    return 0;  // sphere completely behind ray origin
  }

  Flt near_h = (-b - sqrt_x) / a;

  ++r.stats->sphere.hit;
  if (hit_list.csg()) {
    hit_list.addHit(this, near_h, CalcHitPoint(base, dir, near_h), 0, HIT_ENTER);
    hit_list.addHit(this, far_h, CalcHitPoint(base, dir, far_h), 0, HIT_EXIT);
    return 2;
  }

  if (near_h < r.min_length) { near_h = far_h; }
  hit_list.addHit(this, near_h, CalcHitPoint(base, dir, near_h), 0, HIT_NORMAL);
  return 1;
}

Vec3 Sphere::normal(const Ray& r, const HitInfo& h) const
{
  return _trans.normalLocalToGlobal(h.local_pt, r.time);
}


// **** Torus Class ****
BBox Torus::bound(const Matrix* t) const
{
  const Flt r1 = _radius + 1.0;
  const Vec3 pt[8] = {
    { r1,  _radius,  r1}, {-r1,  _radius,  r1},
    { r1, -_radius,  r1}, { r1,  _radius, -r1},
    {-r1, -_radius,  r1}, { r1, -_radius, -r1},
    {-r1,  _radius, -r1}, {-r1, -_radius, -r1}};
  return BBox(pt, std::size(pt), t ? *t : _trans.final());
}

Flt Torus::hitCost() const
{
  return (_cost >= 0.0) ? _cost : CostTable.torus;
}

int Torus::intersect(const Ray& r, HitList& hit_list) const
{
  ++r.stats->torus.tried;

  const Vec3 dir = _trans.rayLocalDir(r);
  const Vec3 base = _trans.rayLocalBase(r);

  const Flt bd = DotProduct(base, dir);
  const Flt dd = DotProduct(dir,  dir);
  const Flt t1 = DotProduct(base, base) + (1.0 - Sqr(_radius));

  Flt c[5];
  c[0] = Sqr(t1) - 4 * (Sqr(base.x) + Sqr(base.z));
  c[1] = 4 * bd * t1 - 8 * (base.x * dir.x + base.z * dir.z);
  c[2] = 4 * Sqr(bd) + 2 * t1 * dd - 4 * (Sqr(dir.x) + Sqr(dir.z));
  c[3] = 4 * bd * dd;
  c[4] = Sqr(dd);

  Flt root[4];
  const int n = SolveQuartic(c, root);
  if ((n != 2) && (n != 4)) {
    return 0;
  }

  // NOTE: output order of SolveQuartic() ok for addHit() logic below
  // if 2 hits:
  //   root[0] < root[1]
  // if 4 hits:
  //   root[0] < root[1] && root[2] < root[3]
  //   (root[1] < root[2] not always true, however)

  Flt far_h = (n==2) ? root[1] : std::max(root[1], root[3]);
  if (far_h < r.min_length) {
    return 0;
  }

  ++r.stats->torus.hit;
  if (hit_list.csg()) {
    for (int i = 0; i < n; ++i) {
      hit_list.addHit(this, root[i], CalcHitPoint(base, dir, root[i]), 0,
                      (i&1) ? HIT_EXIT : HIT_ENTER);
    }
    return n;
  }

  Flt h = VERY_LARGE;
  for (int i = 0; i < n; ++i) {
    if (root[i] >= r.min_length && root[i] < h) { h = root[i]; }
  }

  hit_list.addHit(this, h, CalcHitPoint(base, dir, h), 0, HIT_NORMAL);
  return 1;
}

Vec3 Torus::normal(const Ray& r, const HitInfo& h) const
{
  const Flt x = h.local_pt.x;
  const Flt z = h.local_pt.z;
  const Flt sqrt_d = std::sqrt(Sqr(x) + Sqr(z));
  const Vec3 n{x - (x / sqrt_d), h.local_pt.y, z - (z / sqrt_d)};

  // alternate normal calc
  //const Flt a = DotProduct(h.local_pt, h.local_pt) - 1.0 - Sqr(_radius);
  //const Vec3 n{
  //  4.0 * h.local_pt.x * a,
  //  4.0 * h.local_pt.y * (a + 2.0),
  //  4.0 * h.local_pt.z * a};

  return _trans.normalLocalToGlobal(n, r.time);
}
