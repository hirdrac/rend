//
// Roots.cc
// Copyright (C) 2026 Richard Bradley
//
// solveQuadric(), solveCubic(), solveQuartic() based on code
// by Jochen Schwarze (Graphics Gems I)
//

#include "Roots.hh"
#include "MathUtil.hh"
#include <cmath>


// **** Inlined Functions ****
static inline Flt CBRT(Flt x)
{
  if (x > 0.0) {
    return std::pow(x, 1.0/3.0);
  } else if (x < 0.0) {
    return -std::pow(-x, 1.0/3.0);
  } else {
    return 0.0;
  }
}


// **** Functions ****
int solveQuadric(const Flt c[3], Flt s[2])
{
  // normal form: x^2 + px + q = 0
  const Flt p = c[1] / (2 * c[2]);
  const Flt q = c[0] / c[2];
  const Flt d = p*p - q;

  if (isPositive(d)) {
    // two solutions
    const Flt sqrt_d = std::sqrt(d);
    s[0] = -p - sqrt_d;
    s[1] = -p + sqrt_d;
    return 2;
  } else {
    // 1 or 0 solutions
    // ignore d == 0.0 case (1 solution) to simplify intersections
    return 0;
  }
}


int solveCubic(const Flt c[4], Flt s[3])
{
  // normal form: x^3 + Ax^2 + Bx + C = 0
  const Flt A = c[2] / c[3];
  const Flt B = c[1] / c[3];
  const Flt C = c[0] / c[3];

  // substitute x = y - A/3 to eliminate quadric term: x^3 +px + q = 0
  const Flt sq_A = A*A;
  const Flt third_A = A / 3.0;
  const Flt p = (-1.0/3.0 * sq_A + B) / 3.0;
  const Flt q = .5 * (2.0/27.0 * A * sq_A - third_A * B + C);

  // use Cardano's formula
  const Flt cb_p = p*p*p;
  const Flt D = q*q + cb_p;

  if (isNegative(D)) {
    // three real solutions
    const Flt phi = std::acos(-q / std::sqrt(-cb_p)) / 3.0;
    const Flt t = 2 * std::sqrt(-p);
    s[0] = (t * std::cos(phi)) - third_A;
    s[1] = (-t * std::cos(phi + (PI / 3))) - third_A;
    s[2] = (-t * std::cos(phi - (PI / 3))) - third_A;
    return 3;
  } else if (isPositive(D)) {
    // one real solution
    const Flt sqrt_D = std::sqrt(D);
    const Flt u =  CBRT(sqrt_D - q);
    const Flt v = -CBRT(sqrt_D + q);
    s[0] = (u + v) - third_A;
    return 1;
  } else if (isZero(q)) {
    // one triple solution
    s[0] = -third_A;
    return 1;
  } else {
    // one single and one double solution
    const Flt u = CBRT(-q);
    s[0] = (2 * u)- third_A;
    s[1] = -u - third_A;
    return 2;
  }
}


static int solveCubic1only(const Flt c[4], Flt* s)
{
  // normal form: x^3 + Ax^2 + Bx + C = 0
  const Flt A = c[2] / c[3];
  const Flt B = c[1] / c[3];
  const Flt C = c[0] / c[3];

  // substitute x = y - A/3 to eliminate quadric term: x^3 +px + q = 0
  const Flt sq_A = A*A;
  const Flt third_A = A / 3.0;
  const Flt p = (-1.0/3.0 * sq_A + B) / 3.0;
  const Flt q = .5 * (2.0/27.0 * A * sq_A - third_A * B + C);

  // use Cardano's formula
  const Flt cb_p = p*p*p;
  const Flt D = q*q + cb_p;

  if (isNegative(D)) {
    // three real solutions
    const Flt phi = std::acos(-q / std::sqrt(-cb_p)) / 3.0;
    const Flt t = 2 * std::sqrt(-p);
    s[0] = (t * std::cos(phi)) - third_A;
    //s[1] = (-t * std::cos(phi + (PI / 3))) - third_A;
    //s[2] = (-t * std::cos(phi - (PI / 3))) - third_A;
    return 1; // 3
  } else if (isPositive(D)) {
    // one real solution
    const Flt sqrt_D = std::sqrt(D);
    const Flt u =  CBRT(sqrt_D - q);
    const Flt v = -CBRT(sqrt_D + q);
    s[0] = (u + v) - third_A;
    return 1;
  } else if (isZero(q)) {
    // one triple solution
    s[0] = -third_A;
    return 1;
  } else {
    // one single and one double solution
    const Flt u = CBRT(-q);
    s[0] = (2 * u) - third_A;
    //s[1] = -u - third_A;
    return 1; // 2
  }
}


int solveQuartic(const Flt c[5], Flt s[4])
{
  // normal form: x^4 + Ax^3 + Bx^2 + Cx + D = 0
  const Flt A = c[3] / c[4];
  const Flt B = c[2] / c[4];
  const Flt C = c[1] / c[4];
  const Flt D = c[0] / c[4];

  // substitute x = y - A/4 to eliminate cubic term: x^4 + px^2 + qx + r = 0
  const Flt sq_A = A*A;
  const Flt qtr_A = .25 * A;
  const Flt p = -.375 * sq_A + B;
  const Flt q =  .125 * sq_A * A - .5 * A * B + C;
  const Flt r = (-3.0/256.0) * sq_A * sq_A
    + (1.0/16.0) * sq_A * B - qtr_A * C + D;

  Flt coeffs[4];
  int num;

  if (isZero(r)) {
    // no absolute term: y(y^3 + py + q) = 0
    coeffs[0] = q;
    coeffs[1] = p;
    coeffs[2] = 0;
    coeffs[3] = 1;
    num = solveCubic(coeffs, s);
    s[num++] = 0;

  } else {
    // solve the resolvent cubic
    coeffs[0] = .5 * r * p - .125 * q*q;
    coeffs[1] = -r;
    coeffs[2] = -.5 * p;
    coeffs[3] = 1;
    if (solveCubic1only(coeffs, s) == 0) { return 0; }

    // take the one real solution to build two quadric equations
    const Flt z = s[0];

    Flt u = z*z - r;
    if (isNegative(u)) { return 0; }

    Flt v = (2 * z) - p;
    if (isNegative(v)) { return 0; }

    if (isPositive(u)) { u = std::sqrt(u); } else { u = 0; }
    if (isPositive(v)) { v = std::sqrt(v); } else { v = 0; }

    coeffs[0] = z - u;
    coeffs[1] = (q < 0) ? -v : v;
    coeffs[2] = 1;
    num = solveQuadric(coeffs, s);

    coeffs[0] = z + u;
    coeffs[1] = (q < 0) ? v : -v;
    coeffs[2] = 1;
    num += solveQuadric(coeffs, s + num);
  }

  // resubstitute
  for (int i = 0; i < num; ++i) { s[i] -= qtr_A; }
  return num;
}
