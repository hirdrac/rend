//
// HitCostInfo.cc
// Copyright (C) 2021 Richard Bradley
//

#include "HitCostInfo.hh"


// **** HitCostInfo Class ****
void HitCostInfo::reset()
{
  bound         = .8;
  disc          = .9;
  cone          = 1.5;
  csg           = .3;
  cube          = 1.2;
  cylinder      = 1.7;
  paraboloid    = 1.3;
  plane         = .85;
  sphere        = 1.0;
  torus         = 7.0;
}
