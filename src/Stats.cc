//
// Stats.cc
// Copyright (C) 2021 Richard Bradley
//
// Implementation of stats module
//

#include "Stats.hh"


// **** StatInfo Class ****
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
  torus         += s.torus;
  return *this;
}
