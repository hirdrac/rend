//
// Color.cc
// Copyright (C) 2021 Richard Bradley
//
// Implementation of Color class library
//

#include "Color.hh"

// ADD - colors with more than 3 waves not handled
// BUG - operations between colors with different waves not handled properly


// **** Color Class ****
// Static Members
const Color Color::black(0,0,0);
const Color Color::white(1,1,1);

// Member functions
int Color::getRGB(
  Color::value_type& r, Color::value_type& g, Color::value_type& b) const
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

int Color::setRGB(Color::value_type r, Color::value_type g, Color::value_type b)
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
      _val[0] = (r * static_cast<value_type>(.299))
        + (g * static_cast<value_type>(.587))
        + (b * static_cast<value_type>(.114));
      break;

    default:
      return -1;
  }

  return 0;
}


// **** Functions ****
std::ostream& operator<<(std::ostream& os, const Color& c)
{
  os << '[' << c[0];
  for (int i = 1; i < c.waves(); ++i) { os << ' ' << c[i]; }
  return os << ']';
}
