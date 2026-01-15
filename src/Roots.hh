//
// Roots.hh
// Copyright (C) 2026 Richard Bradley
//
// Functions for solving polynomial equations
//

#pragma once
#include "Types.hh"


// **** Functions ****
int solveQuadric(const Flt c[3], Flt s[2]);
int solveCubic(const Flt c[4], Flt s[3]);
int solveQuartic(const Flt c[5], Flt s[4]);
