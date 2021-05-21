//
// MathUtility.hh
// Copyright (C) 2021 Richard Bradley
//
// fun and useful numeric constants and
// various numeric functions needing a home
//

#pragma once
#include <cmath>


// **** Constants ****
namespace math {
  // various math constants
  template<class T>
#ifndef M_PI
  constexpr T PI = static_cast<T>(3.14159265358979323846);
#else
  constexpr T PI = static_cast<T>(M_PI);
#endif

  template<class T>
#ifndef M_PI_2
  constexpr T PI_2 = static_cast<T>(1.57079632679489661923);
#else
  constexpr T PI_2 = static_cast<T>(M_PI_2);  // pi/2
#endif

  template<class T>
#ifndef M_PI_4
  constexpr T PI_4 = static_cast<T>(0.78539816339744830962);
#else
  constexpr T PI_4 = static_cast<T>(M_PI_4);  // pi/4
#endif

  template<class T>
  constexpr T DEG_TO_RAD = PI<T> / static_cast<T>(180);

  template<class T>
  constexpr T RAD_TO_DEG = static_cast<T>(180) / PI<T>;

  template<class T>
  constexpr T VERY_SMALL = static_cast<T>(1.0e-12);

  template<>
  constexpr float VERY_SMALL<float> = 1.0e-7f;
};


// **** Functions ****
template<typename fltType>
[[nodiscard]] constexpr fltType DegToRad(fltType deg)
{
  return deg * math::DEG_TO_RAD<fltType>;
}

template<typename fltType>
[[nodiscard]] constexpr fltType RadToDeg(fltType rad)
{
  return rad * math::RAD_TO_DEG<fltType>;
}

template<typename fltType>
[[nodiscard]] constexpr bool IsZero(fltType x)
{
  return (x > -math::VERY_SMALL<fltType>) && (x < math::VERY_SMALL<fltType>);
}

template<typename fltType>
[[nodiscard]] constexpr bool IsOne(fltType x)
{
  return (x > (static_cast<fltType>(1) - math::VERY_SMALL<fltType>))
    && (x < (static_cast<fltType>(1) + math::VERY_SMALL<fltType>));
}

template<typename fltType>
[[nodiscard]] constexpr bool IsPositive(fltType x)
{
  return (x >= math::VERY_SMALL<fltType>);
}

template<typename fltType>
[[nodiscard]] constexpr bool IsNegative(fltType x)
{
  return (x <= -math::VERY_SMALL<fltType>);
}

template<typename fltType>
[[nodiscard]] constexpr bool IsEqual(fltType x, fltType y)
{
  return IsZero(x-y);
}

template<typename fltType>
[[nodiscard]] constexpr bool IsLess(fltType x, fltType y)
{
  return IsNegative(x-y);
}

template<typename fltType>
[[nodiscard]] constexpr bool IsGreater(fltType x, fltType y)
{
  return IsPositive(x-y);
}

template<typename intType>
[[nodiscard]] constexpr bool IsPowerOf2(intType x)
{
  static_assert(std::is_integral_v<intType>);
  return (x & (x - 1)) == 0;
}

template<typename numType, typename fltType>
[[nodiscard]] constexpr numType Lerp(numType a, numType b, fltType s)
{
  // use std::lerp() for C++20
  static_assert(std::is_floating_point_v<fltType>);
  if (s <= 0) {
    return a;
  } else if (s >= static_cast<fltType>(1)) {
    return b;
  } else {
    return a + ((b - a) * s);
  }
}

template<typename numType>
[[nodiscard]] constexpr numType Sqr(numType x)
{
  return x * x;
}

template<typename intType>
[[nodiscard]] constexpr intType iPow(intType x, int y)
{
  if (y < 0) { return 0; }

  intType val = 1;
  while (y) {
    if (y & 1) { val *= x; }
    y >>= 1;
    x *= x;
  }

  return val;
}

template <typename numType>
[[nodiscard]] constexpr int Sgn(numType x)
{
  if constexpr (std::is_signed_v<numType>) {
    return int(x > 0) - int(x < 0);
  } else {
    return int(x > 0);
  }
}

// return type promoted to int for small types
[[nodiscard]] constexpr int Abs(signed char x) {
  return (x < 0) ? -int(x) : int(x); }
[[nodiscard]] constexpr int Abs(short x) {
  return (x < 0) ? -int(x) : int(x); }

// template for all other types
template<typename numType>
[[nodiscard]] constexpr numType Abs(numType x)
{
  if constexpr (std::is_signed_v<numType>) {
    // constexpr version of std::abs
    // NOTE: Abs(-MAX_INT) is undefined
    return (x < 0) ? -x : x;
  } else {
    return x;
  }
}
