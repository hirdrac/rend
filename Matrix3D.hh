//
// Matrix3D.hh
// Copyright (C) 2021 Richard Bradley
//
// 4x4 matrix template type/functions for 3D calculations
//

#pragma once
#include "Vector3D.hh"
#include "MathUtility.hh"
#include <ostream>


// **** Types ****
enum MatrixOrderType { ROW_MAJOR, COLUMN_MAJOR };
  // -- Memory Layout --
  // row major matrix | column major matrix
  //   [ 0  1  2  3]  |   [ 0  4  8 12]
  //   [ 4  5  6  7]  |   [ 1  5  9 13]
  //   [ 8  9 10 11]  |   [ 2  6 10 14]
  //   [12 13 14 15]  |   [ 3  7 11 15]

template<typename T, MatrixOrderType MOT>
class Matrix4x4
{
 public:
  using self_type = Matrix4x4<T,MOT>;
  using value_type = T;
  using size_type = unsigned int;


  // Constructors
  Matrix4x4() = default;
  constexpr Matrix4x4(T a, T b, T c, T d, T e, T f, T g, T h,
                      T i, T j, T k, T l, T m, T n, T o, T p) {
    _val[ 0] = a; _val[ 1] = b; _val[ 2] = c; _val[ 3] = d;
    _val[ 4] = e; _val[ 5] = f; _val[ 6] = g; _val[ 7] = h;
    _val[ 8] = i; _val[ 9] = j; _val[10] = k; _val[11] = l;
    _val[12] = m; _val[13] = n; _val[14] = o; _val[15] = p;
  }


  // Operators
  [[nodiscard]] constexpr T& operator[](size_type i) { return _val[i]; }
  [[nodiscard]] constexpr const T& operator[](size_type i) const { return _val[i]; }

  template<unsigned int N>
  constexpr self_type& operator=(const T (&vals)[N]) {
    // NOTE: required by clang for 'Matrix m = {...}'
    static_assert(N == size());
    for (int i = 0; i != size(); ++i) { _val[i] = vals[i]; }
    return *this;
  }

  constexpr self_type& operator=(const self_type&) = default;
    // NOTE: required by clang for operator*= below

  constexpr self_type& operator*=(const self_type& m) {
    return operator=(*this * m); }


  // Iterators
  using iterator = T*;
  using const_iterator = const T*;

  [[nodiscard]] constexpr iterator begin() noexcept { return data(); }
  [[nodiscard]] constexpr const_iterator begin() const noexcept { return data(); }
  [[nodiscard]] constexpr const_iterator cbegin() const noexcept { return data(); }

  [[nodiscard]] constexpr iterator end() noexcept { return data() + size(); }
  [[nodiscard]] constexpr const_iterator end() const noexcept { return data() + size(); }
  [[nodiscard]] constexpr const_iterator cend() const noexcept { return data() + size(); }


  // Member Functions
  [[nodiscard]] static constexpr size_type size() noexcept { return 16; }
  [[nodiscard]] constexpr T* data() noexcept { return _val; }
  [[nodiscard]] constexpr const T* data() const noexcept { return _val; }

  constexpr void setZero();
  constexpr void setIdentity();

  constexpr void setTranslation(T tx, T ty, T tz);
  constexpr void setTranslation(const Vector3<T>& v) {
    setTranslation(v.x, v.y, v.z); }
  constexpr void translate(T tx, T ty, T tz);
  constexpr void translate(const Vector3<T>& v) { translate(v.x, v.y, v.z); }

  inline void translateOptimized(T tx, T ty, T tz);
  void translateOptimized(const Vector3<T>& v) {
    translateOptimized(v.x, v.y, v.z); }

  void setRotationX(T rad) { setRotationX_sc(std::sin(rad), std::cos(rad)); }
  constexpr void setRotationX_sc(T sinVal, T cosVal);
  void rotateX(T rad) { rotateX_sc(std::sin(rad), std::cos(rad)); }
  constexpr void rotateX_sc(T sinVal, T cosVal);

  void setRotationY(T rad) { setRotationY_sc(std::sin(rad), std::cos(rad)); }
  constexpr void setRotationY_sc(T sinVal, T cosVal);
  void rotateY(T rad) { rotateY_sc(std::sin(rad), std::cos(rad)); }
  constexpr void rotateY_sc(T sinVal, T cosVal);

  void setRotationZ(T rad) { setRotationZ_sc(std::sin(rad), std::cos(rad)); }
  constexpr void setRotationZ_sc(T sinVal, T cosVal);
  void rotateZ(T rad) { rotateZ_sc(std::sin(rad), std::cos(rad)); }
  constexpr void rotateZ_sc(T sinVal, T cosVal);

  constexpr void setRotation(const Vector3<T>& axis, T rad) {
    setRotation_sc(axis, std::sin(rad), std::cos(rad)); }
  constexpr void setRotation_sc(const Vector3<T>& axis, T sinVal, T cosVal);
  void rotate(const Vector3<T>& axis, T rad) {
    rotate_sc(axis, std::sin(rad), std::cos(rad)); }
  constexpr void rotate_sc(const Vector3<T>& axis, T sinVal, T cosVal);

  constexpr void setScaling(T sx, T sy, T sz);
  constexpr void setScaling(const Vector3<T>& v) { setScaling(v.x, v.y, v.z); }
  constexpr void scale(T sx, T sy, T sz);
  constexpr void scale(const Vector3<T>& v) { setScaling(v.x, v.y, v.z); }

  constexpr void transpose();

 private:
  T _val[size()];
};


// **** Binary operators ****
template<typename T>
[[nodiscard]] constexpr Matrix4x4<T,ROW_MAJOR> operator*(
  const Matrix4x4<T,ROW_MAJOR>& a, const Matrix4x4<T,ROW_MAJOR>& b)
{
  Matrix4x4<T,ROW_MAJOR> m;
  //for (int i = 0; i != 16; i += 4) {
  //  m[i]   = (a[i]*b[0]) + (a[i+1]*b[4]) + (a[i+2]*b[8])  + (a[i+3]*b[12]);
  //  m[i+1] = (a[i]*b[1]) + (a[i+1]*b[5]) + (a[i+2]*b[9])  + (a[i+3]*b[13]);
  //  m[i+2] = (a[i]*b[2]) + (a[i+1]*b[6]) + (a[i+2]*b[10]) + (a[i+3]*b[14]);
  //  m[i+3] = (a[i]*b[3]) + (a[i+1]*b[7]) + (a[i+2]*b[11]) + (a[i+3]*b[15]);
  //}

  for (int i = 0; i != 16; ++i) {
    const int aa = i & ~3, bb = i & 3;
    m[i] = (a[aa]*b[bb]) + (a[aa+1]*b[bb+4])
      + (a[aa+2]*b[bb+8]) + (a[aa+3]*b[bb+12]);
  }

  return m;
}

template<typename T>
[[nodiscard]] constexpr Matrix4x4<T,COLUMN_MAJOR> operator*(
  const Matrix4x4<T,COLUMN_MAJOR>& a, const Matrix4x4<T,COLUMN_MAJOR>& b)
{
  Matrix4x4<T,COLUMN_MAJOR> m;
  //for (int i = 0; i != 16; i += 4) {
  //  m[i]   = (a[0]*b[i]) + (a[4]*b[i+1]) + (a[8]*b[i+2])  + (a[12]*b[i+3]);
  //  m[i+1] = (a[1]*b[i]) + (a[5]*b[i+1]) + (a[9]*b[i+2])  + (a[13]*b[i+3]);
  //  m[i+2] = (a[2]*b[i]) + (a[6]*b[i+1]) + (a[10]*b[i+2]) + (a[14]*b[i+3]);
  //  m[i+3] = (a[3]*b[i]) + (a[7]*b[i+1]) + (a[11]*b[i+2]) + (a[15]*b[i+3]);
  //}

  for (int i = 0; i != 16; ++i) {
    const int aa = i & 3, bb = i & ~3;
    m[i] = (a[aa]*b[bb]) + (a[aa+4]*b[bb+1])
      + (a[aa+8]*b[bb+2]) + (a[aa+12]*b[bb+3]);
  }

  return m;
}

template<typename T>
[[nodiscard]] constexpr Vector4<T> operator*(
  const Vector4<T>& v, const Matrix4x4<T,ROW_MAJOR>& m)
{
  return Vector4<T>(
    (v.x*m[0]) + (v.y*m[4]) + (v.z*m[8])  + (v.w*m[12]),
    (v.x*m[1]) + (v.y*m[5]) + (v.z*m[9])  + (v.w*m[13]),
    (v.x*m[2]) + (v.y*m[6]) + (v.z*m[10]) + (v.w*m[14]),
    (v.x*m[3]) + (v.y*m[7]) + (v.z*m[11]) + (v.w*m[15]));
}

template<typename T>
[[nodiscard]] constexpr Vector4<T> operator*(
  const Matrix4x4<T,COLUMN_MAJOR>& m, const Vector4<T>& v)
{
  return Vector4<T>(
    (m[0]*v.x) + (m[4]*v.y) + (m[8]*v.z)  + (m[12]*v.w),
    (m[1]*v.x) + (m[5]*v.y) + (m[9]*v.z)  + (m[13]*v.w),
    (m[2]*v.x) + (m[6]*v.y) + (m[10]*v.z) + (m[14]*v.w),
    (m[3]*v.x) + (m[7]*v.y) + (m[11]*v.z) + (m[15]*v.w));
}


// **** Template Stream Operators ****
template<typename T>
inline std::ostream& operator<<(
  std::ostream& out, const Matrix4x4<T,ROW_MAJOR>& m)
{
  for (int i = 0; i != 16; i += 4) {
    out << '[' << m[i] << '\t' << m[i+1] << '\t' << m[i+2]
	<< '\t'	<< m[i+3] << "\t]\n";
  }

  return out;
}

template<typename T>
inline std::ostream& operator<<(
  std::ostream& out, const Matrix4x4<T,COLUMN_MAJOR>& m)
{
  for (int i = 0; i != 4; ++i) {
    out << '[' << m[i] << '\t' << m[i+4] << '\t' << m[i+8]
	<< '\t'	<< m[i+12] << "\t]\n";
  }

  return out;
}


// **** Inline Implementations ****
template<typename T, MatrixOrderType MOT>
constexpr void Matrix4x4<T,MOT>::setZero()
{
  _val[ 0] = 0; _val[ 1] = 0; _val[ 2] = 0; _val[ 3] = 0;
  _val[ 4] = 0; _val[ 5] = 0; _val[ 6] = 0; _val[ 7] = 0;
  _val[ 8] = 0; _val[ 9] = 0; _val[10] = 0; _val[11] = 0;
  _val[12] = 0; _val[13] = 0; _val[14] = 0; _val[15] = 0;
}

template<typename T, MatrixOrderType MOT>
constexpr void Matrix4x4<T,MOT>::setIdentity()
{
  _val[ 0] = 1; _val[ 1] = 0; _val[ 2] = 0; _val[ 3] = 0;
  _val[ 4] = 0; _val[ 5] = 1; _val[ 6] = 0; _val[ 7] = 0;
  _val[ 8] = 0; _val[ 9] = 0; _val[10] = 1; _val[11] = 0;
  _val[12] = 0; _val[13] = 0; _val[14] = 0; _val[15] = 1;
}

template<typename T, MatrixOrderType MOT>
constexpr void Matrix4x4<T,MOT>::setTranslation(T tx, T ty, T tz)
{
  // Translation Matrix:
  // [1 0 0 0]
  // [0 1 0 0]
  // [0 0 1 0]
  // [x y z 1]

  _val[ 0] = 1;  _val[ 1] = 0;  _val[ 2] = 0;  _val[ 3] = 0;
  _val[ 4] = 0;  _val[ 5] = 1;  _val[ 6] = 0;  _val[ 7] = 0;
  _val[ 8] = 0;  _val[ 9] = 0;  _val[10] = 1;  _val[11] = 0;
  _val[12] = tx; _val[13] = ty; _val[14] = tz; _val[15] = 1;
}

template<typename T, MatrixOrderType MOT>
constexpr void Matrix4x4<T,MOT>::translate(T tx, T ty, T tz)
{
  // Translation applied to another matrix (optimized):
  // [a0 a1 a2 a3] [1 0 0 0] [a0+a3x a1+a3y a2+a3z a3]
  // [b0 b1 b2 b3]*[0 1 0 0]=[b0+b3x b1+b3y b2+b3z b3]
  // [c0 c1 c2 c3] [0 0 1 0] [c0+c3x c1+c3y c2+c3z c3]
  // [d0 d1 d2 d3] [x y z 1] [d0+d3x d1+d3y d2+d3z d3]

  //_val[ 0] += _val[ 3] * tx;
  //_val[ 1] += _val[ 3] * ty;
  //_val[ 2] += _val[ 3] * tz;
  //_val[ 4] += _val[ 7] * tx;
  //_val[ 5] += _val[ 7] * ty;
  //_val[ 6] += _val[ 7] * tz;
  //_val[ 8] += _val[11] * tx;
  //_val[ 9] += _val[11] * ty;
  //_val[10] += _val[11] * tz;
  //_val[12] += _val[15] * tx;
  //_val[13] += _val[15] * ty;
  //_val[14] += _val[15] * tz;

  for (int i = 0; i != 16; i += 4) {
    const T v = _val[(i & ~3) + 3];
    _val[i]   += v * tx;
    _val[i+1] += v * ty;
    _val[i+2] += v * tz;
  }
}

template<typename T, MatrixOrderType MOT>
void Matrix4x4<T,MOT>::translateOptimized(T tx, T ty, T tz)
{
  // Translation futher optimized for non-projection matrix
  // assumptions: a3,b3,c3 = 0, d3 = 1;
  // [a0 a1 a2 0] [1 0 0 0] [a0   a1   a2   0]
  // [b0 b1 b2 0]*[0 1 0 0]=[b0   b1   b2   0]
  // [c0 c1 c2 0] [0 0 1 0] [c0   c1   c2   0]
  // [d0 d1 d2 1] [x y z 1] [d0+x d1+y d2+z 1]

  _val[12] += tx;
  _val[13] += ty;
  _val[14] += tz;
}

template<typename T, MatrixOrderType MOT>
constexpr void Matrix4x4<T,MOT>::setRotationX_sc(T sinVal, T cosVal)
{
  // X Axis Rotation Matrix:
  // [1  0  0  0]
  // [0  c  s  0]
  // [0 -s  c  0]
  // [0  0  0  1]

  _val[ 0] = 1; _val[ 1] = 0;       _val[ 2] = 0;      _val[ 3] = 0;
  _val[ 4] = 0; _val[ 5] = cosVal;  _val[ 6] = sinVal; _val[ 7] = 0;
  _val[ 8] = 0; _val[ 9] = -sinVal; _val[10] = cosVal; _val[11] = 0;
  _val[12] = 0; _val[13] = 0;       _val[14] = 0;      _val[15] = 1;
}

template<typename T, MatrixOrderType MOT>
constexpr void Matrix4x4<T,MOT>::rotateX_sc(T sinVal, T cosVal)
{
  // X axis rotation applied to matrix (optimized):
  // c = cos(angle), s = sin(angle)
  // [a0 a1 a2 a3] [1  0  0  0] [a0  (a1)(c)-(a2)(s)  (a1)(s)+(a2)(c)  a3]
  // [b0 b1 b2 b3]*[0  c  s  0]=[b0  (b1)(c)-(b2)(s)  (b1)(s)+(b2)(c)  b3]
  // [c0 c1 c2 c3] [0 -s  c  0] [c0  (c1)(c)-(c2)(s)  (c1)(s)+(c2)(c)  c3]
  // [d0 d1 d2 d3] [0  0  0  1] [d0  (d1)(c)-(d2)(s)  (d1)(s)+(d2)(c)  d3]

  for (int i = 0; i != 16; i += 4) {
    const T t1 = _val[i+1], t2 = _val[i+2];
    _val[i+1] = (t1*cosVal) - (t2*sinVal);
    _val[i+2] = (t1*sinVal) + (t2*cosVal);
  }
}

template<typename T, MatrixOrderType MOT>
constexpr void Matrix4x4<T,MOT>::setRotationY_sc(T sinVal, T cosVal)
{
  // Y Axis Rotation Matrix:
  // [c  0 -s  0]
  // [0  1  0  0]
  // [s  0  c  0]
  // [0  0  0  1]

  _val[ 0] = cosVal; _val[ 1] = 0; _val[ 2] = -sinVal; _val[ 3] = 0;
  _val[ 4] = 0;      _val[ 5] = 1; _val[ 6] = 0;       _val[ 7] = 0;
  _val[ 8] = sinVal; _val[ 9] = 0; _val[10] = cosVal;  _val[11] = 0;
  _val[12] = 0;      _val[13] = 0; _val[14] = 0;       _val[15] = 1;
}

template<typename T, MatrixOrderType MOT>
constexpr void Matrix4x4<T,MOT>::rotateY_sc(T sinVal, T cosVal)
{
  // Y axis rotation applied to matrix (optimized):
  // c = cos(angle), s = sin(angle)
  // [a0 a1 a2 a3] [c  0 -s  0] [(a0)(c)+(a2)(s)  a1  -(a0)(s)+(a2)(c)  a3]
  // [b0 b1 b2 b3]*[0  1  0  0]=[(b0)(c)+(b2)(s)  b1  -(b0)(s)+(b2)(c)  b3]
  // [c0 c1 c2 c3] [s  0  c  0] [(c0)(c)+(c2)(s)  c1  -(c0)(s)+(c2)(c)  c3]
  // [d0 d1 d2 d3] [0  0  0  1] [(d0)(c)+(d2)(s)  d1  -(d0)(s)+(d2)(c)  d3]

  for (int i = 0; i != 16; i += 4) {
    const T t0 = _val[i], t2 = _val[i+2];
    _val[i]   = (t0*cosVal) + (t2*sinVal);
    _val[i+2] = (t2*cosVal) - (t0*sinVal);
  }
}

template<typename T, MatrixOrderType MOT>
constexpr void Matrix4x4<T,MOT>::setRotationZ_sc(T sinVal, T cosVal)
{
  // Z Axis Rotation Matrix:
  // [ c  s  0  0]
  // [-s  c  0  0]
  // [ 0  0  1  0]
  // [ 0  0  0  1]

  _val[ 0] = cosVal;  _val[ 1] = sinVal; _val[ 2] = 0; _val[ 3] = 0;
  _val[ 4] = -sinVal; _val[ 5] = cosVal; _val[ 6] = 0; _val[ 7] = 0;
  _val[ 8] = 0;       _val[ 9] = 0;      _val[10] = 1; _val[11] = 0;
  _val[12] = 0;       _val[13] = 0;      _val[14] = 0; _val[15] = 1;
}

template<typename T, MatrixOrderType MOT>
constexpr void Matrix4x4<T,MOT>::rotateZ_sc(T sinVal, T cosVal)
{
  // Z axis rotation applied to matrix (optimized):
  // c = cos(angle), s = sin(angle)
  // [a0 a1 a2 a3] [ c  s  0  0] [(a0)(c)-(a1)(s)  (a0)(s)+(a1)(c)  a2  a3]
  // [b0 b1 b2 b3]*[-s  c  0  0]=[(b0)(c)-(b1)(s)  (b0)(s)+(b1)(c)  b2  b3]
  // [c0 c1 c2 c3] [ 0  0  1  0] [(c0)(c)-(c1)(s)  (c0)(s)+(c1)(c)  c2  c3]
  // [d0 d1 d2 d3] [ 0  0  0  1] [(d0)(c)-(d1)(s)  (d0)(s)+(d1)(c)  d2  d3]

  for (int i = 0; i != 16; i += 4) {
    const T t0 = _val[i], t1 = _val[i+1];
    _val[i]   = (t0*cosVal) - (t1*sinVal);
    _val[i+1] = (t0*sinVal) + (t1*cosVal);
  }
}

template<typename T, MatrixOrderType MOT>
constexpr void Matrix4x4<T,MOT>::setRotation_sc(
  const Vector3<T>& axis, T sinVal, T cosVal)
{
  const T cinv = static_cast<T>(1) - cosVal;
  const T xyc = axis.x * axis.y * cinv;
  const T xzc = axis.x * axis.z * cinv;
  const T yzc = axis.y * axis.z * cinv;
  const T xs = axis.x * sinVal;
  const T ys = axis.y * sinVal;
  const T zs = axis.z * sinVal;

  _val[ 0] = (Sqr(axis.x) * cinv) + cosVal;
  _val[ 1] = xyc + zs;
  _val[ 2] = xzc - ys;
  _val[ 3] = 0;

  _val[ 4] = xyc - zs;
  _val[ 5] = (Sqr(axis.y) * cinv) + cosVal;
  _val[ 6] = yzc + xs;
  _val[ 7] = 0;

  _val[ 8] = xzc + ys;
  _val[ 9] = yzc - xs;
  _val[10] = (Sqr(axis.z) * cinv) + cosVal;
  _val[11] = 0;

  _val[12] = 0;
  _val[13] = 0;
  _val[14] = 0;
  _val[15] = 1;
}

template<typename T, MatrixOrderType MOT>
constexpr void Matrix4x4<T,MOT>::rotate_sc(
  const Vector3<T>& axis, T sinVal, T cosVal)
{
  // Rotation Matrix (around unit vector x,y,z)
  // c = cos(angle), s = sin(angle)
  // [(x^2)(1-c)+c  (yx)(1-c)+zs  (xz)(1-c)-ys  0]
  // [(xy)(1-c)-zs  (y^2)(1-c)+c  (yz)(1-c)+xs  0]
  // [(xz)(1-c)+ys  (yz)(1-c)-xs  (z^2)(1-c)+c  0]
  // [      0             0             0       1]

  const T cinv = static_cast<T>(1) - cosVal;
  const T xxc = (Sqr(axis.x) * cinv) + cosVal;
  const T yyc = (Sqr(axis.y) * cinv) + cosVal;
  const T zzc = (Sqr(axis.z) * cinv) + cosVal;
  const T xy = axis.x * axis.y * cinv;
  const T xz = axis.x * axis.z * cinv;
  const T yz = axis.y * axis.z * cinv;
  const T xs = axis.x * sinVal;
  const T ys = axis.y * sinVal;
  const T zs = axis.z * sinVal;

  for (int i = 0; i != 16; i += 4) {
    const T t0 = _val[i];
    const T t1 = _val[i+1];
    const T t2 = _val[i+2];

    _val[i]   = t0*xxc       + t1*(xy - zs) + t2*(xz + ys);
    _val[i+1] = t0*(xy + zs) + t1*yyc       + t2*(yz - xs);
    _val[i+2] = t0*(xz - ys) + t1*(yz + xs) + t2*zzc;
  }
}

template<typename T, MatrixOrderType MOT>
constexpr void Matrix4x4<T,MOT>::setScaling(T sx, T sy, T sz)
{
  // Scaling Matrix:
  // [x 0 0 0]
  // [0 y 0 0]
  // [0 0 z 0]
  // [0 0 0 1]

  _val[ 0] = sx; _val[ 1] = 0;  _val[ 2] = 0;  _val[ 3] = 0;
  _val[ 4] = 0;  _val[ 5] = sy; _val[ 6] = 0;  _val[ 7] = 0;
  _val[ 8] = 0;  _val[ 9] = 0;  _val[10] = sz; _val[11] = 0;
  _val[12] = 0;  _val[13] = 0;  _val[14] = 0;  _val[15] = 1;
}

template<typename T, MatrixOrderType MOT>
constexpr void Matrix4x4<T,MOT>::scale(T sx, T sy, T sz)
{
  // Scaling applied to matrix (optimized):
  // [a0 a1 a2 a3] [x 0 0 0] [a0x a1y a2z a3]
  // [b0 b1 b2 b3]*[0 y 0 0]=[b0x b1y b2z b3]
  // [c0 c1 c2 c3] [0 0 z 0] [c0x c1y c2z c3]
  // [d0 d1 d2 d3] [0 0 0 1] [d0x d1y d2z d3]

  _val[0]  *= sx; _val[1]  *= sy; _val[2]  *= sz;
  _val[4]  *= sx; _val[5]  *= sy; _val[6]  *= sz;
  _val[8]  *= sx; _val[9]  *= sy; _val[10] *= sz;
  _val[12] *= sx; _val[13] *= sy; _val[14] *= sz;
}

template<typename T, MatrixOrderType MOT>
constexpr void Matrix4x4<T,MOT>::transpose()
{
  // Transposition
  // [a0 a1 a2 a3]    [a0 b0 c0 d0]
  // [b0 b1 b2 b3] => [a1 b1 c1 d1]
  // [c0 c1 c2 c3]    [a2 b2 c2 d2]
  // [d0 d1 d2 d3]    [a3 b3 c3 d3]

  // std::swap not constexpr until C++20
  //std::swap(_val[ 1], _val[ 4]);
  //std::swap(_val[ 2], _val[ 8]);
  //std::swap(_val[ 3], _val[12]);
  //std::swap(_val[ 6], _val[ 9]);
  //std::swap(_val[ 7], _val[13]);
  //std::swap(_val[11], _val[14]);

  T tmp;
  tmp = _val[1]; _val[1] = _val[4]; _val[4] = tmp;
  tmp = _val[2]; _val[2] = _val[8]; _val[8] = tmp;
  tmp = _val[3]; _val[3] = _val[12]; _val[12] = tmp;
  tmp = _val[6]; _val[6] = _val[9]; _val[9] = tmp;
  tmp = _val[7]; _val[7] = _val[13]; _val[13] = tmp;
  tmp = _val[11]; _val[11] = _val[14]; _val[14] = tmp;
}


// **** Template Functions ****
// MultPoint() - row vector * row major matrix
template<typename T>
[[nodiscard]] constexpr Vector3<T> MultPoint(
  const Vector3<T>& v, const Matrix4x4<T,ROW_MAJOR>& m)
{
  // assumptions: v.w = 1, m[3,7,11] = 0, m[15] = 1
  return Vector3<T>((v.x*m[0]) + (v.y*m[4]) + (v.z*m[8])  + m[12],
		    (v.x*m[1]) + (v.y*m[5]) + (v.z*m[9])  + m[13],
		    (v.x*m[2]) + (v.y*m[6]) + (v.z*m[10]) + m[14]);
}

// MultPoint() - column major matrix * column vector
template<typename T>
[[nodiscard]] constexpr Vector3<T> MultPoint(
  const Matrix4x4<T,COLUMN_MAJOR>& m, const Vector3<T>& v)
{
  // assumptions: v.w = 1, m[3,7,11] = 0, m[15] = 1
  return Vector3<T>((m[0]*v.x) + (m[4]*v.y) + (m[8]*v.z)  + m[12],
		    (m[1]*v.x) + (m[5]*v.y) + (m[9]*v.z)  + m[13],
		    (m[2]*v.x) + (m[6]*v.y) + (m[10]*v.z) + m[14]);
}

// MultVector() - row vector * row major matrix
template<typename T>
[[nodiscard]] constexpr Vector3<T> MultVector(
  const Vector3<T>& v, const Matrix4x4<T,ROW_MAJOR>& m)
{
  // assumptions: v.w = 0, m[3,7,11] = 0, m[15] = 1
  return Vector3<T>((v.x*m[0]) + (v.y*m[4]) + (v.z*m[8]),
		    (v.x*m[1]) + (v.y*m[5]) + (v.z*m[9]),
		    (v.x*m[2]) + (v.y*m[6]) + (v.z*m[10]));
}

// MultVector() - column major matrix * column vector
template<typename T>
[[nodiscard]] constexpr Vector3<T> MultVector(
  const Matrix4x4<T,COLUMN_MAJOR>& m, const Vector3<T>& v)
{
  // assumptions: v.w = 0, m[3,7,11] = 0, m[15] = 1
  return Vector3<T>((m[0]*v.x) + (m[4]*v.y) + (m[8]*v.z),
		    (m[1]*v.x) + (m[5]*v.y) + (m[9]*v.z),
		    (m[2]*v.x) + (m[6]*v.y) + (m[10]*v.z));
}

// MultVectorTrans - row vector * transpose(row major matrix)
template<typename T>
[[nodiscard]] constexpr Vector3<T> MultVectorTrans(
  const Vector3<T>& v, const Matrix4x4<T,ROW_MAJOR>& m)
{
  // assumptions: v.w = 0, m[12,13,14] = 0, m[15] = 1
  return Vector3<T>((v.x*m[0]) + (v.y*m[1]) + (v.z*m[2]),
		    (v.x*m[4]) + (v.y*m[5]) + (v.z*m[6]),
		    (v.x*m[8]) + (v.y*m[9]) + (v.z*m[10]));
}

// MultVectorTrans() - transpose(column major matrix) * column vector
template<typename T>
[[nodiscard]] constexpr Vector3<T> MultVectorTrans(
  const Matrix4x4<T,COLUMN_MAJOR>& m, const Vector3<T>& v)
{
  // assumptions: v.w = 0, m[12,13,14] = 0, m[15] = 1
  return Vector3<T>((m[0]*v.x) + (m[1]*v.y) + (m[2]*v.z),
		    (m[4]*v.x) + (m[5]*v.y) + (m[6]*v.z),
		    (m[8]*v.x) + (m[9]*v.y) + (m[10]*v.z));
}

// Matrix Inversion
template<typename T, MatrixOrderType MOT>
int InvertMatrix(const Matrix4x4<T,MOT>& m, Matrix4x4<T,MOT>& dst)
{
  // based off 'Streaming SIMD Extensions - Inverse of 4x4 Matrix'
  // (Intel document AP-928)
  // inversion done with Cramer's Rule

  // calculate pairs for elements 0-7 (cofactors)
  T t0  = m[10] * m[15];
  T t1  = m[14] * m[11];
  T t2  = m[6]  * m[15];
  T t3  = m[14] * m[7];
  T t4  = m[6]  * m[11];
  T t5  = m[10] * m[7];
  T t6  = m[2]  * m[15];
  T t7  = m[14] * m[3];
  T t8  = m[2]  * m[11];
  T t9  = m[10] * m[3];
  T t10 = m[2]  * m[7];
  T t11 = m[6]  * m[3];

  // calculate elements 0-3 (cofactors)
  const T c0 = (t0*m[5] + t3*m[9] + t4*m[13]) - (t1*m[5] + t2*m[9] + t5*m[13]);
  const T c1 = (t1*m[1] + t6*m[9] + t9*m[13]) - (t0*m[1] + t7*m[9] + t8*m[13]);
  const T c2 = (t2*m[1] + t7*m[5] + t10*m[13]) - (t3*m[1] + t6*m[5] + t11*m[13]);
  const T c3 = (t5*m[1] + t8*m[5] + t11*m[9]) - (t4*m[1] + t9*m[5] + t10*m[9]);

  // calculate determinant
  const T det = m[0]*c0 + m[4]*c1 + m[8]*c2 + m[12]*c3;
  if (IsZero(det)) {
    return -1; // error - singular matrix
  }

  dst[0] = c0;
  dst[1] = c1;
  dst[2] = c2;
  dst[3] = c3;

  // calculate elements 4-7 (cofactors)
  dst[4] = (t1*m[4] + t2*m[8] + t5*m[12]) - (t0*m[4] + t3*m[8] + t4*m[12]);
  dst[5] = (t0*m[0] + t7*m[8] + t8*m[12]) - (t1*m[0] + t6*m[8] + t9*m[12]);
  dst[6] = (t3*m[0] + t6*m[4] + t11*m[12]) - (t2*m[0] + t7*m[4] + t10*m[12]);
  dst[7] = (t4*m[0] + t9*m[4] + t10*m[8]) - (t5*m[0] + t8*m[4] + t11*m[8]);

  // calculate pairs for elements 8-15 (cofactors)
  t0  = m[8]  * m[13];
  t1  = m[12] * m[9];
  t2  = m[4]  * m[13];
  t3  = m[12] * m[5];
  t4  = m[4]  * m[9];
  t5  = m[8]  * m[5];
  t6  = m[0]  * m[13];
  t7  = m[12] * m[1];
  t8  = m[0]  * m[9];
  t9  = m[8]  * m[1];
  t10 = m[0]  * m[5];
  t11 = m[4]  * m[1];

  // calculate elements 8-15 (cofactors)
  dst[8]  = (t0*m[7] + t3*m[11] + t4*m[15]) - (t1*m[7] + t2*m[11] + t5*m[15]);
  dst[9]  = (t1*m[3] + t6*m[11] + t9*m[15]) - (t0*m[3] + t7*m[11] + t8*m[15]);
  dst[10] = (t2*m[3] + t7*m[7] + t10*m[15]) - (t3*m[3] + t6*m[7] + t11*m[15]);
  dst[11] = (t5*m[3] + t8*m[7] + t11*m[11]) - (t4*m[3] + t9*m[7] + t10*m[11]);
  dst[12] = (t2*m[10] + t5*m[14] + t1*m[6]) - (t4*m[14] + t0*m[6] + t3*m[10]);
  dst[13] = (t8*m[14] + t0*m[2] + t7*m[10]) - (t6*m[10] + t9*m[14] + t1*m[2]);
  dst[14] = (t6*m[6] + t11*m[14] + t3*m[2]) - (t10*m[14] + t2*m[2] + t7*m[6]);
  dst[15] = (t10*m[10] + t4*m[2] + t9*m[6]) - (t8*m[6] + t11*m[10] + t5*m[2]);

  // calculate inverse matrix
  const T inv = static_cast<T>(1) / det;
  for (auto& v : dst) { v *= inv; }

  // no errors
  return 0;
}