//
// MathUtil.hh
// Copyright (C) 2025 Richard Bradley
//
// fun and useful numeric constants and
// various numeric functions needing a home
//

#pragma once
#include <type_traits>
#include <concepts>
#include <numbers>


// **** Concepts ****
template<class T>
concept NumType = std::integral<T> || std::floating_point<T>;


// **** Constants ****
namespace math {
  // various math constants
  template<std::floating_point T>
  constexpr T PI = std::numbers::pi_v<T>;

  template<std::floating_point T>
  constexpr T DEG_TO_RAD = PI<T> / T{180};

  template<std::floating_point T>
  constexpr T RAD_TO_DEG = T{180} / PI<T>;

  template<std::floating_point T>
  constexpr T VERY_SMALL = static_cast<T>(1.0e-12);
  template<>
  inline constexpr float VERY_SMALL<float> = 1.0e-7f;
}


// **** Functions ****
template<std::floating_point T>
[[nodiscard]] constexpr T DegToRad(T deg)
{
  return deg * math::DEG_TO_RAD<T>;
}

template<std::floating_point T>
[[nodiscard]] constexpr T RadToDeg(T rad)
{
  return rad * math::RAD_TO_DEG<T>;
}

template<NumType T>
[[nodiscard]] constexpr bool IsZero(T x)
{
  if constexpr (std::is_floating_point_v<T>) {
    return (x > -math::VERY_SMALL<T>) && (x < math::VERY_SMALL<T>);
  } else {
    return (x == 0);
  }
}

template<NumType T>
[[nodiscard]] constexpr bool IsOne(T x)
{
  if constexpr (std::is_floating_point_v<T>) {
    return (x > (T{1} - math::VERY_SMALL<T>))
      && (x < (T{1} + math::VERY_SMALL<T>));
  } else {
    return (x == 1);
  }
}

template<NumType T>
[[nodiscard]] constexpr bool IsPositive(T x)
{
  if constexpr (std::is_floating_point_v<T>) {
    return (x >= math::VERY_SMALL<T>);
  } else {
    return (x > 0);
  }
}

template<NumType T>
[[nodiscard]] constexpr bool IsNegative(T x)
{
  if constexpr (std::is_floating_point_v<T>) {
    return (x <= -math::VERY_SMALL<T>);
  } else {
    return (x < 0);
  }
}

template<NumType T>
[[nodiscard]] constexpr bool IsEqual(T x, T y)
{
  if constexpr (std::is_floating_point_v<T>) {
    return IsZero(x-y);
  } else {
    return (x == y);
  }
}

template<NumType T>
[[nodiscard]] constexpr bool IsLess(T x, T y)
{
  if constexpr (std::is_floating_point_v<T>) {
    return IsNegative(x-y);
  } else {
    return (x < y);
  }
}

template<NumType T>
[[nodiscard]] constexpr bool IsGreater(T x, T y)
{
  if constexpr (std::is_floating_point_v<T>) {
    return IsPositive(x-y);
  } else {
    return (x > y);
  }
}

[[nodiscard]] constexpr bool IsPowerOf2(std::integral auto x)
{
  return (x & (x - 1)) == 0;
}

template<NumType T>
[[nodiscard]] constexpr T Sqr(T x)
{
  return x * x;
}

template<NumType T>
[[nodiscard]] constexpr T iPow(T x, int y)
{
  if (y < 0) { return 0; }

  auto val = T{1};
  while (y) {
    if (y & 1) { val *= x; }
    y >>= 1;
    x *= x;
  }

  return val;
}

template<NumType T>
[[nodiscard]] constexpr int Sgn(T x)
{
  if constexpr (std::is_signed_v<T>) {
    return int{IsPositive(x)} - int{IsNegative(x)};
  } else {
    return int{IsPositive(x)};
  }
}

// return type promoted to int for small types
[[nodiscard]] constexpr int Abs(signed char x) {
  return (x < 0) ? -int{x} : int{x}; }
[[nodiscard]] constexpr int Abs(short x) {
  return (x < 0) ? -int{x} : int{x}; }

// template for all other types
template<NumType T>
[[nodiscard]] constexpr T Abs(T x)
{
  if constexpr (std::is_signed_v<T>) {
    // constexpr version of std::abs
    // NOTE: Abs(-MAX_INT) is undefined
    return (x < 0) ? -x : x;
  } else {
    return x;
  }
}
