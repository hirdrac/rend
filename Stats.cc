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


// **** StatInfo Class ****
void StatInfo::print(const Scene& s, std::ostream& out) const
{
  uint64_t object_tried = disc.tried + cone.tried + cube.tried + cylinder.tried
    + open_cone.tried + open_cylinder.tried + paraboloid.tried + plane.tried
    + sphere.tried + torus.tried;

  uint64_t object_hit = disc.hit + cone.hit + cube.hit + cylinder.hit
    + open_cone.hit + open_cylinder.hit + paraboloid.hit + plane.hit
    + sphere.hit + torus.hit;

  int64_t objs = Inventory.objects - (Inventory.groups + Inventory.bounds);
  int64_t total_rays = int64_t(rays.tried + shadow_rays.tried);
  int64_t dumb_tries = total_rays * objs;
  int64_t total_tries = int64_t(object_tried + bound.tried);

  out << "       Rays Cast  " << rays.tried;
  out << "\n        Rays Hit  " << rays.hit;
  out << "\nShadow Rays Cast  " << shadow_rays.tried;
  out << "\n Shadow Rays Hit  " << shadow_rays.hit;
  out << "\n     Light Count  " << Inventory.lights;
  out << "\n    Shader Count  " << Inventory.shaders;
  out << "\n    Object Count  " << Inventory.objects;
  out << "\n     Bound Count  " << Inventory.bounds;
  out << "\n     Group Count  " << Inventory.groups;
  out << "\n       CSG Count  " << Inventory.csgs;
  out << "\n   Objects Tried  " << object_tried;
  out << "\n     Objects Hit  " << object_hit;
  out << "\n    Bounds Tried  " << bound.tried;
  out << "\n      Bounds Hit  " << bound.hit;
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

StatInfo& StatInfo::operator+=(const StatInfo& s)
{
  rays          += s.rays;
  shadow_rays   += s.shadow_rays;
  bound         += s.bound;
  disc          += s.disc;
  cone          += s.cone;
  cube          += s.cube;
  cylinder      += s.cylinder;
  open_cone     += s.open_cone;
  open_cylinder += s.open_cylinder;
  paraboloid    += s.paraboloid;
  plane         += s.plane;
  sphere        += s.sphere;
  torus         += torus;
  return *this;
}
