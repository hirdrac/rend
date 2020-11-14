//
// Types.hh - revision 37 (2019/1/1)
// Copyright(C) 2019 by Richard Bradley
//
// Definition of standard types and constants
//

#ifndef Types_hh
#define Types_hh

#include "Matrix3D.hh"
#include "Vector3D.hh"
#include "MathUtility.hh"


// **** Types ****
using Flt = double;
using Vec2 = Vector2<Flt>;
using Vec3 = Vector3<Flt>;
using Matrix = Matrix4x4<Flt,ROW_MAJOR>;


// **** Constants ****
constexpr Flt VERY_SMALL = 1.0e-12;
constexpr Flt VERY_LARGE = 1.0e99;

constexpr Flt PI = MathVal<Flt>::PI;

#endif
