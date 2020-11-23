//
// HitCostInfo.cc
// Copyright (C) 2020 Richard Bradley
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
  cone          = 1.2;
  csg           = .2;
  cube          = 1.5;
  cylinder      = 1.4;
  open_cone     = 1.0;
  open_cylinder = 1.0;
  paraboloid    = 1.0;
  plane         = .75;
  sphere        = 1.0;
  torus         = 3.0;
}
