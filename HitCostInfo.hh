//
// HitCostInfo.hh
// Copyright (C) 2020 Richard Bradley
//
// primitive hit cost values used for bound creation
//

#pragma once
#include "Types.hh"


// **** Types ****
class HitCostInfo
{
 public:
  Flt bound;
  Flt disc;
  Flt cone;
  Flt csg;
  Flt cube;
  Flt cylinder;
  Flt open_cone;
  Flt open_cylinder;
  Flt paraboloid;
  Flt plane;
  Flt sphere;
  Flt torus;

  // Constructor
  HitCostInfo() { reset(); }

  // Member Functions
  void reset();
};


// **** Globals ****
extern HitCostInfo CostTable;