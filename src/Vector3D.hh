//
// Vector3D.hh
// Copyright (C) 2026 Richard Bradley
//
// vector template types/functions for 3D calculations
//
// ISSUES:
// - constexpr std::sqrt() is a non-standard extension only available in gcc
//

#pragma once
#include "MathUtil.hh"
#include "InitType.hh"
#include <ostream>
#include <cmath>


// **** Template Types ****
template<NumType T> class Vector2;
template<NumType T> class Vector3;
template<NumType T> class Vector4;


template<NumType T>
class Vector2
{
 public:
  union {
    T _val[2]; // only use [] operator for constexpr access
    struct { T x, y; };
  };

  using type = Vector2<T>;
  using value_type = T;
  using size_type = unsigned int;


  explicit Vector2(NoInit_t) { }
  constexpr Vector2() : _val{} { }
  constexpr Vector2(T vx, T vy) : _val{vx, vy} { }


  // Operators
  [[nodiscard]] constexpr T& operator[](size_type i) {
    return _val[i]; }
  [[nodiscard]] constexpr const T& operator[](size_type i) const {
    return _val[i]; }

  constexpr type& operator+=(const type& v) {
    _val[0] += v[0]; _val[1] += v[1]; return *this; }
  constexpr type& operator-=(const type& v) {
    _val[0] -= v[0]; _val[1] -= v[1]; return *this; }
  constexpr type& operator*=(T v) {
    _val[0] *= v; _val[1] *= v; return *this; }
  constexpr type& operator/=(T v) {
    _val[0] /= v; _val[1] /= v; return *this; }

  [[nodiscard]] constexpr bool operator==(const type& v) const = default;
  [[nodiscard]] constexpr type operator-() const {
    return {-_val[0], -_val[1]}; }


  // Iterators
  using iterator = T*;
  using const_iterator = const T*;

  [[nodiscard]] constexpr iterator begin() { return data(); }
  [[nodiscard]] constexpr const_iterator begin() const { return data(); }
  [[nodiscard]] constexpr const_iterator cbegin() const { return data(); }

  [[nodiscard]] constexpr iterator end() { return data() + size(); }
  [[nodiscard]] constexpr const_iterator end() const { return data() + size(); }
  [[nodiscard]] constexpr const_iterator cend() const { return data() + size(); }


  // Member Functions
  [[nodiscard]] static constexpr size_type size() { return 2; }
  [[nodiscard]] constexpr T* data() { return _val; }
  [[nodiscard]] constexpr const T* data() const { return _val; }

  [[nodiscard]] constexpr T lengthSqr() const {
    return Sqr(_val[0])+Sqr(_val[1]); }
  [[nodiscard]] T length() const { return std::sqrt(lengthSqr()); }
  [[nodiscard]] T lengthInv() const { return T{1} / length(); }
  [[nodiscard]] constexpr bool isUnit() const { return IsOne(lengthSqr()); }

  constexpr void set(T vx, T vy) { _val[0] = vx; _val[1] = vy; }
  constexpr void set(const T* v) { set(v[0], v[1]); }

  constexpr void normalize() { (*this) *= lengthInv(); }
};


template<NumType T>
class Vector3
{
 public:
  union {
    T _val[3]; // only use [] operator for constexpr access
    struct { T x, y, z; };
    struct { T r, g, b; };
  };

  using type = Vector3<T>;
  using value_type = T;
  using size_type = unsigned int;


  explicit Vector3(NoInit_t) { }
  constexpr Vector3() : _val{} { }
  constexpr Vector3(T vx, T vy, T vz) : _val{vx, vy, vz} { }
  constexpr Vector3(const Vector2<T>& v, T vz)
    : _val{v[0], v[1], vz} { }


  // Operators
  [[nodiscard]] constexpr T& operator[](size_type i) {
    return _val[i]; }
  [[nodiscard]] constexpr const T& operator[](size_type i) const {
    return _val[i]; }

  constexpr type& operator+=(const type& v) {
    _val[0] += v[0]; _val[1] += v[1]; _val[2] += v[2]; return *this; }
  constexpr type& operator-=(const type& v) {
    _val[0] -= v[0]; _val[1] -= v[1]; _val[2] -= v[2]; return *this; }
  constexpr type& operator*=(T v) {
    _val[0] *= v; _val[1] *= v; _val[2] *= v; return *this; }
  constexpr type& operator/=(T v) {
    _val[0] /= v; _val[1] /= v; _val[2] /= v; return *this; }

  [[nodiscard]] constexpr bool operator==(const type& v) const = default;
  [[nodiscard]] constexpr type operator-() const {
    return {-_val[0], -_val[1], -_val[2]}; }


  // Iterators
  using iterator = T*;
  using const_iterator = const T*;

  [[nodiscard]] constexpr iterator begin() { return data(); }
  [[nodiscard]] constexpr const_iterator begin() const { return data(); }
  [[nodiscard]] constexpr const_iterator cbegin() const { return data(); }

  [[nodiscard]] constexpr iterator end() { return data() + size(); }
  [[nodiscard]] constexpr const_iterator end() const { return data() + size(); }
  [[nodiscard]] constexpr const_iterator cend() const { return data() + size(); }


  // Member Functions
  [[nodiscard]] static constexpr size_type size() { return 3; }
  [[nodiscard]] constexpr T* data() { return _val; }
  [[nodiscard]] constexpr const T* data() const { return _val; }

  [[nodiscard]] constexpr T lengthSqr() const {
    return Sqr(_val[0]) + Sqr(_val[1]) + Sqr(_val[2]); }
  [[nodiscard]] T length() const { return std::sqrt(lengthSqr()); }
  [[nodiscard]] T lengthInv() const { return T{1} / length(); }
  [[nodiscard]] constexpr bool isUnit() const { return IsOne(lengthSqr()); }

  constexpr void set(T vx, T vy, T vz) {
    _val[0] = vx; _val[1] = vy; _val[2] = vz; }
  constexpr void set(const T* v) { set(v[0], v[1], v[2]); }
  constexpr void set(const Vector2<T>& v, T vz) { set(v[0], v[1], vz); }

  constexpr void normalize() { (*this) *= lengthInv(); }
};


template<NumType T>
class Vector4
{
 public:
  union {
    T _val[4]; // only use [] operator for constexpr access
    struct { T x, y, z, w; };
    struct { T r, g, b, a; };
  };

  using type = Vector4<T>;
  using value_type = T;
  using size_type = unsigned int;


  explicit Vector4(NoInit_t) { }
  constexpr Vector4() : _val{} { }
  constexpr Vector4(T vx, T vy, T vz, T vw) : _val{vx, vy, vz, vw} { }
  constexpr Vector4(const Vector3<T>& v, T vw)
    : _val{v[0], v[1], v[2], vw} { }


  // Operators
  [[nodiscard]] constexpr T& operator[](size_type i) {
    return _val[i]; }
  [[nodiscard]] constexpr const T& operator[](size_type i) const {
    return _val[i]; }

  constexpr type& operator+=(const type& v) {
    _val[0] += v[0]; _val[1] += v[1];
    _val[2] += v[2]; _val[3] += v[3]; return *this;
  }
  constexpr type& operator-=(const type& v) {
    _val[0] -= v[0]; _val[1] -= v[1];
    _val[2] -= v[2]; _val[3] -= v[3]; return *this;
  }
  constexpr type& operator*=(T v) {
    _val[0] *= v; _val[1] *= v; _val[2] *= v; _val[3] *= v; return *this; }
  constexpr type& operator/=(T v) {
    _val[0] /= v; _val[1] /= v; _val[2] /= v; _val[3] /= v; return *this; }

  [[nodiscard]] constexpr bool operator==(const type& v) const = default;
  [[nodiscard]] constexpr type operator-() const {
    return {-_val[0], -_val[1], -_val[2], -_val[3]}; }


  // Iterators
  using iterator = T*;
  using const_iterator = const T*;

  [[nodiscard]] constexpr iterator begin() { return data(); }
  [[nodiscard]] constexpr const_iterator begin() const { return data(); }
  [[nodiscard]] constexpr const_iterator cbegin() const { return data(); }

  [[nodiscard]] constexpr iterator end() { return data() + size(); }
  [[nodiscard]] constexpr const_iterator end() const { return data() + size(); }
  [[nodiscard]] constexpr const_iterator cend() const { return data() + size(); }


  // Member Functions
  [[nodiscard]] static constexpr size_type size() { return 4; }
  [[nodiscard]] constexpr T* data() { return _val; }
  [[nodiscard]] constexpr const T* data() const { return _val; }

  constexpr void set(T vx, T vy, T vz, T vw) {
    _val[0] = vx; _val[1] = vy; _val[2] = vz; _val[3] = vw; }
  constexpr void set(const T* v) { set(v[0], v[1], v[2], v[3]); }
  constexpr void set(const Vector3<T>& v, T vw) { set(v[0], v[1], v[2], vw); }
};


// **** Binary Operators ****
// vec + vec
template<NumType T>
[[nodiscard]] constexpr Vector2<T> operator+(
  const Vector2<T>& a, const Vector2<T>& b)
{
  return {a[0] + b[0], a[1] + b[1]};
}

template<NumType T>
[[nodiscard]] constexpr Vector3<T> operator+(
  const Vector3<T>& a, const Vector3<T>& b)
{
  return {a[0] + b[0], a[1] + b[1], a[2] + b[2]};
}

template<NumType T>
[[nodiscard]] constexpr Vector4<T> operator+(
  const Vector4<T>& a, const Vector4<T>& b)
{
  return {a[0] + b[0], a[1] + b[1], a[2] + b[2], a[3] + b[3]};
}

// vec - vec
template<NumType T>
[[nodiscard]] constexpr Vector2<T> operator-(
  const Vector2<T>& a, const Vector2<T>& b)
{
  return {a[0] - b[0], a[1] - b[1]};
}

template<NumType T>
[[nodiscard]] constexpr Vector3<T> operator-(
  const Vector3<T>& a, const Vector3<T>& b)
{
  return {a[0] - b[0], a[1] - b[1], a[2] - b[2]};
}

template<NumType T>
[[nodiscard]] constexpr Vector4<T> operator-(
  const Vector4<T>& a, const Vector4<T>& b)
{
  return {a[0] - b[0], a[1] - b[1], a[2] - b[2], a[3] - b[3]};
}

// vec * vec
template<NumType T>
[[nodiscard]] constexpr Vector2<T> operator*(
  const Vector2<T>& a, const Vector2<T>& b)
{
  return {a[0] * b[0], a[1] * b[1]};
}

template<NumType T>
[[nodiscard]] constexpr Vector3<T> operator*(
  const Vector3<T>& a, const Vector3<T>& b)
{
  return {a[0] * b[0], a[1] * b[1], a[2] * b[2]};
}

template<NumType T>
[[nodiscard]] constexpr Vector4<T> operator*(
  const Vector4<T>& a, const Vector4<T>& b)
{
  return {a[0] * b[0], a[1] * b[1], a[2] * b[2], a[3] * b[3]};
}

// vec / vec
template<NumType T>
[[nodiscard]] constexpr Vector2<T> operator/(
  const Vector2<T>& a, const Vector2<T>& b)
{
  return {a[0] / b[0], a[1] / b[1]};
}

template<NumType T>
[[nodiscard]] constexpr Vector3<T> operator/(
  const Vector3<T>& a, const Vector3<T>& b)
{
  return {a[0] / b[0], a[1] / b[1], a[2] / b[2]};
}

template<NumType T>
[[nodiscard]] constexpr Vector4<T> operator/(
  const Vector4<T>& a, const Vector4<T>& b)
{
  return {a[0] / b[0], a[1] / b[1], a[2] / b[2], a[3] / b[3]};
}

// vec * x
template<NumType T>
[[nodiscard]] constexpr Vector2<T> operator*(const Vector2<T>& a, T b)
{
  return {a[0] * b, a[1] * b};
}

template<NumType T>
[[nodiscard]] constexpr Vector3<T> operator*(const Vector3<T>& a, T b)
{
  return {a[0] * b, a[1] * b, a[2] * b};
}

template<NumType T>
[[nodiscard]] constexpr Vector4<T> operator*(const Vector4<T>& a, T b)
{
  return {a[0] * b, a[1] * b, a[2] * b, a[3] * b};
}

// x * vec
template<NumType T>
[[nodiscard]] constexpr Vector2<T> operator*(T a, const Vector2<T>& b)
{
  return {a * b[0], a * b[1]};
}

template<NumType T>
[[nodiscard]] constexpr Vector3<T> operator*(T a, const Vector3<T>& b)
{
  return {a * b[0], a * b[1], a * b[2]};
}

template<NumType T>
[[nodiscard]] constexpr Vector4<T> operator*(T a, const Vector4<T>& b)
{
  return {a * b[0], a * b[1], a * b[2], a * b[3]};
}

// vec / x
template<NumType T>
[[nodiscard]] constexpr Vector2<T> operator/(const Vector2<T>& a, T b)
{
  return {a[0] / b, a[1] / b};
}

template<NumType T>
[[nodiscard]] constexpr Vector3<T> operator/(const Vector3<T>& a, T b)
{
  return {a[0] / b, a[1] / b, a[2] / b};
}

template<NumType T>
[[nodiscard]] constexpr Vector4<T> operator/(const Vector4<T>& a, T b)
{
  return {a[0] / b, a[1] / b, a[2] / b, a[3] / b};
}

// x / vec
template<NumType T>
[[nodiscard]] constexpr Vector2<T> operator/(T a, const Vector2<T>& b)
{
  return {a / b[0], a / b[1]};
}

template<NumType T>
[[nodiscard]] constexpr Vector3<T> operator/(T a, const Vector3<T>& b)
{
  return {a / b[0], a / b[1], a / b[2]};
}

template<NumType T>
[[nodiscard]] constexpr Vector4<T> operator/(T a, const Vector4<T>& b)
{
  return {a / b[0], a / b[1], a / b[2], a / b[3]};
}


// **** Template Stream Operators ****
template<NumType T>
std::ostream& operator<<(std::ostream& os, const Vector2<T>& v)
{
  return os << '[' << v.x << ' ' << v.y << ']';
}

template<NumType T>
std::ostream& operator<<(std::ostream& os, const Vector3<T>& v)
{
  return os << '[' << v.x << ' ' << v.y << ' ' << v.z << ']';
}

template<NumType T>
std::ostream& operator<<(std::ostream& os, const Vector4<T>& v)
{
  return os << '[' << v.x << ' ' << v.y << ' ' << v.z << ' ' << v.w << ']';
}


// **** Template Functions ****
template<NumType T>
[[nodiscard]] constexpr T pointDistanceSqr(
  const Vector2<T>& a, const Vector2<T>& b)
{
  return (a - b).lengthSqr();
}

template<NumType T>
[[nodiscard]] constexpr T pointDistanceSqr(
  const Vector3<T>& a, const Vector3<T>& b)
{
  return (a - b).lengthSqr();
}

template<NumType T>
[[nodiscard]] T pointDistance(const Vector2<T>& a, const Vector2<T>& b)
{
  return (a - b).length();
}

template<NumType T>
[[nodiscard]] T pointDistance(const Vector3<T>& a, const Vector3<T>& b)
{
  return (a - b).length();
}

template<NumType T>
[[nodiscard]] constexpr T dotProduct(const Vector2<T>& a, const Vector2<T>& b)
{
  return (a[0] * b[0]) + (a[1] * b[1]);
}

template<NumType T>
[[nodiscard]] constexpr T dotProduct(const Vector3<T>& a, const Vector3<T>& b)
{
  return (a[0] * b[0]) + (a[1] * b[1]) + (a[2] * b[2]);
}

template<NumType T>
[[nodiscard]] constexpr Vector3<T> crossProduct(
  const Vector3<T>& a, const Vector3<T>& b)
{
  return {(a[1] * b[2]) - (a[2] * b[1]),
          (a[2] * b[0]) - (a[0] * b[2]),
          (a[0] * b[1]) - (a[1] * b[0])};
}

template<NumType T>
[[nodiscard]] Vector2<T> unitVec(const Vector2<T>& v)
{
  return v * v.lengthInv();
}

template<NumType T>
[[nodiscard]] Vector3<T> unitVec(const Vector3<T>& v)
{
  return v * v.lengthInv();
}

template<NumType T>
[[nodiscard]] Vector2<T> unitVec(T x, T y)
{
  const T inv = T{1} / std::sqrt(Sqr(x) + Sqr(y));
  return {x * inv, y * inv};
}

template<NumType T>
[[nodiscard]] Vector3<T> unitVec(T x, T y, T z)
{
  const T inv = T{1} / std::sqrt(Sqr(x) + Sqr(y) + Sqr(z));
  return {x * inv, y * inv, z * inv};
}
