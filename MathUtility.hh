//
// MathUtility.hh - revision 7 (2019/3/8)
// Copyright(C) 2019 by Richard Bradley
//
// fun and useful numeric constants and
// various numeric functions needing a home
//

#ifndef MathUtility_hh
#define MathUtility_hh

#include <cmath>


// **** Constants ****
template<typename fltType>
struct MathVal
{
  static constexpr fltType PI = M_PI;
  static constexpr fltType PI_2 = M_PI_2;  // pi/2
  static constexpr fltType PI_4 = M_PI_4;  // pi/4
  static constexpr fltType DEG_TO_RAD = M_PI / 180.0;
  static constexpr fltType RAD_TO_DEG = 180.0 / M_PI;
  static constexpr fltType VERY_SMALL = static_cast<fltType>(1.0e-7);
};


// **** Functions ****
template<typename fltType>
constexpr fltType DegToRad(const fltType& deg)
{
  return deg * MathVal<fltType>::DEG_TO_RAD;
}

template<typename fltType>
constexpr fltType RadToDeg(const fltType& rad)
{
  return rad * MathVal<fltType>::RAD_TO_DEG;
}

template<typename fltType>
constexpr bool IsZero(const fltType& x)
{
  return (x > -MathVal<fltType>::VERY_SMALL)
    && (x < MathVal<fltType>::VERY_SMALL);
}

template<typename fltType>
constexpr bool IsOne(const fltType& x)
{
  return (x > (static_cast<fltType>(1.0) - MathVal<fltType>::VERY_SMALL))
    && (x < (static_cast<fltType>(1.0) + MathVal<fltType>::VERY_SMALL));
}

template<typename fltType>
constexpr bool IsPositive(const fltType& x)
{
  return (x >= MathVal<fltType>::VERY_SMALL);
}

template<typename fltType>
constexpr bool IsNegative(const fltType& x)
{
  return (x <= -MathVal<fltType>::VERY_SMALL);
}

template<typename fltType>
constexpr bool IsEqual(const fltType& x, const fltType& y)
{
  return IsZero(x-y);
}

template<typename fltType>
constexpr bool IsLess(const fltType& x, const fltType& y)
{
  return IsNegative(x-y);
}

template<typename fltType>
constexpr bool IsGreater(const fltType& x, const fltType& y)
{
  return IsPositive(x-y);
}

template<typename numType>
constexpr numType Clamp(
  const numType& x, const numType& low, const numType& high)
{
  // assume high >= low
  // use std::clamp() instead for C++17
  return (x < low) ? low : ((high < x) ? high : x);
}

template<typename intType>
constexpr bool IsPowerOf2(const intType& x)
{
  return (x & (x - 1)) == 0;
}

template<typename numType, typename fltType>
constexpr numType Lerp(const numType& a, const numType& b, const fltType& s)
{
  if (s <= 0) {
    return a;
  } else if (s >= static_cast<fltType>(1.0)) {
    return b;
  } else {
    return a + ((b - a) * s);
  }
}

template<typename numType>
constexpr numType Sqr(const numType& x)
{
  return x * x;
}

template<typename numType>
constexpr numType Pow3(const numType& x)
{
  return x * x * x;
}

template<typename numType>
constexpr numType Pow4(const numType& x)
{
  numType x2 = x * x; return x2 * x2;
}

template<typename numType>
constexpr numType Pow5(const numType& x)
{
  numType x2 = x * x; return x2 * x2 * x;
}

template<typename numType>
constexpr numType Pow6(const numType& x)
{
  numType x2 = x * x; return x2 * x2 * x2;
}

template<typename numType>
constexpr numType Pow7(const numType& x)
{
  numType x2 = x * x; return x2 * x2 * x2 * x;
}

template<typename numType>
constexpr numType Pow8(const numType& x)
{
  numType x2 = x * x, x4 = x2 * x2; return x4 * x4;
}

template<typename numType>
constexpr numType Pow9(const numType& x)
{
  numType x2 = x * x, x4 = x2 * x2; return x4 * x4 * x;
}

template<typename intType>
constexpr intType iPow(intType x, int y)
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
constexpr int Sgn(const numType& x)
{
  return (x < 0) ? -1 : ((x > 0) ? 1 : 0);
}

#endif
