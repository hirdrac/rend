//
// Color.hh
// Copyright (C) 2021 Richard Bradley
//
// Standard color class library
//

#pragma once
#include <ostream>


// **** Types ****
class Color
{
 public:
  // Constants
  static constexpr int WAVES_MAX = 3;
  static constexpr int WAVES_DEFAULT = 3;

  using value_type = float;

  Color() = default;
  Color(int w) : _waves{w} { }
  constexpr Color(value_type r, value_type g, value_type b)
    : _val{r,g,b}, _waves{3} { }

  // Member Functions
  constexpr void clear();
  constexpr void full();

  int setRGB(value_type r, value_type g, value_type b);
  int getRGB(value_type& r, value_type& g, value_type& b) const;
  [[nodiscard]] constexpr bool isBlack(value_type v) const;

  [[nodiscard]] constexpr int waves() const { return _waves; }

  // Operators
  constexpr Color& operator+=(const Color& c);
  constexpr Color& operator-=(const Color& c);
  constexpr Color& operator*=(const Color& c);

  template <class T>
  constexpr Color& operator*=(T s);

  template <class T>
  constexpr Color& operator/=(T s);

  [[nodiscard]] constexpr value_type& operator[](int i) {
    return _val[i]; }
  [[nodiscard]] constexpr value_type operator[](int i) const {
    return _val[i]; }

  // Static Members
  static const Color black; // 0 for all values
  static const Color white; // 1 for all values

 private:
  value_type _val[WAVES_MAX];
  int _waves = WAVES_DEFAULT;
};


// **** Inline implementation ****
constexpr void Color::clear()
{
  for (int i = 0; i < _waves; ++i) { _val[i] = {}; }
}

constexpr void Color::full()
{
  for (int i = 0; i < _waves; ++i) { _val[i] = static_cast<value_type>(1); }
}

constexpr bool Color::isBlack(Color::value_type v) const
{
  for (int i = 0; i < _waves; ++i) {
    if (_val[i] >= v) { return false; }
  }

  return true;
}

constexpr Color& Color::operator+=(const Color& c)
{
  for (int i = 0; i < _waves; ++i) { _val[i] += c._val[i]; }
  return *this;
}

constexpr Color& Color::operator-=(const Color& c)
{
  for (int i = 0; i < _waves; ++i) { _val[i] -= c._val[i]; }
  return *this;
}

constexpr Color& Color::operator*=(const Color& c)
{
  for (int i = 0; i < _waves; ++i) { _val[i] *= c._val[i]; }
  return *this;
}

template <class T>
constexpr Color& Color::operator*=(T s)
{
  const auto v = static_cast<value_type>(s);
  for (int i = 0; i < _waves; ++i) { _val[i] *= v; }
  return *this;
}

template <class T>
constexpr Color& Color::operator/=(T s)
{
  const auto v = static_cast<value_type>(s);
  for (int i = 0; i < _waves; ++i) { _val[i] /= v; }
  return *this;
}


// **** Non-member operators/functions ****
std::ostream& operator<<(std::ostream& os, const Color& c);

template <class T>
[[nodiscard]] constexpr Color operator*(Color& c, T s) {
  Color result{c};
  return result *= s;
}

template <class T>
[[nodiscard]] constexpr Color operator/(Color& c, T s) {
  Color result{c};
  return result /= s;
}

inline void MultColor(const Color& a, const Color& b, Color& result) {
  for (int i = 0; i < result.waves(); ++i) { result[i] = a[i] * b[i]; }
}

[[nodiscard]] inline Color MultColor(const Color& a, const Color& b) {
  Color result(a.waves());
  for (int i = 0; i < a.waves(); ++i) { result[i] = a[i] * b[i]; }
  return result;
}
