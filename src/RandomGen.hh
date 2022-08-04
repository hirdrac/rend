//
// RandomGen.hh
// Copyright (C) 2022 Richard Bradley
//

#pragma once
#include "MathUtility.hh"
#include "Types.hh"
#include <random>


class RandomGen
{
 public:
  RandomGen(uint64_t seed = devRnd64())
    : _rnd{seed}, _seed{seed} { }

  [[nodiscard]] uint64_t seed() const { return _seed; }

  Flt jitter() { return _jitterDist(_rnd); }
    // random value in [-.5, .5)

  Vec2 diskPt() {
    // random point on diameter=1 disk
    Flt x, y;
    do {
      x = jitter();
      y = jitter();
    } while ((Sqr(x) + Sqr(y)) > .25);
    return {x, y};
  }

  Vec3 dir() {
    // random direction unit vector
    Vec3 d{INIT_NONE};
    Flt len2;
    do {
      d.x = jitter();
      d.y = jitter();
      d.z = jitter();
      len2 = d.lengthSqr();
    } while (len2 > .25 || !IsPositive(len2));
    return d / std::sqrt(len2);
  }

  Vec3 hemisphereDir(const Vec3& normal) {
    Vec3 d{INIT_NONE};
    Flt dot;
    do {
      d = dir();
      dot = DotProduct(normal, d);
    } while (IsZero(dot));
    return (dot < 0.0) ? -d : d;
  }

  static uint32_t devRnd32() { return _devRnd(); }
  static uint64_t devRnd64() {
    return uint64_t(_devRnd()) | (uint64_t(_devRnd()) << 32); }

 private:
  std::mt19937_64 _rnd;
  std::uniform_real_distribution<Flt> _jitterDist{-.5,.5};
  uint64_t _seed;

  inline static std::random_device _devRnd;
};
