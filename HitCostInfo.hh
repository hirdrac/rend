//
// HitCostInfo.hh - revision 1 (2019/1/3)
// Copyright(C) 2019 by Richard Bradley
//
// primitive hit cost values used for bound creation
//

#ifndef HitCostInfo_hh
#define HitCostInfo_hh

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

#endif
