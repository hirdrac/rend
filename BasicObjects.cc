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
// Constructor
Disc::Disc()
{
  _solid = false;
}

// SceneItem Functions
int Disc::init(Scene& s)
{
  if (Primitive::init(s)) { return -1; }

  normal_cache = _trans.normalLocalToGlobal({0,0,1}, 0);  // cache plane normal
  return 0;
}

// Object Functions
int Disc::intersect(const Ray& r, HitList& hit_list) const
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

  hit_list.addHit(this, h, pt);
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
  return MakeBound(b, _trans, pt, 4);
}


// **** Cone Class ****
// Constructor
Cone::Cone()
{
  _solid = true;
}

// SceneItem Functions
int Cone::init(Scene& s)
{
  if (Primitive::init(s)) { return -1; }

  normal_cache = _trans.normalLocalToGlobal({0,0,-1}, 0);
  return 0;
}

// Object Functions
int Cone::intersect(const Ray& r, HitList& hit_list) const
{
  ++r.stats->cone_tried;

  Vec3 base, dir;
  r.globalToLocal(_trans, base, dir);

  Flt a = Sqr(dir.x) + Sqr(dir.y) - (0.25 * Sqr(dir.z));
  Flt b = (base.x * dir.x) + (base.y * dir.y) +
    (dir.z * (1.0 - base.z) * 0.25);
  Flt c = Sqr(base.x) + Sqr(base.y) - (0.25 * Sqr(1.0 - base.z));
  Flt x = Sqr(b) - (a * c);

  int hits = 0;
  if (IsPositive(x)) {
    Flt sqrt_x = std::sqrt(x);
    Flt h = (-b - sqrt_x) / a;  // near hit
    Vec3 pt = CalcHitPoint(base, dir, h);
    if ((pt.z >= -1.0) && (pt.z <= 1.0)) {
      // hit side
      hit_list.addHit(this, h, pt);
      ++hits;
    }

    h = (-b + sqrt_x) / a;  // far hit
    pt = CalcHitPoint(base, dir, h);
    if ((pt.z >= -1.0) && (pt.z <= 1.0)) {
      // hit side
      hit_list.addHit(this, h, pt);
      ++hits;
    }
  }

  if (!IsZero(dir.z)) {
    Flt h = -(base.z + 1.0) / dir.z;
    Vec3 pt = CalcHitPoint(base, dir, h);
    if ((Sqr(pt.x) + Sqr(pt.y)) <= 1.0) {
      // hit base
      hit_list.addHit(this, h, pt, 1);
      ++hits;
    }
  }

  if (hits > 0) { ++r.stats->cone_hit; }
  return hits;
}

int Cone::evalHit(const HitInfo& h, Vec3& normal, Vec3& map) const
{
  if (h.side == 1) {
    // base
    normal = normal_cache;
    map.set(-(h.local_pt.x), h.local_pt.y, -1.0);

  } else {
    // side
    Vec3 n{h.local_pt.x * 2.0, h.local_pt.y * 2.0,
	   (1.0 - h.local_pt.z) * .25};
    normal = _trans.normalLocalToGlobal(n, 0);

    Vec2 dir = {h.local_pt.x, h.local_pt.y};
    dir.normalize();
    Flt x = std::clamp(dir.x, -1.0 + VERY_SMALL, 1.0 - VERY_SMALL);
    Flt u = (std::acos(x) * (2.0/PI)) - 1.0;
    map.set((h.local_pt.y >= 0.0) ? u : -u, h.local_pt.z, 0.0);
  }

  return 0;
}


// **** Cube Class ****
// Constructor
Cube::Cube()
{
  _solid = true;
}

// SceneItem Functions
int Cube::init(Scene& s)
{
  if (Primitive::init(s)) { return -1; }

  static constexpr Vec3 n[6] = {
    {1, 0, 0}, {-1, 0, 0}, {0, -1, 0}, {0, 1, 0}, {0, 0, 1}, {0, 0, -1}};

  for (int i = 0; i < 6; ++i) {
    normal_cache[i] = _trans.normalLocalToGlobal(n[i], 0);
  }

  return 0;
}

// Object Functions
int Cube::intersect(const Ray& r, HitList& hit_list) const
{
  ++r.stats->cube_tried;

  Vec3 base, dir;
  r.globalToLocal(_trans, base, dir);

  Flt near_hit = -VERY_LARGE, far_hit = VERY_LARGE;
  int near_side = 0, far_side = 0;

  // X
  if (!IsZero(dir.x)) {
    Flt h1 = (-1.0 - base.x) / dir.x;
    Flt h2 = (1.0 - base.x) / dir.x;
    if (h1 > h2) {
      near_hit = h2;
      near_side = 0;
      far_hit = h1;
      far_side = 1;
    } else {
      near_hit = h1;
      near_side = 1;
      far_hit = h2;
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
      if (h2 > near_hit) { near_hit = h2; near_side = 2; }
      if (h1 < far_hit)  { far_hit  = h1; far_side  = 3; }
    } else {
      if (h1 > near_hit) { near_hit = h1; near_side = 3; }
      if (h2 < far_hit)  { far_hit  = h2; far_side  = 2; }
    }

    if (near_hit > far_hit) {
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
      if (h2 > near_hit) { near_hit = h2; near_side = 4; }
      if (h1 < far_hit)  { far_hit  = h1; far_side  = 5; }
    } else {
      if (h1 > near_hit) { near_hit = h1; near_side = 5; }
      if (h2 < far_hit)  { far_hit  = h2; far_side  = 4; }
    }

    if (near_hit > far_hit) {
      return 0;  // Miss
    }
  } else if ((base.z < -1.0) || (base.z > 1.0)) {
    return 0;  // Miss
  }

  if (far_hit < 0) {
    return 0;  // cube completely behind ray origin
  }

  hit_list.addHit(this, near_hit, CalcHitPoint(base, dir, near_hit), near_side);
  hit_list.addHit(this, far_hit, CalcHitPoint(base, dir, far_hit), far_side);
  ++r.stats->cube_hit;
  return 2;
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
// Constructor
Cylinder::Cylinder()
{
  _solid = true;
}

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
int Cylinder::intersect(const Ray& r, HitList& hit_list) const
{
  ++r.stats->cylinder_tried;

  Vec3 base, dir;
  r.globalToLocal(_trans, base, dir);

  // Intersect cylinder side
  Flt a = Sqr(dir.x) + Sqr(dir.y);
  Flt b = (dir.x * base.x) + (dir.y * base.y);
  Flt c = Sqr(base.x) + Sqr(base.y) - 1.0;
  Flt x = Sqr(b) - (a * c);

  Flt near_hit = -VERY_LARGE, far_hit = VERY_LARGE;
  if (IsPositive(x)) {
    Flt sqrt_x = std::sqrt(x);
    near_hit = (-b - sqrt_x) / a;  // cylinder hit 1
    far_hit  = (-b + sqrt_x) / a;  // cylinder hit 2
  } else if (IsNegative(x) || ((Sqr(base.x) + Sqr(base.y)) > 1.0)) {
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
      if (h2 > near_hit) { near_hit = h2; near_side = 2; }
      if (h1 < far_hit)  { far_hit  = h1; far_side  = 1; }

    } else {
      if (h1 > near_hit) { near_hit = h1; near_side = 1; }
      if (h2 < far_hit)  { far_hit  = h2; far_side  = 2; }
    }

    if (near_hit > far_hit) {
      return 0;  // cylinder missed
    }
  }

  if (far_hit < 0) {
    return 0;  // cylinder completely behind ray origin
  }

  hit_list.addHit(this, near_hit, CalcHitPoint(base, dir, near_hit), near_side);
  hit_list.addHit(this, far_hit, CalcHitPoint(base, dir, far_hit), far_side);
  ++r.stats->cylinder_hit;
  return 2;
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
// Constructor
OpenCone::OpenCone()
{
  _solid = false;
}

// Object Functions
int OpenCone::intersect(const Ray& r, HitList& hit_list) const
{
  ++r.stats->open_cone_tried;

  Vec3 base, dir;
  r.globalToLocal(_trans, base, dir);

  Flt a = Sqr(dir.x) + Sqr(dir.y) - (0.25 * Sqr(dir.z));
  Flt b = (base.x * dir.x) + (base.y * dir.y) +
    (dir.z * (1.0 - base.z) * 0.25);
  Flt c = Sqr(base.x) + Sqr(base.y) - (0.25 * Sqr(1.0 - base.z));
  Flt x = Sqr(b) - (a * c);
  if (x < VERY_SMALL) {
    return 0;  // Cone missed
  }

  Flt sqrt_x = std::sqrt(x);
  Flt h1 = (-b + sqrt_x) / a;  // side hit
  Flt h2 = (-b - sqrt_x) / a;  // side hit
  if ((h1 < 0) && (h2 < 0)) {
    return 0;  // cone completely behind ray origin
  }

  Vec3 pt;
  int hits = 0;

  pt = CalcHitPoint(base, dir, h1);
  if ((pt.z >= -1.0) && (pt.z <= 1.0)) {
    hit_list.addHit(this, h1, pt);
    ++hits;
  }

  pt = CalcHitPoint(base, dir, h2);
  if ((pt.z >= -1.0) && (pt.z <= 1.0)) {
    hit_list.addHit(this, h2, pt);
    ++hits;
  }

  if (hits > 0) { ++r.stats->open_cone_hit; }
  return hits;
}

int OpenCone::evalHit(const HitInfo& h, Vec3& normal, Vec3& map) const
{
  Vec3 n{h.local_pt.x * 2.0,
	 h.local_pt.y * 2.0,
	 (1.0 - h.local_pt.z) * .25};
  normal = _trans.normalLocalToGlobal(n, 0);

  Vec2 dir{h.local_pt.x, h.local_pt.y};
  dir.normalize();
  Flt x = std::clamp(dir.x, -1.0 + VERY_SMALL, 1.0 - VERY_SMALL);
  Flt u = (std::acos(x) * (2.0/PI)) - 1.0;

  map.set((h.local_pt.y >= 0.0) ? u : -u, h.local_pt.z, 0.0);
  return 0;
}


// **** OpenCylinder Class ****
// Constructor
OpenCylinder::OpenCylinder()
{
  _solid = false;
}

// Object Functions
int OpenCylinder::intersect(const Ray& r, HitList& hit_list) const
{
  ++r.stats->open_cylinder_tried;

  Vec3 base, dir;
  r.globalToLocal(_trans, base, dir);

  Flt a = Sqr(dir.x) + Sqr(dir.y);
  Flt b = (dir.x * base.x) + (dir.y * base.y);
  Flt c = Sqr(base.x) + Sqr(base.y) - 1.0;
  Flt x = Sqr(b) - (a * c);

  if (x < VERY_SMALL) {
    return 0; // missed
  }

  Flt sqrt_x = std::sqrt(x);
  Flt h = (-b + sqrt_x) / a;  // far hit
  if (h < VERY_SMALL) {
    return 0;  // cylinder completely behind ray origin
  }

  Vec3 pt;
  int hits = 0;

  pt = CalcHitPoint(base, dir, h);
  if ((pt.z >= -1.0) && (pt.z <= 1.0)) {
    hit_list.addHit(this, h, pt);
    ++hits;
  }

  h = (-b - sqrt_x) / a;  // near hit
  pt = CalcHitPoint(base, dir, h);
  if ((pt.z >= -1.0) && (pt.z <= 1.0)) {
    hit_list.addHit(this, h, pt);
    ++hits;
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
// Constructor
Paraboloid::Paraboloid()
{
  _solid = false;
}

// Object Functions
int Paraboloid::intersect(const Ray& r, HitList& hit_list) const
{
  ++r.stats->paraboloid_tried;

  Vec3 base, dir;
  r.globalToLocal(_trans, base, dir);

  Flt a = Sqr(dir.x) + Sqr(dir.y);
  Flt b = (dir.x * base.x) + (dir.y * base.y) + dir.z * .25;
  Flt c = Sqr(base.x) + Sqr(base.y) + (base.z - .5);
  Flt x = Sqr(b) - (a * c);
  if (x < VERY_SMALL) {
    return 0;  // paraboloid missed
  }

  // Find hit points on paraboloid
  Flt sqrt_x = std::sqrt(x);
  Flt h = (-b + sqrt_x) / a;  // far hit
  if (h < 0) {
    return 0;  // paraboloid completely behind ray origin
  }

  Vec3 pt;
  int hits = 0;

  pt = CalcHitPoint(base, dir, h);
  if (pt.z >= -1.0) {
    hit_list.addHit(this, h, pt);
    ++hits;
  }

  h = (-b - sqrt_x) / a;  // near hit
  pt = CalcHitPoint(base, dir, h);
  if (pt.z >= -1.0) {
    hit_list.addHit(this, h, pt);
    ++hits;
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
// Constructor
Plane::Plane()
{
  _solid = false;
}

// SceneItem Functions
int Plane::init(Scene& s)
{
  if (Primitive::init(s)) { return -1; }

  normal_cache = _trans.normalLocalToGlobal({0,0,1}, 0); // cache plane normal
  return 0;
}

// Object Functions
int Plane::intersect(const Ray& r, HitList& hit_list) const
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
  hit_list.addHit(this, h, pt);
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
  return MakeBound(b, _trans, pt, 4);
}


// **** Sphere Class ****
// Constructor
Sphere::Sphere()
{
  _solid = true;
}

// Object Functions
int Sphere::intersect(const Ray& r, HitList& hit_list) const
{
  ++r.stats->sphere_tried;

  Vec3 base, dir;
  r.globalToLocal(_trans, base, dir);

  Flt a = DotProduct(dir,  dir);
  Flt b = DotProduct(base, dir);
  Flt c = DotProduct(base, base) - 1.0;
  Flt x = Sqr(b) - (a * c);

  if (x < VERY_SMALL) {
    return 0;  // missed
  }

  // Find hit points on sphere
  Flt sqrt_x = std::sqrt(x);
  Flt h = (-b + sqrt_x) / a;  // far hit
  if (h < VERY_SMALL) {
    return 0;  // sphere completely behind ray origin
  }

  hit_list.addHit(this, h, CalcHitPoint(base, dir, h));

  h = (-b - sqrt_x) / a;  // near hit
  hit_list.addHit(this, h, CalcHitPoint(base, dir, h));

  ++r.stats->sphere_hit;
  return 2;
}

int Sphere::evalHit(const HitInfo& h, Vec3& normal, Vec3& map) const
{
  normal = _trans.normalLocalToGlobal(h.local_pt, 0);

  map.set((h.local_pt.z > 0.0) ? h.local_pt.x : -h.local_pt.x,
	  h.local_pt.y, 0.0);
  return 0;
}


// **** Torus Class ****
// Constructor
Torus::Torus() :
  radius(.5)
{
  _solid = true;
}

// SceneItem Functions
int Torus::init(Scene& s)
{
  if (Primitive::init(s)) { return -1; }

  r_constant = 1.0 - Sqr(radius);
  return 0;
}

// Object Functions
int Torus::intersect(const Ray& r, HitList& hit_list) const
{
  ++r.stats->torus_tried;

  Vec3 base, dir;
  r.globalToLocal(_trans, base, dir);

  Flt bd = DotProduct(base, dir);
  Flt dd = DotProduct(dir,  dir);
  Flt t1 = DotProduct(base, base) + r_constant;

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

  for (int i = 0; i < n; ++i) {
    hit_list.addHit(this, root[i], CalcHitPoint(base, dir, root[i]));
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

  d = std::sqrt(d);
  Vec3 n{x - (x / d), h.local_pt.y, z - (z / d)};
  n.normalize();
  normal = _trans.normalLocalToGlobal(n, 0);

  map.set((h.local_pt.y >= 0.0) ? h.local_pt.x : -h.local_pt.x,
	  -h.local_pt.z, 0.0);
  return 0;
}

int Torus::bound(BBox& b) const
{
  Vec3 pt[8] = {
    { 1 + radius,  radius,  1 + radius}, {-1 - radius,  radius,  1 + radius},
    { 1 + radius, -radius,  1 + radius}, { 1 + radius,  radius, -1 - radius},
    {-1 - radius, -radius,  1 + radius}, { 1 + radius, -radius, -1 - radius},
    {-1 - radius,  radius, -1 - radius}, {-1 - radius, -radius, -1 - radius}};
  return MakeBound(b, _trans, pt, 8);
}
