//
// Stats.cc
// Copyright (C) 2021 Richard Bradley
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
  paraboloid    += s.paraboloid;
  plane         += s.plane;
  prism         += s.prism;
  sphere        += s.sphere;
  torus         += s.torus;
  return *this;
}
