//
// HitCostInfo.cc
// Copyright (C) 2021 Richard Bradley
//

#include "HitCostInfo.hh"


// **** HitCostInfo Class ****
void HitCostInfo::reset()
{
  bound         = .5;
  disc          = .8;
  cone          = 1.5;
  csg           = .3;
  cube          = 1.2;
  cylinder      = 1.7;
  paraboloid    = 1.3;
  plane         = .8;
  sphere        = 1.0;
  torus         = 7.0;
}
