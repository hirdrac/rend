//
// Color.hh
// Copyright (C) 2022 Richard Bradley
//
// Standard color class library
//

#pragma once
#include <ostream>


// **** Types ****
class Color
{
 public:
  using value_type = float;
  using size_type = unsigned int;

  // Constants
  static constexpr size_type CHANNELS = 3;
  static constexpr size_type RED_CHANNEL = 0;
  static constexpr size_type GREEN_CHANNEL = 1;
  static constexpr size_type BLUE_CHANNEL = 2;


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

  [[nodiscard]] constexpr value_type& operator[](size_type i) {
    return _val[i]; }
  [[nodiscard]] constexpr value_type operator[](size_type i) const {
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
  value_type _val[4];  // 4 performs better than 3
};

namespace colors {
  constexpr Color black{0,0,0};
  constexpr Color white{1,1,1};
}


// **** Inline Implementation ****
constexpr bool Color::isBlack(value_type min) const
{
  return !((_val[0] > min) || (_val[1] > min) || (_val[2] > min));
}

constexpr Color& Color::operator+=(const Color& c)
{
  _val[0] += c._val[0];
  _val[1] += c._val[1];
  _val[2] += c._val[2];
  return *this;
}

constexpr Color& Color::operator-=(const Color& c)
{
  _val[0] -= c._val[0];
  _val[1] -= c._val[1];
  _val[2] -= c._val[2];
  return *this;
}

constexpr Color& Color::operator*=(const Color& c)
{
  _val[0] *= c._val[0];
  _val[1] *= c._val[1];
  _val[2] *= c._val[2];
  return *this;
}

template <class T>
constexpr Color& Color::operator*=(T s)
{
  const auto x = static_cast<value_type>(s);
  _val[0] *= x;
  _val[1] *= x;
  _val[2] *= x;
  return *this;
}

template <class T>
constexpr Color& Color::operator/=(T s)
{
  const auto x = static_cast<value_type>(s);
  _val[0] /= x;
  _val[1] /= x;
  _val[2] /= x;
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
