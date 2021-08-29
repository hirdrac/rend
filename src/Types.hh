//
// Types.hh
// Copyright (C) 2021 Richard Bradley
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
constexpr Flt VERY_SMALL = 1.0e-12;
constexpr Flt VERY_LARGE = 1.0e99;

constexpr Flt PI = math::PI<Flt>;


// **** Macros ****
#ifndef __has_builtin
#define __has_builtin(x) 0
#endif
#if !__has_builtin(__builtin_expect)
#error "__builtin_expect() required"
#endif

#define LIKELY(x) __builtin_expect(!!(x), 1)
#define UNLIKELY(x) __builtin_expect(!!(x), 0)

// disable with
//#define LIKELY(x) (x)
//#define UNLIKELY(x) (x)

//#define ALWAYS_INLINE __attribute__((always_inline))
//#define NOINLINE __attribute__((noinline))
//#define PURE __attribute__((pure))
