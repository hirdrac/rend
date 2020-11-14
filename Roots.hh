//
// Roots.hh - revision 5 (2019/1/1)
// Copyright(C) 2019 by Richard Bradley
//
// Functions for solving polynomial equations
//

#ifndef Roots_hh
#define Roots_hh

#include "Types.hh"


// **** Functions ****
int SolveQuadric(const Flt c[3], Flt s[2]);
int   SolveCubic(const Flt c[4], Flt s[3]);
int SolveQuartic(const Flt c[5], Flt s[4]);

#endif
