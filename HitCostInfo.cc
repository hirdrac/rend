//
// HitCostInfo.cc
// Copyright (C) 2021 Richard Bradley
//

#include "HitCostInfo.hh"


// **** Globals ****
HitCostInfo CostTable;


// **** HitCostInfo Class ****
// Member Functions
void HitCostInfo::reset()
{
  bound         = .5;
  disc          = .75;
  cone          = 1.6;
  csg           = 1.0;
  cube          = 1.2;
  cylinder      = 1.8;
  open_cone     = 1.3;
  open_cylinder = 1.3;
  paraboloid    = 1.3;
  plane         = .75;
  sphere        = 1.0;
  torus         = 15.0;
}
