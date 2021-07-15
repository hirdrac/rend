//
// BasicObjects.cc
// Copyright (C) 2021 Richard Bradley
//
// Implementation of primitive module
//

#include "BasicObjects.hh"
#include "Intersect.hh"
#include "Ray.hh"
#include "Roots.hh"
#include "Stats.hh"
#include <algorithm>
#include <cmath>


// **** Disc Class ****
// SceneItem Functions
int Disc::init(Scene& s)
{
  if (Primitive::init(s)) { return -1; }

  normal_cache = _trans.normalLocalToGlobal({0,0,1}, 0);  // cache plane normal
  return 0;
}

// Object Functions
int Disc::intersect(const Ray& r, bool csg, HitList& hit_list) const
{
  ++r.stats->disc_tried;

  Vec3 base, dir;
  r.globalToLocal(_trans, base, dir);

  if (IsZero(dir.z)) {
    return 0;  // parallel with disc plane
  }

  Flt h = -base.z / dir.z;
  if (h < 0) {
    return 0;  // disc behind ray origin
  }

  Vec3 pt{base.x + (dir.x * h), base.y + (dir.y * h), 0.0};
  if ((Sqr(pt.x) + Sqr(pt.y)) > 1.0) {
    return 0;
  }

  hit_list.addHit(this, h, pt, 0, HIT_NORMAL);
  ++r.stats->disc_hit;
  return 1;
}

int Disc::evalHit(const HitInfo& h, Vec3& normal, Vec3& map) const
{
  normal = normal_cache;
  map = h.local_pt;
  return 0;
}

int Disc::bound(BBox& b) const
{
  static constexpr Vec3 pt[4] = {
    { 1, 1, 0}, {-1, 1, 0}, { 1,-1, 0}, {-1,-1, 0}};
  b.fit(_trans, pt, 4);
  return 0;
}


// **** Cone Class ****
// SceneItem Functions
int Cone::init(Scene& s)
{
  if (Primitive::init(s)) { return -1; }

  normal_cache = _trans.normalLocalToGlobal({0,0,-1}, 0);
  return 0;
}

// Object Functions
int Cone::intersect(const Ray& r, bool csg, HitList& hit_list) const
{
  ++r.stats->cone_tried;

  Vec3 base, dir;
  r.globalToLocal(_trans, base, dir);

  if (IsZero(dir.z) && (base.z < -1.0 || base.z > 1.0)) {
    // ray parallel with cone bottom & doesn't hit side
    return 0;
  }

  const Flt a = Sqr(dir.x) + Sqr(dir.y) - (0.25 * Sqr(dir.z));
  const Flt b = (base.x * dir.x) + (base.y * dir.y) +
    (dir.z * (1.0 - base.z) * 0.25);
  const Flt c = Sqr(base.x) + Sqr(base.y) - (0.25 * Sqr(1.0 - base.z));
  const Flt x = Sqr(b) - (a * c);
  if (x < VERY_SMALL || IsZero(a)) {
    return 0;
  }

  const Flt sqrt_x = std::sqrt(x);
  const Flt h1 = (-b - sqrt_x) / a;
  const Flt h2 = (-b + sqrt_x) / a;

  Flt h[3] = {};
  int side[3] = {};
  int hits = 0;

  const Flt h1z = base.z + (dir.z * h1);
  if (h1z >= -1.0 && h1z <= 1.0) {
    h[hits] = h1;
    side[hits] = 0;
    ++hits;
  }

  const Flt h2z = base.z + (dir.z * h2);
  if (h2z >= -1.0 && h2z <= 1.0) {
    h[hits] = h2;
    side[hits] = 0;
    ++hits;
  }

  if (!IsZero(dir.z)) {
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

  hit_list.addHit(this, near_h, CalcHitPoint(base, dir, near_h), near_side,
                  csg ? HIT_ENTER : HIT_NORMAL);
  if (csg) {
    hit_list.addHit(this, far_h, CalcHitPoint(base, dir, far_h), far_side,
                    HIT_EXIT);
  }

  ++r.stats->cone_hit;
  return 1 + int(csg);
}

int Cone::evalHit(const HitInfo& h, Vec3& normal, Vec3& map) const
{
  if (h.side == 1) {
    // base
    normal = normal_cache;
    map.set(-(h.local_pt.x), h.local_pt.y, -1.0);

  } else {
    // side
    Vec3 n{h.local_pt.x, h.local_pt.y, (1.0 - h.local_pt.z) / 4.0};
    normal = _trans.normalLocalToGlobal(n, 0);

    Vec2 dir = UnitVec(Vec2{h.local_pt.x, h.local_pt.y});
    Flt x = std::clamp(dir.x, -1.0 + VERY_SMALL, 1.0 - VERY_SMALL);
    Flt u = (std::acos(x) * (2.0/PI)) - 1.0;
    map.set((h.local_pt.y >= 0.0) ? u : -u, h.local_pt.z, 0.0);
  }

  return 0;
}


// **** Cube Class ****
// SceneItem Functions
int Cube::init(Scene& s)
{
  if (Primitive::init(s)) { return -1; }

  static constexpr Vec3 n[6] = {
    {1, 0, 0}, {-1, 0, 0}, {0, 1, 0}, {0, -1, 0}, {0, 0, 1}, {0, 0, -1}};

  for (int i = 0; i < 6; ++i) {
    normal_cache[i] = _trans.normalLocalToGlobal(n[i], 0);
  }

  return 0;
}

// Object Functions
int Cube::intersect(const Ray& r, bool csg, HitList& hit_list) const
{
  ++r.stats->cube_tried;

  Vec3 base, dir;
  r.globalToLocal(_trans, base, dir);

  Flt near_h = -VERY_LARGE, far_h = VERY_LARGE;
  int near_side = -1, far_side = -1;

  // X
  if (!IsZero(dir.x)) {
    Flt h1 = (-1.0 - base.x) / dir.x;
    Flt h2 = (1.0 - base.x) / dir.x;
    if (h1 > h2) {
      near_h = h2;
      near_side = 0;
      far_h = h1;
      far_side = 1;
    } else {
      near_h = h1;
      near_side = 1;
      far_h = h2;
      far_side = 0;
    }
  } else if ((base.x < -1.0) || (base.x > 1.0)) {
    return 0;  // Miss
  }

  // Y
  if (!IsZero(dir.y)) {
    Flt h1 = (-1.0 - base.y) / dir.y;
    Flt h2 = (1.0 - base.y) / dir.y;
    if (h1 > h2) {
      if (h2 > near_h) { near_h = h2; near_side = 2; }
      if (h1 < far_h)  { far_h  = h1; far_side  = 3; }
    } else {
      if (h1 > near_h) { near_h = h1; near_side = 3; }
      if (h2 < far_h)  { far_h  = h2; far_side  = 2; }
    }

    if (near_h > far_h) {
      return 0;  // Miss
    }
  } else if ((base.y < -1.0) || (base.y > 1.0)) {
    return 0;  // Miss
  }

  // Z
  if (!IsZero(dir.z)) {
    Flt h1 = (-1.0 - base.z) / dir.z;
    Flt h2 = ( 1.0 - base.z) / dir.z;
    if (h1 > h2) {
      if (h2 > near_h) { near_h = h2; near_side = 4; }
      if (h1 < far_h)  { far_h  = h1; far_side  = 5; }
    } else {
      if (h1 > near_h) { near_h = h1; near_side = 5; }
      if (h2 < far_h)  { far_h  = h2; far_side  = 4; }
    }

    if (near_h > far_h) {
      return 0;  // Miss
    }
  } else if ((base.z < -1.0) || (base.z > 1.0)) {
    return 0;  // Miss
  }

  if (far_h < 0) {
    return 0;  // cube completely behind ray origin
  }

  hit_list.addHit(this, near_h, CalcHitPoint(base, dir, near_h), near_side,
                  csg ? HIT_ENTER : HIT_NORMAL);
  if (csg) {
    hit_list.addHit(this, far_h, CalcHitPoint(base, dir, far_h), far_side,
                    HIT_EXIT);
  }

  ++r.stats->cube_hit;
  return 1 + int(csg);
}

int Cube::evalHit(const HitInfo& h, Vec3& normal, Vec3& map) const
{
  switch (h.side) {
    case 0: map.set(-h.local_pt.z,  h.local_pt.y, 0.0); break;
    case 1: map.set( h.local_pt.z,  h.local_pt.y, 0.0); break;
    case 2: map.set( h.local_pt.x, -h.local_pt.z, 0.0); break;
    case 3: map.set( h.local_pt.x,  h.local_pt.z, 0.0); break;
    case 4: map.set( h.local_pt.x,  h.local_pt.y, 0.0); break;
    case 5: map.set(-h.local_pt.x,  h.local_pt.y, 0.0); break;
    default: return -1;
  }

  normal = normal_cache[h.side];
  return 0;
}


// **** Cylinder Class ****
// SceneItem Functions
int Cylinder::init(Scene& s)
{
  if (Primitive::init(s)) { return -1; }

  static constexpr Vec3 n0{0, 0, 1}, n1{0, 0, -1};
  normal_cache[0] = _trans.normalLocalToGlobal(n0, 0);
  normal_cache[1] = _trans.normalLocalToGlobal(n1, 0);
  return 0;
}

// Object Functions
int Cylinder::intersect(const Ray& r, bool csg, HitList& hit_list) const
{
  ++r.stats->cylinder_tried;

  Vec3 base, dir;
  r.globalToLocal(_trans, base, dir);

  // Intersect cylinder side
  Flt a = Sqr(dir.x) + Sqr(dir.y);
  Flt b = (dir.x * base.x) + (dir.y * base.y);
  Flt c = Sqr(base.x) + Sqr(base.y) - 1.0;
  Flt x = Sqr(b) - (a * c);

  Flt near_h = -VERY_LARGE, far_h = VERY_LARGE;
  if (IsPositive(x)) {
    Flt sqrt_x = std::sqrt(x);
    near_h = (-b - sqrt_x) / a;  // cylinder hit 1
    far_h  = (-b + sqrt_x) / a;  // cylinder hit 2
  } else if (IsNegative(x) || c > 0.0) {
    // cylinder missed completely or
    // ray parallel with cylinder side but outside cylinder
    return 0;
  }

  // Intersect cylinder ends
  int near_side = 0, far_side = 0;
  if (IsZero(dir.y)) {
    // ray parallel with planes
    if ((base.y < -1.0) || (base.y > 1.0)) {
      return 0;  // ray not between planes
    }

  } else {
    Flt h1 = -(base.z - 1.0) / dir.z;  // plane hit 1
    Flt h2 = -(base.z + 1.0) / dir.z;  // plane hit 2

    if (h1 > h2) {
      if (h2 > near_h) { near_h = h2; near_side = 2; }
      if (h1 < far_h)  { far_h  = h1; far_side  = 1; }

    } else {
      if (h1 > near_h) { near_h = h1; near_side = 1; }
      if (h2 < far_h)  { far_h  = h2; far_side  = 2; }
    }

    if (near_h > far_h) {
      return 0;  // cylinder missed
    }
  }

  if (far_h < 0) {
    return 0;  // cylinder completely behind ray origin
  }

  hit_list.addHit(this, near_h, CalcHitPoint(base, dir, near_h), near_side,
                  csg ? HIT_ENTER : HIT_NORMAL);
  if (csg) {
    hit_list.addHit(this, far_h, CalcHitPoint(base, dir, far_h), far_side,
                    HIT_EXIT);
  }

  ++r.stats->cylinder_hit;
  return 1 + int(csg);
}

int Cylinder::evalHit(const HitInfo& h, Vec3& normal, Vec3& map) const
{
  switch (h.side) {
    case 0: {  // side
      Vec3 n{h.local_pt.x, h.local_pt.y, 0.0};
      normal = _trans.normalLocalToGlobal(n, 0);

      Flt x = std::clamp(h.local_pt.x, -1.0 + VERY_SMALL, 1.0 - VERY_SMALL);
      Flt u = (std::acos(x) * (2.0/PI)) - 1.0;
      map.set((h.local_pt.y >= 0.0) ? u : -u, h.local_pt.z, 0.0);
      break;
    }

    case 1:  // end 1
      normal = normal_cache[0];
      map.set(h.local_pt.x, h.local_pt.y, 0.0);
      break;

    case 2:  // end 2
      normal = normal_cache[1];
      map.set(h.local_pt.x, h.local_pt.y, 0.0);
      break;

    default:
      return -1;
  }

  return 0;
}


// **** OpenCone Class ****
// Object Functions
int OpenCone::intersect(const Ray& r, bool csg, HitList& hit_list) const
{
  ++r.stats->open_cone_tried;

  Vec3 base, dir;
  r.globalToLocal(_trans, base, dir);

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
  const Flt h1z = base.z + (dir.z * h1);
  const bool h1_hit = (h1z >= -1.0 && h1z <= 1.0);

  const Flt h2 = (-b + sqrt_x) / a;
  const Flt h2z = base.z + (dir.z * h2);
  const bool h2_hit = (h2z >= -1.0 && h2z <= 1.0);

  const int hits = int(h1_hit) + int(h2_hit);
  if (hits == 0) { return 0; }

  Flt near_h;
  if (hits == 2) {
    near_h = std::min(h1, h2);
  } else {
    near_h = h1_hit ? h1 : h2;
  }

  hit_list.addHit(this, near_h, CalcHitPoint(base, dir, near_h), 0, HIT_NORMAL);
  ++r.stats->open_cone_hit;
  return 1;
}

int OpenCone::evalHit(const HitInfo& h, Vec3& normal, Vec3& map) const
{
  Vec3 n{h.local_pt.x, h.local_pt.y, (1.0 - h.local_pt.z) / 4.0};
  normal = _trans.normalLocalToGlobal(n, 0);

  Vec2 dir = UnitVec(Vec2{h.local_pt.x, h.local_pt.y});
  Flt x = std::clamp(dir.x, -1.0 + VERY_SMALL, 1.0 - VERY_SMALL);
  Flt u = (std::acos(x) * (2.0/PI)) - 1.0;

  map.set((h.local_pt.y >= 0.0) ? u : -u, h.local_pt.z, 0.0);
  return 0;
}


// **** OpenCylinder Class ****
// Object Functions
int OpenCylinder::intersect(const Ray& r, bool csg, HitList& hit_list) const
{
  ++r.stats->open_cylinder_tried;

  Vec3 base, dir;
  r.globalToLocal(_trans, base, dir);

  Flt a = Sqr(dir.x) + Sqr(dir.y);
  Flt b = (dir.x * base.x) + (dir.y * base.y);
  Flt c = Sqr(base.x) + Sqr(base.y) - 1.0;
  Flt x = Sqr(b) - (a * c);
  if (x < VERY_SMALL) {
    return 0;  // missed (avoid single intersection case)
  }

  Flt sqrt_x = std::sqrt(x);
  Flt far_h = (-b + sqrt_x) / a;
  if (far_h < VERY_SMALL) {
    return 0;  // cylinder completely behind ray origin
  }

  int hits = 0;

  Flt near_h = (-b - sqrt_x) / a;
  Vec3 near_pt = CalcHitPoint(base, dir, near_h);
  if ((near_pt.z >= -1.0) && (near_pt.z <= 1.0)) {
    hit_list.addHit(this, near_h, near_pt, 0, HIT_NORMAL);
    ++hits;
  }

  if (csg || hits == 0) {
    Vec3 far_pt = CalcHitPoint(base, dir, far_h);
    if ((far_pt.z >= -1.0) && (far_pt.z <= 1.0)) {
      hit_list.addHit(this, far_h, far_pt, 0, HIT_NORMAL);
      ++hits;
    }
  }

  if (hits > 0) { ++r.stats->open_cylinder_hit; }
  return hits;
}

int OpenCylinder::evalHit(const HitInfo& h, Vec3& normal, Vec3& map) const
{
  Vec3 n{h.local_pt.x, h.local_pt.y, 0.0};
  normal = _trans.normalLocalToGlobal(n, 0);

  Flt x = std::clamp(h.local_pt.x, -1.0 + VERY_SMALL, 1.0 - VERY_SMALL);
  Flt u = (std::acos(x) * (2.0/PI)) - 1.0;
  map.set((h.local_pt.y >= 0.0) ? u : -u, h.local_pt.z, 0.0);
  return 0;
}


// **** Paraboloid Class ****
// Object Functions
int Paraboloid::intersect(const Ray& r, bool csg, HitList& hit_list) const
{
  ++r.stats->paraboloid_tried;

  Vec3 base, dir;
  r.globalToLocal(_trans, base, dir);

  Flt a = Sqr(dir.x) + Sqr(dir.y);
  Flt b = (dir.x * base.x) + (dir.y * base.y) + dir.z * .25;
  Flt c = Sqr(base.x) + Sqr(base.y) + (base.z - .5);
  Flt x = Sqr(b) - (a * c);
  if (x < VERY_SMALL) {
    return 0;  // paraboloid missed (avoid single intersection case)
  }

  // Find hit points on paraboloid
  Flt sqrt_x = std::sqrt(x);
  Flt far_h = (-b + sqrt_x) / a;
  if (far_h < 0) {
    return 0;  // paraboloid completely behind ray origin
  }

  int hits = 0;

  Flt near_h = (-b - sqrt_x) / a;
  Vec3 near_pt = CalcHitPoint(base, dir, near_h);
  if (near_pt.z >= -1.0) {
    hit_list.addHit(this, near_h, near_pt, 0, HIT_NORMAL);
    ++hits;
  }

  if (hits == 0) {
    Vec3 far_pt = CalcHitPoint(base, dir, far_h);
    if (far_pt.z >= -1.0) {
      hit_list.addHit(this, far_h, far_pt, 0, HIT_NORMAL);
      ++hits;
    }
  }

  if (hits > 0) { ++r.stats->paraboloid_hit; }
  return hits;
}

int Paraboloid::evalHit(const HitInfo& h, Vec3& normal, Vec3& map) const
{
  Vec3 n{h.local_pt.x, h.local_pt.y, .125};
  normal = _trans.normalLocalToGlobal(n, 0);

  map.set((h.local_pt.z > 0.0) ? h.local_pt.x : -h.local_pt.x,
	  h.local_pt.y, 0.0);
  return 0;
}


// **** Plane Class ****
// SceneItem Functions
int Plane::init(Scene& s)
{
  if (Primitive::init(s)) { return -1; }

  normal_cache = _trans.normalLocalToGlobal({0,0,1}, 0); // cache plane normal
  return 0;
}

// Object Functions
int Plane::intersect(const Ray& r, bool csg, HitList& hit_list) const
{
  ++r.stats->plane_tried;

  Vec3 base, dir;
  r.globalToLocal(_trans, base, dir);
  if (IsZero(dir.z)) {
    return 0;  // parallel with plane
  }

  Flt h = -base.z / dir.z;
  if (h < 0) {
    return 0;  // plane behind ray origin
  }

  Vec3 pt;
  pt.x = base.x + (dir.x * h);
  if ((pt.x < -1.0) || (pt.x > 1.0)) {
    return 0;
  }

  pt.y = base.y + (dir.y * h);
  if ((pt.y < -1.0) || (pt.y > 1.0)) {
    return 0;
  }

  pt.z = 0.0;
  hit_list.addHit(this, h, pt, 0, HIT_NORMAL);
  ++r.stats->plane_hit;
  return 1;
}

int Plane::evalHit(const HitInfo& h, Vec3& normal, Vec3& map) const
{
  normal = normal_cache;
  map = h.local_pt;
  return 0;
}

int Plane::bound(BBox& b) const
{
  static constexpr Vec3 pt[4] = {
    { 1, 1, 0}, {-1, 1, 0}, { 1,-1, 0}, {-1,-1, 0}};
  b.fit(_trans, pt, 4);
  return 0;
}


// **** Sphere Class ****
// Object Functions
int Sphere::intersect(const Ray& r, bool csg, HitList& hit_list) const
{
  ++r.stats->sphere_tried;

  Vec3 base, dir;
  r.globalToLocal(_trans, base, dir);

  Flt a = DotProduct(dir,  dir);
  Flt b = DotProduct(base, dir);
  Flt c = DotProduct(base, base) - 1.0;
  Flt x = Sqr(b) - (a * c);
  if (x < VERY_SMALL) {
    return 0;  // missed (avoid single intersection case)
  }

  // Find hit points on sphere
  Flt sqrt_x = std::sqrt(x);
  Flt far_h = (-b + sqrt_x) / a;
  if (far_h < VERY_SMALL) {
    return 0;  // sphere completely behind ray origin
  }

  Flt near_h = (-b - sqrt_x) / a;
  hit_list.addHit(this, near_h, CalcHitPoint(base, dir, near_h), 0,
                  csg ? HIT_ENTER : HIT_NORMAL);
  if (csg) {
    hit_list.addHit(this, far_h, CalcHitPoint(base, dir, far_h), 0, HIT_EXIT);
  }

  ++r.stats->sphere_hit;
  return 1 + int(csg);
}

int Sphere::evalHit(const HitInfo& h, Vec3& normal, Vec3& map) const
{
  normal = _trans.normalLocalToGlobal(h.local_pt, 0);

  map.set((h.local_pt.z > 0.0) ? h.local_pt.x : -h.local_pt.x,
	  h.local_pt.y, 0.0);
  return 0;
}


// **** Torus Class ****
// Object Functions
int Torus::intersect(const Ray& r, bool csg, HitList& hit_list) const
{
  ++r.stats->torus_tried;

  Vec3 base, dir;
  r.globalToLocal(_trans, base, dir);

  Flt bd = DotProduct(base, dir);
  Flt dd = DotProduct(dir,  dir);
  Flt t1 = DotProduct(base, base) + (1.0 - Sqr(_radius));

  Flt c[5];
  c[4] = Sqr(dd);
  c[3] = 4 * bd * dd;
  c[2] = 4 * Sqr(bd) + 2 * t1 * dd - 4 * (Sqr(dir.x) + Sqr(dir.z));
  c[1] = 4 * bd * t1 - 8 * (base.x * dir.x + base.z * dir.z);
  c[0] = Sqr(t1) - 4 * (Sqr(base.x) + Sqr(base.z));

  Flt root[4];
  int n = SolveQuartic(c, root);
  if ((n != 2) && (n != 4)) {
    return 0;
  }

  // NOTE: output order of SolveQuartic() ok for addHit() logic below
  // if 2 hits:
  //   root[0] < root[1]
  // if 4 hits:
  //   root[0] < root[1] && root[2] < root[3]
  //   (root[1] < root[2] not always true, however)

  if (csg) {
    bool enter = true;
    for (int i = 0; i < n; ++i) {
      hit_list.addHit(this, root[i], CalcHitPoint(base, dir, root[i]), 0,
                      enter ? HIT_ENTER : HIT_EXIT);
      enter = !enter;
    }
  } else {
    Flt h = (n == 4 && root[2] < root[0]) ? root[2] : root[0];
    hit_list.addHit(this, h, CalcHitPoint(base, dir, h), 0, HIT_NORMAL);
  }

  ++r.stats->torus_hit;
  return n;
}

int Torus::evalHit(const HitInfo& h, Vec3& normal, Vec3& map) const
{
  Flt x = h.local_pt.x;
  Flt z = h.local_pt.z;
  Flt d = Sqr(x) + Sqr(z);
  if (!IsPositive(d)) { return -1; }

  Flt sqrt_d = std::sqrt(d);
  Vec3 n{x - (x / sqrt_d), h.local_pt.y, z - (z / sqrt_d)};
  normal = _trans.normalLocalToGlobal(n, 0);

  map.set((h.local_pt.y >= 0.0) ? h.local_pt.x : -h.local_pt.x,
	  -h.local_pt.z, 0.0);
  return 0;
}

int Torus::bound(BBox& b) const
{
  Vec3 pt[8] = {
    { 1 + _radius,  _radius,  1 + _radius},
    {-1 - _radius,  _radius,  1 + _radius},
    { 1 + _radius, -_radius,  1 + _radius},
    { 1 + _radius,  _radius, -1 - _radius},
    {-1 - _radius, -_radius,  1 + _radius},
    { 1 + _radius, -_radius, -1 - _radius},
    {-1 - _radius,  _radius, -1 - _radius},
    {-1 - _radius, -_radius, -1 - _radius}};
  b.fit(_trans, pt, 8);
  return 0;
}
