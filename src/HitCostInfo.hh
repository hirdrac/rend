//
// HitCostInfo.hh
// Copyright (C) 2021 Richard Bradley
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
  Flt paraboloid;
  Flt plane;
  Flt sphere;
  Flt torus;

  HitCostInfo() { reset(); }

  // Member Functions
  void reset();
};
