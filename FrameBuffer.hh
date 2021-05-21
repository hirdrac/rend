//
// FrameBuffer.hh
// Copyright (C) 2020 Richard Bradley
//
// RGB image frame buffer class library (holds render result)
//

#pragma once
#include <string>
#include <vector>

// ADD - add low mem (24 bits/pixel) image class
// ADD - add mips map image class


// **** Types ****
class FrameBuffer
{
 public:
  // Constructors
  FrameBuffer() = default;
  FrameBuffer(int w, int h) { init(w, h); }

  // Member Functions
  int init(int width, int height);
  int saveBMP(const std::string& filename) const;

  int clear(float r = 0, float g = 0, float b = 0);
  int plot(int x, int y, float r, float g, float b);
  int line(int x0, int y0, int x1, int y1, float r, float g, float b);
  int value(int x, int y, float& r, float& g, float& b) const;
  int range(float& min, float& max) const;

  int width() const { return _width; }
  int height() const { return _height; }

 private:
  int _width = 0, _height = 0, _pixels = 0;
  std::vector<float> _buffer;

  // prevent copy/assign
  FrameBuffer(const FrameBuffer&) = delete;
  FrameBuffer& operator=(const FrameBuffer&) = delete;
};