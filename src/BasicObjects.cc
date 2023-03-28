//
// BasicObjects.cc
// Copyright (C) 2023 Richard Bradley
//

#include "BasicObjects.hh"
#include "Intersect.hh"
#include "Ray.hh"
#include "Roots.hh"
#include "Stats.hh"
#include "BBox.hh"
#include "HitCostInfo.hh"
#include "Print.hh"
#include "RegisterObject.hh"
#include <algorithm>
#include <cmath>


// shared data
static constexpr Vec3 planeBoundPoints[4] = {
  { 1, 1, 0}, {-1, 1, 0}, { 1,-1, 0}, {-1,-1, 0}};


// **** Disc Class ****
REGISTER_OBJECT_CLASS(Disc,"disc");

int Disc::init(Scene& s, const Transform* tr)
{
  _normal = _trans.normalLocalToGlobal({0,0,1}, 0);  // cache plane normal
  return 0;
}

BBox Disc::bound(const Matrix* t) const
{
  return {std::data(planeBoundPoints), std::size(planeBoundPoints),
          t ? *t : _trans.final()};
}

Flt Disc::hitCost(const HitCostInfo& hc) const
{
  return (_cost >= 0.0) ? _cost : hc.disc;
}

int Disc::intersect(const Ray& r, HitList& hl) const
{
  ++hl.stats().disc.tried;

  const Vec3 dir = _trans.rayLocalDir(r);
  if (dir.z == 0.0) [[unlikely]] {
    return 0;  // parallel with disc plane
  }

  const Vec3 base = _trans.rayLocalBase(r);
  const Flt h = -base.z / dir.z;
  if (!r.inRange(h)) {
    return 0;
  }

  const Vec3 pt{base.x + (dir.x * h), base.y + (dir.y * h), 0.0};
  if ((Sqr(pt.x) + Sqr(pt.y)) > 1.0) {
    return 0;
  }

  ++hl.stats().disc.hit;
  hl.addHit(this, h, pt, 0, HIT_NORMAL);
  return 1;
}

Vec3 Disc::normal(const Ray& r, const HitInfo& h) const
{
  return _normal;
}


// **** Cone Class ****
REGISTER_OBJECT_CLASS(Cone,"cone");

int Cone::init(Scene& s, const Transform* tr)
{
  _baseNormal = _trans.normalLocalToGlobal({0,0,-1}, 0);
  return 0;
}

int Cone::intersect(const Ray& r, HitList& hl) const
{
  // modified z-axis cone:
  //   x^2 + y^2 - ((1-z)/2)^2 = 0
  //  (single cone in z=-1 to 1 range; points at +Z; fits in unit cube)
  //
  // general quadric surface equation form:
  //   x^2 + y^2 - .25z^2 + .5z - .25 = 0
  // (A=1 B=1 C=-.25 D=0 E=0 F=0 G=0 H=0 I=.5 J=-.25)
  //
  // Aq = xd^2 + yd^2 - .25z^2
  // Bq = 2xoxd + 2yoyd - .5zozd + .5zd
  //    = 2xoxd + 2yoyd + .5zd(1 - zo)
  // Cq = xo^2 + yo^2 - .25zo^2 +.5zo - .25
  //    = xo^2 + yo^2 - .25(zo - 1)^2

  ++hl.stats().cone.tried;
  const Vec3 dir = _trans.rayLocalDir(r);
  const Vec3 base = _trans.rayLocalBase(r);

  Flt h[2];
  int side[2];
  int hits = 0;

  const Flt a = Sqr(dir.x) + Sqr(dir.y) - (.25 * Sqr(dir.z));
  const Flt b = (base.x * dir.x) + (base.y * dir.y)
    + (.25 * dir.z * (1.0 - base.z));
  const Flt c = Sqr(base.x) + Sqr(base.y) - (.25 * Sqr(base.z - 1.0));
  if (IsZero(a)) {
    // ray parallel with cone side (1 hit)
    const Flt h1 = -c / (2.0 * b);
    const Flt h1z = base.z + (dir.z * h1);
    if (h1z >= -1.0 && h1z <= 1.0) {
      h[hits] = h1; side[hits] = 0; ++hits;
    }
  } else {
    // solve quadradic
    const Flt d = Sqr(b) - (a * c);
    if (d < VERY_SMALL) { return 0; } // 0 or 1 hit

    const Flt sqrt_d = std::sqrt(d);

    const Flt h1 = (-b - sqrt_d) / a;
    const Flt h1z = base.z + (dir.z * h1);
    if (h1z >= -1.0 && h1z <= 1.0) {
      h[hits] = h1; side[hits] = 0; ++hits;
    }

    const Flt h2 = (-b + sqrt_d) / a;
    const Flt h2z = base.z + (dir.z * h2);
    if (h2z >= -1.0 && h2z <= 1.0) {
      h[hits] = h2; side[hits] = 0; ++hits;
    }
  }

  if (hits == 1 && dir.z != 0.0) {
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

  if (far_h < r.min_length || near_h >= r.max_length) {
    return 0;
  }

  if (hl.csg()) {
    ++hl.stats().cone.hit;
    hl.addHit(
      this, near_h, CalcHitPoint(base, dir, near_h), near_side, HIT_ENTER);
    hl.addHit(
      this, far_h, CalcHitPoint(base, dir, far_h), far_side, HIT_EXIT);
    return 2;
  }

  if (near_h < r.min_length) {
    if (far_h >= r.max_length) { return 0; }
    near_h = far_h; near_side = far_side;
  }

  ++hl.stats().cone.hit;
  hl.addHit(
    this, near_h, CalcHitPoint(base, dir, near_h), near_side, HIT_NORMAL);
  return 1;
}

Vec3 Cone::normal(const Ray& r, const HitInfo& h) const
{
  if (h.side == 1) { return _baseNormal; }

  // cone normal calculation:
  // (A=1 B=1 C=-.25 D=0 E=0 F=0 G=0 H=0 I=.5 J=-.25)
  // xn = 2*A*xi + D*yi + E*zi + G =  2xi
  // yn = 2*B*yi + D*xi + F*zi + H =  2yi
  // zn = 2*C*zi + E*xi + F*yi + I = -.5zi + .5

  const Vec3 n{h.local_pt.x, h.local_pt.y, .25 * (1.0 - h.local_pt.z)};
  return _trans.normalLocalToGlobal(n, r.time);
}

Flt Cone::hitCost(const HitCostInfo& hc) const
{
  return (_cost >= 0.0) ? _cost : hc.cone;
}


// **** Cube Class ****
REGISTER_OBJECT_CLASS(Cube,"cube");

int Cube::init(Scene& s, const Transform* tr)
{
  static constexpr Vec3 n[6] = {
    {-1, 0, 0}, {1, 0, 0}, {0, -1, 0}, {0, 1, 0}, {0, 0, -1}, {0, 0, 1}};

  for (int i = 0; i < 6; ++i) {
    _sideNormal[i] = _trans.normalLocalToGlobal(n[i], 0);
  }

  return 0;
}

Flt Cube::hitCost(const HitCostInfo& hc) const
{
  return (_cost >= 0.0) ? _cost : hc.cube;
}

int Cube::intersect(const Ray& r, HitList& hl) const
{
  ++hl.stats().cube.tried;

  const Vec3 dir = _trans.rayLocalDir(r);
  const Vec3 base = _trans.rayLocalBase(r);

  Flt near_h = -VERY_LARGE, far_h = VERY_LARGE;
  int near_side = -1, far_side = -1;

  // X
  if (dir.x != 0.0) [[likely]] {
    const Flt h1 = (-1.0 - base.x) / dir.x;
    const Flt h2 = ( 1.0 - base.x) / dir.x;
    if (h1 < h2) {
      near_h = h1; near_side = 0;
      far_h = h2; far_side = 1;
    } else {
      near_h = h2; near_side = 1;
      far_h = h1; far_side = 0;
    }
  } else if (Abs(base.x) > 1.0) {
    return 0;  // Miss
  }

  // Y & Z
  for (unsigned int i = 1; i < 3; ++i) {
    if (dir[i] != 0.0) [[likely]] {
      const int s = int(i)*2;
      const Flt h1 = (-1.0 - base[i]) / dir[i];
      const Flt h2 = ( 1.0 - base[i]) / dir[i];
      if (h1 < h2) {
        if (h1 > near_h) { near_h = h1; near_side = s; }
        if (h2 < far_h)  { far_h  = h2; far_side  = s+1; }
      } else {
        if (h2 > near_h) { near_h = h2; near_side = s+1; }
        if (h1 < far_h)  { far_h  = h1; far_side  = s; }
      }

      if (near_h > far_h) {
        return 0;  // Miss
      }
    } else if (Abs(base[i]) > 1.0) {
      return 0;  // Miss
    }
  }

  if (far_h < r.min_length || near_h >= r.max_length) {
    return 0;
  }

  if (hl.csg()) {
    ++hl.stats().cube.hit;
    hl.addHit(
      this, near_h, CalcHitPoint(base, dir, near_h), near_side, HIT_ENTER);
    hl.addHit(
      this, far_h, CalcHitPoint(base, dir, far_h), far_side, HIT_EXIT);
    return 2;
  }

  if (near_h < r.min_length) {
    if (far_h >= r.max_length) { return 0; }
    near_h = far_h; near_side = far_side;
  }

  ++hl.stats().cube.hit;
  hl.addHit(
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
REGISTER_OBJECT_CLASS(Cylinder,"cylinder");

int Cylinder::init(Scene& s, const Transform* tr)
{
  _endNormal[0] = _trans.normalLocalToGlobal({0, 0,  1}, 0);
  _endNormal[1] = _trans.normalLocalToGlobal({0, 0, -1}, 0);
  return 0;
}

Flt Cylinder::hitCost(const HitCostInfo& hc) const
{
  return (_cost >= 0.0) ? _cost : hc.cylinder;
}

int Cylinder::intersect(const Ray& r, HitList& hl) const
{
  // z-axis cylinder:
  //   x^2 + y^2 - 1 = 0
  // (A=1 B=1 C=0 D=0 E=0 F=0 G=0 H=0 I=0 J=-1)
  //
  // Aq = xd^2 + yd^2
  // Bq = 2xoxd + 2yoyd
  // Cq = xo^2 + yo^2 - 1

  ++hl.stats().cylinder.tried;
  const Vec3 dir = _trans.rayLocalDir(r);
  const Vec3 base = _trans.rayLocalBase(r);

  const Flt a = Sqr(dir.x) + Sqr(dir.y);
  const Flt b = (dir.x * base.x) + (dir.y * base.y);
  const Flt c = Sqr(base.x) + Sqr(base.y) - 1.0;
  const Flt d = Sqr(b) - (a * c);

  Flt near_h = -VERY_LARGE, far_h = VERY_LARGE;
  if (IsPositive(d)) {
    const Flt sqrt_d = std::sqrt(d);
    near_h = (-b - sqrt_d) / a;  // cylinder hit 1
    far_h  = (-b + sqrt_d) / a;  // cylinder hit 2
  } else if (IsNegative(d) || c > 0.0) {
    // cylinder missed completely or
    // ray parallel with cylinder side but outside cylinder
    return 0;
  }

  // Intersect cylinder ends
  int near_side = 0, far_side = 0;
  if (dir.z != 0.0) [[likely]] {
    Flt h1 = -(base.z - 1.0) / dir.z;  // plane hit 1
    int s1 = 1;
    Flt h2 = -(base.z + 1.0) / dir.z;  // plane hit 2
    int s2 = 2;
    if (h1 > h2) { std::swap(h1, h2); std::swap(s1, s2); }
    if (h1 > near_h) { near_h = h1; near_side = s1; }
    if (h2 < far_h)  { far_h  = h2; far_side  = s2; }

    if (near_h > far_h) {
      return 0;  // cylinder missed
    }
  } else if (Abs(base.z) > 1.0) {
    return 0; // ray parallel with planes but not between planes
  }

  if (far_h < r.min_length || near_h >= r.max_length) {
    return 0;
  }

  if (hl.csg()) {
    ++hl.stats().cylinder.hit;
    hl.addHit(
      this, near_h, CalcHitPoint(base, dir, near_h), near_side, HIT_ENTER);
    hl.addHit(
      this, far_h, CalcHitPoint(base, dir, far_h), far_side, HIT_EXIT);
    return 2;
  }

  if (near_h < r.min_length) {
    if (far_h >= r.max_length) { return 0; }
    near_h = far_h; near_side = far_side;
  }

  ++hl.stats().cylinder.hit;
  hl.addHit(
    this, near_h, CalcHitPoint(base, dir, near_h), near_side, HIT_NORMAL);
  return 1;
}

Vec3 Cylinder::normal(const Ray& r, const HitInfo& h) const
{
  if (h.side == 1) { return _endNormal[0]; }
  else if (h.side == 2) { return _endNormal[1]; }

  // cylinder normal calculation:
  // (A=1 B=1 C=0 D=0 E=0 F=0 G=0 H=0 I=0 J=-1)
  // xn = 2*A*xi + D*yi + E*zi + G = 2xi
  // yn = 2*B*yi + D*xi + F*zi + H = 2yi
  // zn = 2*C*zi + E*xi + F*yi + I = 0

  const Vec3 n{h.local_pt.x, h.local_pt.y, 0.0};
  return _trans.normalLocalToGlobal(n, r.time);
}


// **** Paraboloid Class ****
REGISTER_OBJECT_CLASS(Paraboloid,"paraboloid");

int Paraboloid::init(Scene& s, const Transform* tr)
{
  _baseNormal = _trans.normalLocalToGlobal({0,0,-1}, 0);
  return 0;
}

int Paraboloid::intersect(const Ray& r, HitList& hl) const
{
  // modified z-axis elliptic paraboloid:
  // (paraboloid in -1 to 1 range; points at +Z; fits in unit cube
  //   x^2 + y^2 - .5(1 - z) = 0
  //
  // general quadric surface equation form:
  //   x^2 + y^2 +.5z - .5
  // (A=1 B=1 C=0 D=0 E=0 F=0 G=0 H=0 I=.5 J=-.5)
  //
  // Aq = xd^2 + yd^2
  // Bq = 2xoxd + 2yoyd + .5zd
  // Cq = xo^2 + yo^2 + .5zo -.5

  ++hl.stats().paraboloid.tried;
  const Vec3 dir = _trans.rayLocalDir(r);
  const Vec3 base = _trans.rayLocalBase(r);

  Flt h[2];
  int side[2];
  int hits = 0;

  const Flt a = Sqr(dir.x) + Sqr(dir.y);
  const Flt b = (dir.x * base.x) + (dir.y * base.y) + (.25 * dir.z);
  const Flt c = Sqr(base.x) + Sqr(base.y) + (.5 * base.z) - .5;
  if (IsZero(a)) {
    // special single intersection case
    const Flt h1 = -c / (2.0 * b);
    if ((base.z + (dir.z * h1)) >= -1.0) {
      h[hits] = h1; side[hits] = 0; ++hits;
    }
  } else {
    // solve quadradic
    const Flt d = Sqr(b) - (a * c);
    if (d < VERY_SMALL) { return 0; } // 0 or 1 hit

    const Flt sqrt_d = std::sqrt(d);

    const Flt h1 = (-b - sqrt_d) / a;
    if ((base.z + (dir.z * h1)) >= -1.0) {
      h[hits] = h1; side[hits] = 0; ++hits;
    }

    const Flt h2 = (-b + sqrt_d) / a;
    if ((base.z + (dir.z * h2)) >= -1.0) {
      h[hits] = h2; side[hits] = 0; ++hits;
    }
  }

  if (hits == 1 && dir.z != 0.0) {
    // base cap hit
    const Flt h1 = -(base.z + 1.0) / dir.z;
    h[hits] = h1; side[hits] = 1; ++hits;
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

  if (far_h < r.min_length || near_h >= r.max_length) {
    return 0;
  }

  if (hl.csg()) {
    ++hl.stats().paraboloid.hit;
    hl.addHit(
      this, near_h, CalcHitPoint(base, dir, near_h), near_side, HIT_ENTER);
    hl.addHit(
      this, far_h, CalcHitPoint(base, dir, far_h), far_side, HIT_EXIT);
    return 2;
  }

  if (near_h < r.min_length) {
    if (far_h >= r.max_length) { return 0; }
    near_h = far_h; near_side = far_side;
  }

  ++hl.stats().paraboloid.hit;
  hl.addHit(
    this, near_h, CalcHitPoint(base, dir, near_h), near_side, HIT_NORMAL);
  return 1;
}

Vec3 Paraboloid::normal(const Ray& r, const HitInfo& h) const
{
  if (h.side == 1) { return _baseNormal; }

  // paraboloid normal calculation:
  // (A=1 B=1 C=0 D=0 E=0 F=0 G=0 H=0 I=.5 J=-.5)
  // xn = 2*A*xi + D*yi + E*zi + G = 2xi
  // yn = 2*B*yi + D*xi + F*zi + H = 2yi
  // zn = 2*C*zi + E*xi + F*yi + I = .5

  const Vec3 n{h.local_pt.x, h.local_pt.y, .25};
  return _trans.normalLocalToGlobal(n, r.time);
}

Flt Paraboloid::hitCost(const HitCostInfo& hc) const
{
  return (_cost >= 0.0) ? _cost : hc.paraboloid;
}


// **** Plane Class ****
REGISTER_OBJECT_CLASS(Plane,"plane");

int Plane::init(Scene& s, const Transform* tr)
{
  _normal = _trans.normalLocalToGlobal({0,0,1}, 0); // cache plane normal
  return 0;
}

BBox Plane::bound(const Matrix* t) const
{
  return {std::data(planeBoundPoints), std::size(planeBoundPoints),
          t ? *t : _trans.final()};
}

Flt Plane::hitCost(const HitCostInfo& hc) const
{
  return (_cost >= 0.0) ? _cost : hc.plane;
}

int Plane::intersect(const Ray& r, HitList& hl) const
{
  ++hl.stats().plane.tried;

  const Vec3 dir = _trans.rayLocalDir(r);
  if (dir.z == 0.0) [[unlikely]] {
    return 0;  // parallel with plane
  }

  const Vec3 base = _trans.rayLocalBase(r);
  const Flt h = -base.z / dir.z;
  if (!r.inRange(h)) {
    return 0;
  }

  const Flt px = base.x + (dir.x * h);
  if (Abs(px) > 1.0) {
    return 0;
  }

  const Flt py = base.y + (dir.y * h);
  if (Abs(py) > 1.0) {
    return 0;
  }

  ++hl.stats().plane.hit;
  hl.addHit(this, h, {px,py,0.0}, 0, HIT_NORMAL);
  return 1;
}

Vec3 Plane::normal(const Ray& r, const HitInfo& h) const
{
  return _normal;
}


// **** Sphere Class ****
REGISTER_OBJECT_CLASS(Sphere,"sphere");

Flt Sphere::hitCost(const HitCostInfo& hc) const
{
  return (_cost >= 0.0) ? _cost : hc.sphere;
}

int Sphere::intersect(const Ray& r, HitList& hl) const
{
  // unit sphere:
  //   x^2 + y^2 + z^2 - 1 = 0
  // (A=1 B=1 C=1 D=0 E=0 F=0 G=0 H=0 I=0 J=-1)
  //
  // Aq = xd^2 + yd^2 + zd^2
  // Bq = 2xoxd + 2yoyd + 2zozd
  // Cq = xo^2 + yo^2 + zo^2 - 1

  ++hl.stats().sphere.tried;
  const Vec3 dir = _trans.rayLocalDir(r);
  const Vec3 base = _trans.rayLocalBase(r);

  const Flt a = DotProduct(dir,  dir);
  const Flt b = DotProduct(base, dir);
  const Flt c = DotProduct(base, base) - 1.0;
  const Flt d = Sqr(b) - (a * c);
  if (d < VERY_SMALL) {
    return 0;  // missed (avoid single intersection case)
  }

  // Find hit points on sphere
  const Flt sqrt_d = std::sqrt(d);
  const Flt far_h = (-b + sqrt_d) / a;
  if (far_h < r.min_length) { return 0; }

  Flt near_h = (-b - sqrt_d) / a;
  if (near_h >= r.max_length) { return 0; }

  if (hl.csg()) {
    ++hl.stats().sphere.hit;
    hl.addHit(this, near_h, CalcHitPoint(base, dir, near_h), 0, HIT_ENTER);
    hl.addHit(this, far_h, CalcHitPoint(base, dir, far_h), 0, HIT_EXIT);
    return 2;
  }

  if (near_h < r.min_length) {
    if (far_h >= r.max_length) { return 0; }
    near_h = far_h;
  }

  ++hl.stats().sphere.hit;
  hl.addHit(this, near_h, CalcHitPoint(base, dir, near_h), 0, HIT_NORMAL);
  return 1;
}

Vec3 Sphere::normal(const Ray& r, const HitInfo& h) const
{
  // sphere normal calculation:
  // (A=1 B=1 C=1 D=0 E=0 F=0 G=0 H=0 I=0 J=-1)
  // xn = 2*A*xi + D*yi + E*zi + G = 2xi
  // yn = 2*B*yi + D*xi + F*zi + H = 2yi
  // zn = 2*C*zi + E*xi + F*yi + I = 2zi

  return _trans.normalLocalToGlobal(h.local_pt, r.time);
}


// **** Torus Class ****
REGISTER_OBJECT_CLASS(Torus,"torus");

int Torus::init(Scene& s, const Transform* tr)
{
  if (_radius < VERY_SMALL) {
    println_err("Bad 'radius' value of ", _radius);
    return -1;
  }

  return 0;
}

BBox Torus::bound(const Matrix* t) const
{
  const Flt r1 = _radius + 1.0;
  const Vec3 pt[8] = {
    { r1,  _radius,  r1}, {-r1,  _radius,  r1},
    { r1, -_radius,  r1}, { r1,  _radius, -r1},
    {-r1, -_radius,  r1}, { r1, -_radius, -r1},
    {-r1,  _radius, -r1}, {-r1, -_radius, -r1}};
  return {std::data(pt), std::size(pt), t ? *t : _trans.final()};
}

Flt Torus::hitCost(const HitCostInfo& hc) const
{
  return (_cost >= 0.0) ? _cost : hc.torus;
}

int Torus::intersect(const Ray& r, HitList& hl) const
{
  // R=1 torus (symmetric about z-axis):
  // (x^2 + y^2 + z^2 - r^2 + 1)^2 - 4(x^2 + y^2) = 0

  ++hl.stats().torus.tried;

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

  Flt near_h, far_h;
  if (n == 2) {
    near_h = root[0];
    far_h = root[1];
  } else {
    near_h = std::min(root[0], root[2]);
    far_h = std::max(root[1], root[3]);
  }

  if (far_h < r.min_length || near_h >= r.max_length) {
    return 0;
  }

  if (hl.csg()) {
    ++hl.stats().torus.hit;
    for (int i = 0; i < n; ++i) {
      hl.addHit(this, root[i], CalcHitPoint(base, dir, root[i]), 0,
                (i&1) ? HIT_EXIT : HIT_ENTER);
    }
    return n;
  }

  Flt h = VERY_LARGE;
  for (int i = 0; i < n; ++i) {
    if (root[i] >= r.min_length && root[i] < h) { h = root[i]; }
  }

  if (h >= r.max_length) { return 0; }

  ++hl.stats().torus.hit;
  hl.addHit(this, h, CalcHitPoint(base, dir, h), 0, HIT_NORMAL);
  return 1;
}

Vec3 Torus::normal(const Ray& r, const HitInfo& h) const
{
  // previous normal calc
  //const Flt x = h.local_pt.x;
  //const Flt z = h.local_pt.z;
  //const Flt sqrt_d = std::sqrt(Sqr(x) + Sqr(z));
  //const Vec3 n{x - (x / sqrt_d), h.local_pt.y, z - (z / sqrt_d)};

  const Flt a = DotProduct(h.local_pt, h.local_pt) - 1.0 - Sqr(_radius);
  const Vec3 n = h.local_pt * Vec3{a, a + 2.0, a};

  return _trans.normalLocalToGlobal(n, r.time);
}


// **** NOTES ****
// Ray - Quadric Intersection
// (from "Practical Ray Tracing in C"
//  by Craig A. Lindley, John Wiley & Sons, 1992, pp. 86-89)
//
// General quadric surface equation:
// Ax^2 + By^2 + Cz^2 + Dxy + Exz + Fyz + Gx + Hy + Iz + J = 0
//
// Ray: Ro + Rd*t  (Ro:xo,yo,zo, Rd:xd,yd,zd)
//
// Intersection:
// Aq = Axd^2 + Byd^2 + Czd^2 + Dxdyd + Exdzd + Fydzd
// Bq = 2*Axoxd + 2*Byoyd + 2*Czozd + D(xoyd + yoxd) + E(xozd + zoxd)
//      + F(yozd + ydzo) + Gxd + Hyd + Izd
// Cq = Axo^2 + Byo^2 + Czo^2 + Dxoyo + Exozo + Fyozo + Gxo + Hyo + Izo + J
//
// if Aq = 0 then t = -Cq / Bq (single intersection)
// otherwise, check Dq
//
// Quadradic formula discriminant:
// Dq = Bq^2 - 4AqCq
//
// Dq < 0: miss
// Dq = 0: 1 hit
// Dq > 0: 2 hits
//
// Intersection points
// t0 = (-Bq - sqrt(Dq)) / (2Aq)
// t1 = (-Bq + sqrt(Dq)) / (2Aq)
//
// Normal Equation (xi,yi,zi is intersection point)
// xn = 2*A*xi + D*yi + E*zi + G
// yn = 2*B*yi + D*xi + F*zi + H
// zn = 2*C*zi + E*xi + F*yi + I
// (not normalized)
//
// ****
