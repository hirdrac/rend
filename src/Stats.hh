//
// Stats.hh
// Copyright (C) 2021 Richard Bradley
//
// raytracing statistics gathering
//

#pragma once
#include <cstdint>


// **** Types ****
class StatInfo
{
 public:
  struct RayStats {
    uint64_t tried = 0, hit = 0;

    RayStats& operator+=(const RayStats& x) {
      tried += x.tried; hit += x.hit; return *this; }
  };

  RayStats rays;
  RayStats shadow_rays;
  RayStats bound;
  RayStats disc;
  RayStats cone;
  RayStats cube;
  RayStats cylinder;
  RayStats open_cone;
  RayStats open_cylinder;
  RayStats paraboloid;
  RayStats plane;
  RayStats sphere;
  RayStats torus;

  // Member Functions
  StatInfo& operator+=(const StatInfo& stats);
};
