//
// JobState.hh
// Copyright (C) 2025 Richard Bradley
//
// Job (thread) specific data used in rendering
//

#pragma once
#include "HitInfo.hh"
#include "Stats.hh"
#include "RandomDist.hh"
#include "Types.hh"


struct JobState
{
  HitCache cache;
  StatInfo stats;

  // random state
  std::mt19937_64 rnd;
  JitterDistribution jitterXDist;
  JitterDistribution jitterYDist;
  DiskDistribution apertureDist;
  UnitDirDistribution dirDist;

  // helper functions
  void init(const Scene& s);

  [[nodiscard]] Flt rndJitterX() {
    return jitterXDist(rnd); }
  [[nodiscard]] Flt rndJitterY() {
    return jitterYDist(rnd); }
  [[nodiscard]] Vec2 rndAperturePt() {
    return apertureDist(rnd); }
  [[nodiscard]] Vec3 rndDir() {
    return dirDist(rnd); }
  [[nodiscard]] Vec3 rndHemisphereDir(const Vec3& normal) {
    return dirDist(rnd, normal); }
};
