//
// Types.hh
// Copyright (C) 2023 Richard Bradley
//
// Definition of standard types and constants
//

#pragma once
#include "Matrix3D.hh"
#include "Vector3D.hh"
#include "MathUtility.hh"


// **** Types ****
using Flt = double;
using Vec2 = Vector2<Flt>;
using Vec3 = Vector3<Flt>;
using Matrix = Matrix4x4<Flt,ROW_MAJOR>;


// **** Constants ****
constexpr Flt VERY_SMALL = math::VERY_SMALL<Flt>;
constexpr Flt VERY_LARGE = 1.0e99;

constexpr Flt PI = math::PI<Flt>;


// **** Macros ****
//#define ALWAYS_INLINE __attribute__((always_inline))
//#define NOINLINE __attribute__((noinline))
//#define PURE __attribute__((pure))
