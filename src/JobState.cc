//
// JobState.cc
// Copyright (C) 2023 Richard Bradley
//

#include "JobState.hh"
#include "Scene.hh"
#include <random>


[[nodiscard]] static uint64_t randomSeed()
{
  static std::random_device rnd;
  return uint64_t{rnd()} | (uint64_t{rnd()} << 32);
}


void JobState::init(const Scene& s)
{
  stats = {};
  rnd.seed(randomSeed());
  jitterXDist = JitterDistribution{s.jitter / Flt(std::max(s.sample_x, 1))};
  jitterYDist = JitterDistribution{s.jitter / Flt(std::max(s.sample_y, 1))};
  apertureDist = DiskDistribution{.5};
}
