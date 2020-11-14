//
// Color.hh - revision 21 (2019/1/4)
// Copyright(C) 2019 by Richard Bradley
//
// Standard color class library
//

#ifndef Color_hh
#define Color_hh

#include "Types.hh"
#include <iosfwd>


// **** Types ****
class Color
{
 public:
  enum {
    WAVES_MAX     = 3,
    WAVES_DEFAULT = 3
  };

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
  bool isBlack(Flt v = 1.0e-6) const;

  int waves() const { return _waves; }

  // Operators
  Color& operator+=(const Color& c);
  Color& operator-=(const Color& c);
  Color& operator*=(const Color& c);

  Color& operator*=(Flt s);
  Color& operator/=(Flt s);

  Flt&       operator[](int i)       { return _val[i]; }
  const Flt& operator[](int i) const { return _val[i]; }

  // Static Members
  static const Color black; // 0 for all values
  static const Color white; // 1 for all values

 private:
  Flt _val[WAVES_MAX];
  int _waves;
};


// **** Function ****
std::ostream& operator<<(std::ostream& stream, const Color& c);

inline Color operator*(Color& c, Flt s) {
  Color result(c.waves());
  for (int w = 0; w < c.waves(); ++w) { result[w] = c[w] * s; }
  return result;
}

inline Color operator/(Color& c, Flt s) {
  Color result(c.waves());
  for (int w = 0; w < c.waves(); ++w) { result[w] = c[w] / s; }
  return result;
}

void MultColor(const Color& a, const Color& b, Color& r);
Color MultColor(const Color& a, const Color& b);
  // multiply 2 colors together

#endif
