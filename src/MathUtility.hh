//
// MathUtility.hh
// Copyright (C) 2023 Richard Bradley
//
// fun and useful numeric constants and
// various numeric functions needing a home
//

#pragma once
#include <type_traits>
#include <concepts>
#include <numbers>


// **** Constants ****
namespace math {
  // various math constants
  template<std::floating_point T>
  inline constexpr T PI = std::numbers::pi_v<T>;

  template<std::floating_point T>
  inline constexpr T DEG_TO_RAD = PI<T> / T{180};

  template<std::floating_point T>
  inline constexpr T RAD_TO_DEG = T{180} / PI<T>;

  template<std::floating_point T>
  inline constexpr T VERY_SMALL = static_cast<T>(1.0e-12);
  template<>
  inline constexpr float VERY_SMALL<float> = 1.0e-7f;
}


// **** Functions ****
[[nodiscard]] constexpr auto DegToRad(std::floating_point auto deg)
{
  return deg * math::DEG_TO_RAD<decltype(deg)>;
}

[[nodiscard]] constexpr auto RadToDeg(std::floating_point auto rad)
{
  return rad * math::RAD_TO_DEG<decltype(rad)>;
}

template<class numType>
[[nodiscard]] constexpr bool IsZero(numType x)
{
  if constexpr (std::is_floating_point_v<numType>) {
    return (x > -math::VERY_SMALL<numType>) && (x < math::VERY_SMALL<numType>);
  } else {
    return (x == 0);
  }
}

template<class numType>
[[nodiscard]] constexpr bool IsOne(numType x)
{
  if constexpr (std::is_floating_point_v<numType>) {
    return (x > (numType{1} - math::VERY_SMALL<numType>))
      && (x < (numType{1} + math::VERY_SMALL<numType>));
  } else {
    return (x == 1);
  }
}

template<class numType>
[[nodiscard]] constexpr bool IsPositive(numType x)
{
  if constexpr (std::is_floating_point_v<numType>) {
    return (x >= math::VERY_SMALL<numType>);
  } else {
    return (x > 0);
  }
}

template<class numType>
[[nodiscard]] constexpr bool IsNegative(numType x)
{
  if constexpr (std::is_floating_point_v<numType>) {
    return (x <= -math::VERY_SMALL<numType>);
  } else {
    return (x < 0);
  }
}

template<class numType>
[[nodiscard]] constexpr bool IsEqual(numType x, numType y)
{
  if constexpr (std::is_floating_point_v<numType>) {
    return IsZero(x-y);
  } else {
    return (x == y);
  }
}

template<class numType>
[[nodiscard]] constexpr bool IsLess(numType x, numType y)
{
  if constexpr (std::is_floating_point_v<numType>) {
    return IsNegative(x-y);
  } else {
    return (x < y);
  }
}

template<class numType>
[[nodiscard]] constexpr bool IsGreater(numType x, numType y)
{
  if constexpr (std::is_floating_point_v<numType>) {
    return IsPositive(x-y);
  } else {
    return (x > y);
  }
}

[[nodiscard]] constexpr bool IsPowerOf2(std::integral auto x)
{
  return (x & (x - 1)) == 0;
}

[[nodiscard]] constexpr auto Sqr(auto x)
{
  return x * x;
}

template<class numType>
[[nodiscard]] constexpr numType iPow(numType x, int y)
{
  if (y < 0) { return 0; }

  auto val = numType{1};
  while (y) {
    if (y & 1) { val *= x; }
    y >>= 1;
    x *= x;
  }

  return val;
}

template<class numType>
[[nodiscard]] constexpr int Sgn(numType x)
{
  if constexpr (std::is_signed_v<numType>) {
    return int{x > 0} - int{x < 0};
  } else {
    return int{x > 0};
  }
}

// return type promoted to int for small types
[[nodiscard]] constexpr int Abs(signed char x) {
  return (x < 0) ? -int{x} : int{x}; }
[[nodiscard]] constexpr int Abs(short x) {
  return (x < 0) ? -int{x} : int{x}; }

// template for all other types
template<class numType>
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
