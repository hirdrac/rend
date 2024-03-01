//
// RandomDist.hh
// Copyright (C) 2024 Richard Bradley
//

#pragma once
#include "MathUtility.hh"
#include "Types.hh"
#include <random>


class JitterDistribution
{
 public:
  JitterDistribution() { }
  JitterDistribution(Flt scale) : _dist{-.5 * scale, .5 * scale} { }

  template<class Generator>
  [[nodiscard]] Flt operator()(Generator& g) { return _dist(g); }

 private:
  std::uniform_real_distribution<Flt> _dist;
};


class DiskDistribution
{
 public:
  DiskDistribution() { }
  DiskDistribution(Flt radius)
    : _dist{-radius, radius}, _radiusSqr{Sqr(radius)} { }

  template<class Generator>
  [[nodiscard]] Vec2 operator()(Generator& g) {
    Vec2 pt{INIT_NONE};
    do {
      pt.x = _dist(g);
      pt.y = _dist(g);
    } while (pt.lengthSqr() > _radiusSqr);
    return pt;
  }

 private:
  std::uniform_real_distribution<Flt> _dist;
  Flt _radiusSqr = 0;
};


class SphereDistribution
{
 public:
  SphereDistribution() { }
  SphereDistribution(Flt radius)
    : _dist{-radius, radius}, _radiusSqr{Sqr(radius)} { }

  template<class Generator>
  [[nodiscard]] Vec3 operator()(Generator& g) {
    Vec3 pt{INIT_NONE};
    do {
      pt.x = _dist(g);
      pt.y = _dist(g);
      pt.z = _dist(g);
    } while (pt.lengthSqr() > _radiusSqr);
    return pt;
  }

 private:
  std::uniform_real_distribution<Flt> _dist;
  Flt _radiusSqr = 0;
};


class UnitDirDistribution
{
 public:
  template<class Generator>
  [[nodiscard]] Vec3 operator()(Generator& g) {
    Vec3 d{INIT_NONE};
    Flt len2;
    do {
      d.x = _dist(g);
      d.y = _dist(g);
      d.z = _dist(g);
      len2 = d.lengthSqr();
    } while (len2 > 1.0 || !IsPositive(len2));
    return d / std::sqrt(len2);
  }

  template<class Generator>
  [[nodiscard]] Vec3 operator()(Generator& g, const Vec3& normal) {
    // hemisphere dir
    Vec3 d{INIT_NONE};
    Flt dot;
    do {
      d = operator()(g);
      dot = DotProduct(normal, d);
    } while (IsZero(dot));
    return (dot < 0.0) ? -d : d;
  }

 private:
  std::uniform_real_distribution<Flt> _dist{-1,1};
};
