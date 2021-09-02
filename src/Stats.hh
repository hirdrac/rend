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
  RayStats paraboloid;
  RayStats plane;
  RayStats prism;
  RayStats sphere;
  RayStats torus;

  // Member Functions
  StatInfo& operator+=(const StatInfo& stats);

  uint64_t objectsTried() const {
    return disc.tried + cone.tried + cube.tried + cylinder.tried
      + paraboloid.tried + plane.tried + prism.tried + sphere.tried
      + torus.tried;
  }

  uint64_t objectsHit() const {
    return disc.hit + cone.hit + cube.hit + cylinder.hit
      + paraboloid.hit + plane.hit + prism.hit + sphere.hit
      + torus.hit;
  }
};
