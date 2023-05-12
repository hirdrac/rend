//
// PerlinNoise.hh
// Copyright (C) 2023 Richard Bradley
//
// from 'Improved Noise' by Ken Perlin (SIGGRAPH 2002)
// http://mrl.nyu.edu/~perlin/noise/
//

#pragma once
#include <concepts>
#include <cmath>


namespace perlin {
  template<std::floating_point T>
  constexpr T fade(T t) {
    return t * t * t * ((t * ((t * 6) - 15)) + 10);
  }

  template<std::floating_point T>
  constexpr T grad(int hash, T x, T y, T z) {
    const int h = hash & 15;
    const T u = (h < 8) ? x : y;
    const T v = (h < 4) ? y : (((h == 12) || (h == 14)) ? x : z);
    return ((h & 1) ? -u : u) + ((h & 2) ? -v : v);
  }

  template<std::floating_point T>
  T noise(T x, T y, T z) {
    static constexpr int data[257] = {
      151,160,137,91, 90,15,131,13, 201,95,96,53, 194,233,7,225,
      140,36,103,30, 69,142,8,99, 37,240,21,10, 23,190,6,148,
      247,120,234,75, 0,26,197,62, 94,252,219,203, 117,35,11,32,
      57,177,33,88, 237,149,56,87, 174,20,125,136, 171,168,68,175,

      74,165,71,134, 139,48,27,166, 77,146,158,231, 83,111,229,122,
      60,211,133,230, 220,105,92,41, 55,46,245,40, 244,102,143,54,
      65,25,63,161, 1,216,80,73, 209,76,132,187, 208,89,18,169,
      200,196,135,130, 116,188,159,86, 164,100,109,198, 173,186,3,64,

      52,217,226,250, 124,123,5,202, 38,147,118,126, 255,82,85,212,
      207,206,59,227, 47,16,58,17, 182,189,28,42, 223,183,170,213,
      119,248,152,2, 44,154,163,70, 221,153,101,155, 167,43,172,9,
      129,22,39,253, 19,98,108,110, 79,113,224,232, 178,185,112,104,

      218,246,97,228, 251,34,242,193, 238,210,144,12, 191,179,162,241,
      81,51,145,235, 249,14,239,107, 49,192,214,31, 181,199,106,157,
      184,84,204,176, 115,121,50,45, 127,4,150,254, 138,236,205,93,
      222,114,67,29, 24,72,243,141, 128,195,78,66, 215,61,156,180,

      // repeat from start
      151
    };

    const T fx = std::floor(x);
    const T fy = std::floor(y);
    const T fz = std::floor(z);

    // unit cube that contains point
    const int cx = int(fx) & 255;
    const int cy = int(fy) & 255;
    const int cz = int(fz) & 255;

    // hash coord of 8 cube corners
    const int a  = (data[cx]+cy) & 255;
    const int b  = (data[cx+1]+cy) & 255;
    const int aa = (data[a]+cz) & 255;
    const int ab = (data[a+1]+cz) & 255;
    const int ba = (data[b]+cz) & 255;
    const int bb = (data[b+1]+cz) & 255;

    // relative x,y,z of point in cube
    const T rx = x - fx;
    const T ry = y - fy;
    const T rz = z - fz;

    // fade curves
    const T u = fade(rx);
    const T v = fade(ry);
    const T w = fade(rz);

    // final noise value
    const T n1 =
      std::lerp(
        std::lerp(grad(data[aa], rx, ry, rz),
                  grad(data[ba], rx-1, ry, rz), u),
        std::lerp(grad(data[ab], rx, ry-1, rz),
                  grad(data[bb], rx-1, ry-1, rz), u),
        v);
    const T n2 =
      std::lerp(
        std::lerp(grad(data[aa+1], rx, ry, rz-1),
                  grad(data[ba+1], rx-1, ry, rz-1), u),
        std::lerp(grad(data[ab+1], rx, ry-1, rz-1),
                  grad(data[bb+1], rx-1, ry-1, rz-1), u),
        v);
    return std::lerp(n1, n2, w);
  }
}
