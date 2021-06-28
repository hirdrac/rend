//
// Roots.cc
// Copyright (C) 2021 Richard Bradley
//
// Implementation of polynomial root finding functions
// Most of SolveQuadric(), SolveCubic(), SolveQuartic() taken from code
// by Jochen Schwarze (Graphics Gems I)
//

#include "Roots.hh"
#include "MathUtility.hh"
#include <cmath>


/**** Inlined Functions ****/
constexpr Flt CBRT(Flt x)
{
  if (x > 0.0) {
    return std::pow(x, 1.0/3.0);
  } else if (x < 0.0) {
    return -std::pow(-x, 1.0/3.0);
  } else {
    return 0.0;
  }
}


/**** Functions ****/
int SolveQuadric(const Flt c[3], Flt s[2])
{
  // normal form: x^2 + px + q = 0
  Flt p = c[1] / (2 * c[2]);
  Flt q = c[0] / c[2];
  Flt d = Sqr(p) - q;

  if (IsNegative(d)) {
    // no solutions
    return 0;
  } else if (IsPositive(d)) {
    // two solutions
    Flt sqrt_d = std::sqrt(d);
    s[0] = -p + sqrt_d;
    s[1] = -p - sqrt_d;
    return 2;
  } else {
    // one solution
    s[0] = -p;
    return 1;
  }
}


int SolveCubic(const Flt c[4], Flt s[3])
{
  // normal form: x^3 + Ax^2 + Bx + C = 0
  Flt A = c[2] / c[3];
  Flt B = c[1] / c[3];
  Flt C = c[0] / c[3];

  // substitute x = y - A/3 to eliminate quadric term: x^3 +px + q = 0
  Flt sq_A = Sqr(A);
  Flt third_A = 1.0/3.0 * A;
  Flt p = 1.0/3.0 * (-1.0/3.0 * sq_A + B);
  Flt q = .5 * (2.0/27.0 * A * sq_A - third_A * B + C);

  // use Cardano's formula
  Flt cb_p = Sqr(p) * p;
  Flt D = Sqr(q) + cb_p;

  if (IsNegative(D)) {
    // three real solutions
    Flt phi = 1.0/3.0 * std::acos(-q / std::sqrt(-cb_p));
    Flt t = 2 * std::sqrt(-p);
    s[0] = (t * std::cos(phi)) - third_A;
    s[1] = (-t * std::cos(phi + (PI / 3))) - third_A;
    s[2] = (-t * std::cos(phi - (PI / 3))) - third_A;
    return 3;
  } else if (IsPositive(D)) {
    // one real solution
    Flt sqrt_D = std::sqrt(D);
    Flt u =  CBRT(sqrt_D - q);
    Flt v = -CBRT(sqrt_D + q);
    s[0] = (u + v) - third_A;
    return 1;
  } else if (IsZero(q)) {
    // one triple solution
    s[0] = -third_A;
    return 1;
  } else {
    // one single and one double solution
    Flt u = CBRT(-q);
    s[0] = (2 * u)- third_A;
    s[1] = -u - third_A;
    return 2;
  }
}


int SolveQuartic(const Flt c[5], Flt s[4])
{
  // normal form: x^4 + Ax^3 + Bx^2 + Cx + D = 0
  Flt A = c[3] / c[4];
  Flt B = c[2] / c[4];
  Flt C = c[1] / c[4];
  Flt D = c[0] / c[4];

  // substitute x = y - A/4 to eliminate cubic term: x^4 + px^2 + qx + r = 0
  Flt sq_A = Sqr(A);
  Flt qtr_A = .25 * A;
  Flt p = -.375 * sq_A + B;
  Flt q =  .125 * sq_A * A - .5 * A * B + C;
  Flt r = (-3.0/256.0) * sq_A * sq_A + (1.0/16.0) * sq_A * B - qtr_A * C + D;

  Flt coeffs[4];
  int num;

  if (IsZero(r)) {
    // no absolute term: y(y^3 + py + q) = 0
    coeffs[0] = q;
    coeffs[1] = p;
    coeffs[2] = 0;
    coeffs[3] = 1;
    num = SolveCubic(coeffs, s);
    s[num++] = 0;

  } else {
    // solve the resolvent cubic
    coeffs[0] = .5 * r * p - .125 * Sqr(q);
    coeffs[1] = -r;
    coeffs[2] = -.5 * p;
    coeffs[3] = 1;
    SolveCubic(coeffs, s);

    // take the one real solution to build two quadric equations
    Flt z = s[0];

    Flt u = Sqr(z) - r;
    if (IsNegative(u)) {
      return 0;
    } else if (IsPositive(u)) {
      u = std::sqrt(u);
    } else {
      u = 0;
    }

    Flt v = (2 * z) - p;
    if (IsNegative(v)) {
      return 0;
    } else if (IsPositive(v)) {
      v = std::sqrt(v);
    } else {
      v = 0;
    }

    coeffs[0] = z - u;
    coeffs[1] = (q < 0) ? -v : v;
    coeffs[2] = 1;
    num = SolveQuadric(coeffs, s);

    coeffs[0] = z + u;
    coeffs[1] = (q < 0) ? v : -v;
    coeffs[2] = 1;
    num += SolveQuadric(coeffs, s + num);
  }

  // resubstitute
  for (int i = 0; i < num; ++i) { s[i] -= qtr_A; }
  return num;
}
