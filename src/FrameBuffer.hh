//
// FrameBuffer.hh
// Copyright (C) 2021 Richard Bradley
//
// RGB image frame buffer class for holding render result
//

#pragma once
#include "Color.hh"
#include <string>
#include <vector>


class FrameBuffer
{
 public:
  FrameBuffer() = default;
  FrameBuffer(int w, int h) { init(w, h); }

  // Member Functions
  int init(int width, int height);
  int saveBMP(const std::string& filename) const;

  void clear(const Color& c = colors::black);
  int plot(int x, int y, const Color& c);

  int value(int x, int y, float& r, float& g, float& b) const;
  int range(float& min, float& max) const;

  [[nodiscard]] int width() const { return _width; }
  [[nodiscard]] int height() const { return _height; }

 private:
  std::vector<float> _buffer;
  int _width = 0, _height = 0;

  static constexpr int CHANNELS = 3; // RGB
};
