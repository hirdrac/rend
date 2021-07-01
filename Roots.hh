//
// Roots.hh
// Copyright (C) 2021 Richard Bradley
//
// Functions for solving polynomial equations
//

#pragma once
#include "Types.hh"


// **** Functions ****
int SolveQuadric(const Flt c[3], Flt s[2]);
int   SolveCubic(const Flt c[4], Flt s[3]);
int SolveQuartic(const Flt c[5], Flt s[4]);
