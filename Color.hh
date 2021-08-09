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
  static constexpr int CHANNELS = 3;
  static constexpr int RED_CHANNEL = 0;
  static constexpr int GREEN_CHANNEL = 1;
  static constexpr int BLUE_CHANNEL = 2;

  using value_type = float;

  Color() = default;
  constexpr Color(value_type r, value_type g, value_type b) : _val{r,g,b} { }

  // Member Functions
  [[nodiscard]] constexpr bool isBlack(value_type v) const;

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

  [[nodiscard]] constexpr value_type red() const {
    return _val[RED_CHANNEL]; }
  [[nodiscard]] constexpr value_type green() const {
    return _val[GREEN_CHANNEL]; }
  [[nodiscard]] constexpr value_type blue() const {
    return _val[BLUE_CHANNEL]; }

  [[nodiscard]] constexpr value_type grayValue() const {
    return (red() * static_cast<value_type>(.299))
      + (green() * static_cast<value_type>(.587))
      + (blue() * static_cast<value_type>(.114));
  }

  [[nodiscard]] constexpr value_type cieLuminance() const {
    return (red() * static_cast<value_type>(.2126))
      + (green() * static_cast<value_type>(.7152))
      + (blue() * static_cast<value_type>(.0722));
  }

 private:
  value_type _val[CHANNELS];
};

namespace colors {
  constexpr Color black{0,0,0};
  constexpr Color white{1,1,1};
}


// **** Inline implementation ****
constexpr bool Color::isBlack(Color::value_type min) const
{
  for (auto v : _val) { if (v >= min) { return false; } }
  return true;
}

constexpr Color& Color::operator+=(const Color& c)
{
  for (int i = 0; i < CHANNELS; ++i) { _val[i] += c._val[i]; }
  return *this;
}

constexpr Color& Color::operator-=(const Color& c)
{
  for (int i = 0; i < CHANNELS; ++i) { _val[i] -= c._val[i]; }
  return *this;
}

constexpr Color& Color::operator*=(const Color& c)
{
  for (int i = 0; i < CHANNELS; ++i) { _val[i] *= c._val[i]; }
  return *this;
}

template <class T>
constexpr Color& Color::operator*=(T s)
{
  const auto x = static_cast<value_type>(s);
  for (auto& v : _val) { v *= x; }
  return *this;
}

template <class T>
constexpr Color& Color::operator/=(T s)
{
  const auto x = static_cast<value_type>(s);
  for (auto& v : _val) { v /= x; }
  return *this;
}


// **** Non-member operators/functions ****
inline std::ostream& operator<<(std::ostream& os, const Color& c) {
  return os << '[' << c[0] << ' ' << c[1] << ' ' << c[2] << ']';
}

template <class T>
[[nodiscard]] constexpr Color operator*(const Color& c, T s) {
  return Color{c} *= s;
}

template <class T>
[[nodiscard]] constexpr Color operator/(const Color& c, T s) {
  return Color{c} /= s;
}

[[nodiscard]] constexpr Color operator*(const Color& a, const Color& b) {
  return Color{a[0] * b[0], a[1] * b[1], a[2] * b[2]};
}
