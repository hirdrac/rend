//
// FrameBuffer.hh
// Copyright (C) 2021 Richard Bradley
//
// RGB image frame buffer class library (holds render result)
//

#pragma once
#include "Color.hh"
#include <string>
#include <vector>

// ADD - add low mem (24 bits/pixel) image class
// ADD - add mips map image class


// **** Types ****
class FrameBuffer
{
 public:
  FrameBuffer() = default;
  FrameBuffer(int w, int h) { init(w, h); }

  // Member Functions
  int init(int width, int height);
  int saveBMP(const std::string& filename) const;

  void clear(float r = 0, float g = 0, float b = 0);
  void clear(const Color& c) {
    clear(static_cast<Color::value_type>(c.red()),
          static_cast<Color::value_type>(c.green()),
          static_cast<Color::value_type>(c.blue())); }

  int plot(int x, int y, float r, float g, float b);
  int plot(int x, int y, const Color& c) {
    return plot(x, y,
                static_cast<Color::value_type>(c.red()),
                static_cast<Color::value_type>(c.green()),
                static_cast<Color::value_type>(c.blue())); }

  int line(int x0, int y0, int x1, int y1, float r, float g, float b);
  int line(int x0, int y0, int x1, int y1, const Color& c) {
    return line(x0, y0, x1, y1,
                static_cast<Color::value_type>(c.red()),
                static_cast<Color::value_type>(c.green()),
                static_cast<Color::value_type>(c.blue())); }

  int value(int x, int y, float& r, float& g, float& b) const;
  int range(float& min, float& max) const;

  [[nodiscard]] int width() const { return _width; }
  [[nodiscard]] int height() const { return _height; }

 private:
  int _width = 0, _height = 0;
  std::vector<float> _buffer;
};
