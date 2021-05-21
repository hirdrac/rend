//
// Color.hh
// Copyright (C) 2021 Richard Bradley
//
// Standard color class library
//

#pragma once
#include "Types.hh"
#include <iosfwd>


// **** Types ****
class Color
{
 public:
  // Constants
  static constexpr int WAVES_MAX = 3;
  static constexpr int WAVES_DEFAULT = 3;

  // Constructors
  Color() : _waves(WAVES_DEFAULT) { }
  Color(int w) : _waves(w) { }
  Color(Flt r, Flt g, Flt b) : _waves(WAVES_DEFAULT) { setRGB(r, b, g); }

  //Color(int number, const Flt* values) : _waves(number) {
  //  for (int i = 0; i < _waves; ++i) { _val[i] = values[i]; }
  //}

  // Member Functions
  void clear();
  void full();

  int setRGB(Flt r, Flt g, Flt b);
  int setRGB(const Flt* array) {
    return setRGB(array[0], array[1], array[2]); }

  int getRGB(Flt& r, Flt& g, Flt& b) const;
  [[nodiscard]] bool isBlack(Flt v = 1.0e-6) const;

  [[nodiscard]] int waves() const { return _waves; }

  // Operators
  Color& operator+=(const Color& c);
  Color& operator-=(const Color& c);
  Color& operator*=(const Color& c);

  Color& operator*=(Flt s);
  Color& operator/=(Flt s);

  [[nodiscard]] Flt& operator[](int i)       { return _val[i]; }
  [[nodiscard]] Flt  operator[](int i) const { return _val[i]; }

  // Static Members
  static const Color black; // 0 for all values
  static const Color white; // 1 for all values

 private:
  Flt _val[WAVES_MAX];
  int _waves;
};


// **** Functions ****
std::ostream& operator<<(std::ostream& os, const Color& c);

[[nodiscard]] inline Color operator*(Color& c, Flt s) {
  Color result(c.waves());
  for (int w = 0; w < c.waves(); ++w) { result[w] = c[w] * s; }
  return result;
}

[[nodiscard]] inline Color operator/(Color& c, Flt s) {
  Color result(c.waves());
  for (int w = 0; w < c.waves(); ++w) { result[w] = c[w] / s; }
  return result;
}

// multiply 2 colors together
inline void MultColor(const Color& a, const Color& b, Color& result) {
  for (int i = 0; i < result.waves(); ++i) { result[i] = a[i] * b[i]; }
}

[[nodiscard]] inline Color MultColor(const Color& a, const Color& b) {
  Color result(a.waves());
  for (int i = 0; i < a.waves(); ++i) { result[i] = a[i] * b[i]; }
  return result;
}
