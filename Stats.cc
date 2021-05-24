//
// Stats.cc
// Copyright (C) 2021 Richard Bradley
//
// Implementation of stats module
//

#include "Stats.hh"
#include "Scene.hh"


// **** Globals ****
InventoryInfo Inventory;
StatInfo Stats;


// **** StatInfo Class ****
void StatInfo::clear()
{
  // Clear render stats
  rays_cast           = 0;
  rays_hit            = 0;
  shadow_rays_cast    = 0;
  bound_tried         = 0;
  bound_hit           = 0;
  disc_tried          = 0;
  disc_hit            = 0;
  cone_tried          = 0;
  cone_hit            = 0;
  cube_tried          = 0;
  cube_hit            = 0;
  cylinder_tried      = 0;
  cylinder_hit        = 0;
  open_cone_tried     = 0;
  open_cone_hit       = 0;
  open_cylinder_tried = 0;
  open_cylinder_hit   = 0;
  paraboloid_tried    = 0;
  paraboloid_hit      = 0;
  plane_tried         = 0;
  plane_hit           = 0;
  sphere_tried        = 0;
  sphere_hit          = 0;
  torus_tried         = 0;
  torus_hit           = 0;
}

void StatInfo::print(const Scene& s, std::ostream& out) const
{
  uint64_t object_tried = disc_tried + cone_tried + cylinder_tried
    + open_cone_tried + open_cylinder_tried + paraboloid_tried
    + plane_tried + sphere_tried + torus_tried;

  uint64_t object_hit = disc_hit + cone_hit + cylinder_hit + open_cone_hit
    + open_cylinder_hit + paraboloid_hit + plane_hit + sphere_hit + torus_hit;

  int64_t objs = Inventory.objects - (Inventory.groups + Inventory.bounds);
  uint64_t total_rays = rays_cast + shadow_rays_cast;
  uint64_t dumb_tries = total_rays * objs;
  uint64_t total_tries = object_tried + bound_tried;

  out << "       Rays Cast  " << rays_cast;
  out << "\nShadow Rays Cast  " << shadow_rays_cast;
  out << "\n        Rays Hit  " << rays_hit;
  out << "\n     Light Count  " << Inventory.lights;
  out << "\n    Shader Count  " << Inventory.shaders;
  out << "\n    Object Count  " << Inventory.objects;
  out << "\n     Bound Count  " << Inventory.bounds;
  out << "\n     Group Count  " << Inventory.groups;
  out << "\n       CSG Count  " << Inventory.csgs;
  out << "\n   Objects Tried  " << object_tried;
  out << "\n     Objects Hit  " << object_hit;
  out << "\n    Bounds Tried  " << bound_tried;
  out << "\n      Bounds Hit  " << bound_hit;
  out << "\n";
  out << "\n Total Rays Cast  " << total_rays;
  out << "\n Total Hit Tries  " << total_tries;
  out << "\n  Dumb Hit Tries  " << dumb_tries;
  if (dumb_tries > 0) {
    out << "\n       Reduction  "
	<< (Flt(dumb_tries - total_tries) / Flt(dumb_tries)) * 100.0 << '%';
  }

  out << '\n';
}
