//
// Stats.hh
// Copyright (C) 2021 Richard Bradley
//
// raytracing statistics gathering
//

#pragma once
#include "Types.hh"
#include <iosfwd>


// **** Types ****
class Scene;

struct InventoryInfo
{
  uint32_t bounds = 0;
  uint32_t csgs = 0;
  uint32_t groups = 0;
  uint32_t lights = 0;
  uint32_t objects = 0;
  uint32_t shaders = 0;
};

class StatInfo
{
 public:
  struct RayStats {
    uint64_t tried= 0, hit = 0;

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
  void print(const Scene& s, std::ostream& out) const;

  StatInfo& operator+=(const StatInfo& stats);
};


// **** Globals ****
extern InventoryInfo Inventory;
