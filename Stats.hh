//
// Stats.hh - revision 14 (2019/1/3)
// Copyright(C) 2019 by Richard Bradley
//
// raytracing statistics gathering
//

#ifndef Stats_hh
#define Stats_hh

#include "Types.hh"
#include <iosfwd>


// **** Types ****
class Scene;

class InventoryInfo
{
 public:
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
  uint64_t rays_cast;
  uint64_t rays_hit;
  uint64_t shadow_rays_cast;
  uint64_t shadow_rays_hit;
  uint64_t bound_tried;
  uint64_t bound_hit;
  uint64_t disc_tried;
  uint64_t disc_hit;
  uint64_t cone_tried;
  uint64_t cone_hit;
  uint64_t cube_tried;
  uint64_t cube_hit;
  uint64_t cylinder_tried;
  uint64_t cylinder_hit;
  uint64_t open_cone_tried;
  uint64_t open_cone_hit;
  uint64_t open_cylinder_tried;
  uint64_t open_cylinder_hit;
  uint64_t paraboloid_tried;
  uint64_t paraboloid_hit;
  uint64_t plane_tried;
  uint64_t plane_hit;
  uint64_t sphere_tried;
  uint64_t sphere_hit;
  uint64_t torus_tried;
  uint64_t torus_hit;

  // Constructor
  StatInfo() { clear(); }

  // Member Functions
  void clear();
  void print(const Scene& s, std::ostream& out) const;
};


// **** Globals ****
extern InventoryInfo Inventory;
extern StatInfo Stats;

#endif
