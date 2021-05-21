//
// Color.cc
// Copyright (C) 2020 Richard Bradley
//
// Implementation of Color class library
//

#include "Color.hh"
#include <iostream>

// ADD - colors with more than 3 waves not handled
// BUG - operations between colors with different waves not handled properly


// **** Color Class ****
// Static Members
const Color Color::black(0,0,0);
const Color Color::white(1,1,1);

// Member functions
int Color::getRGB(Flt& r, Flt& g, Flt& b) const
{
  switch (_waves) {
    case 3:
      r = _val[0];
      g = _val[1];
      b = _val[2];
      break;

    case 1:
      r = _val[0];
      g = _val[0];
      b = _val[0];
      break;

    default:
      return -1;  // Error
  }

  return 0;
}

int Color::setRGB(Flt r, Flt g, Flt b)
{
  switch (_waves) {
    case 3:
      _val[0] = r;
      _val[1] = g;
      _val[2] = b;
      break;

    case 1:
      // CIE luminance from linear red, green, blue
      //_val[0] = (r * .2126) + (g * .7152) + (b * .0722);

      // nonlinear video luma from nonlinear red, green, blue
      _val[0] = (r * .299) + (g * .587) + (b * .114);
      break;

    default:
      return -1;
  }

  return 0;
}

void Color::clear()
{
  for (int i = 0; i < _waves; ++i) { _val[i] = 0.0; }
}

void Color::full()
{
  for (int i = 0; i < _waves; ++i) { _val[i] = 1.0; }
}

Color& Color::operator+=(const Color& c)
{
  for (int i = 0; i < _waves; ++i) { _val[i] += c._val[i]; }
  return *this;
}

Color& Color::operator-=(const Color& c)
{
  for (int i = 0; i < _waves; ++i) { _val[i] -= c._val[i]; }
  return *this;
}

Color& Color::operator*=(const Color& c)
{
  for (int i = 0; i < _waves; ++i) { _val[i] *= c._val[i]; }
  return *this;
}

Color& Color::operator*=(Flt s)
{
  for (int i = 0; i < _waves; ++i) { _val[i] *= s; }
  return *this;
}

Color& Color::operator/=(Flt s)
{
  for (int i = 0; i < _waves; ++i) { _val[i] /= s; }
  return *this;
}

bool Color::isBlack(Flt v) const
{
  for (int i = 0; i < _waves; ++i) {
    if (_val[i] >= v) { return false; }
  }

  return true;
}


// **** Functions ****
std::ostream& operator<<(std::ostream& os, const Color& c)
{
  os << '[' << c[0];
  for (int i = 1; i < c.waves(); ++i) { os << ' ' << c[i]; }
  return os << ']';
}
