//
// FrameBuffer.hh
// Copyright (C) 2022 Richard Bradley
//
// RGB image frame buffer class for holding render result
//

#pragma once
#include "Color.hh"
#include <string>
#include <memory>


class FrameBuffer
{
 public:
  FrameBuffer() = default;
  FrameBuffer(int w, int h) { init(w, h); }

  // Member Functions
  int init(int width, int height, const Color& c = colors::black);
  int saveBMP(const std::string& filename) const;
  int savePNG(const std::string& filename) const;

  void clear(const Color& c = colors::black);
  int plot(int x, int y, const Color& c);

  [[nodiscard]] Color value(int x, int y) const;
  int range(float& min, float& max) const;

  [[nodiscard]] int width() const { return _width; }
  [[nodiscard]] int height() const { return _height; }

 private:
  std::unique_ptr<float[]> _buffer;
  int _width = 0, _height = 0; // image size
  int _channels = 3; // 3 or 4 only
  int _rowSize = 0;  // (width * channels) padded to 64 byte alignment

  [[nodiscard]] float* bufferRow(int y) {
    return _buffer.get() + (_rowSize * y); }
  [[nodiscard]] const float* bufferRow(int y) const {
    return _buffer.get() + (_rowSize * y); }

  [[nodiscard]] float* pixel(int x, int y) {
    return bufferRow(y) + (x * _channels); }
  [[nodiscard]] const float* pixel(int x, int y) const {
    return bufferRow(y) + (x * _channels); }

  [[nodiscard]] bool inRange(int x, int y) const {
    return (x >= 0) && (x < _width) && (y >= 0) && (y < _height); }
};
