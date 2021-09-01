//
// FrameBuffer.cc
// Copyright (C) 2021 Richard Bradley
//

#include "FrameBuffer.hh"
#include "MathUtility.hh"
#include "Print.hh"
#include <fstream>
#include <algorithm>
#include <memory>


// **** FrameBuffer Class ****
int FrameBuffer::init(int w, int h, const Color& c)
{
  if (w <= 0 || h <= 0) { return -1; }

  _width = w;
  _height = h;
  _buffer.resize(std::size_t(w * h * CHANNELS));
  clear(c);
  return 0;
}

int FrameBuffer::saveBMP(const std::string& filename) const
{
  if (_buffer.empty()) { return -1; }

  const int row_pad = (4 - ((_width * CHANNELS) % 4)) % 4;
    // rows must be padded to 4-byte multiple
  const int row_size = (_width * CHANNELS) + row_pad;
  const uint32_t file_size = 54 + uint32_t(row_size * _height);

  // setup BMP header
  uint8_t header[54] = {};
  header[ 0] = 66;                               // BMP type "BM"
  header[ 1] = 77;
  header[ 2] = uint8_t(file_size         & 255); // File size
  header[ 3] = uint8_t((file_size >>  8) & 255);
  header[ 4] = uint8_t((file_size >> 16) & 255);
  header[ 5] = uint8_t(file_size >> 24);
  header[10] = 54;                               // Bitmap start
  header[14] = 40;                               // BITMAPINFOHEADER size
  header[18] = uint8_t(_width          & 255);   // BMP width
  header[19] = uint8_t((_width  >>  8) & 255);
  header[20] = uint8_t((_width  >> 16) & 255);
  header[21] = uint8_t(_width  >> 24);
  header[22] = uint8_t(_height         & 255);   // BMP height
  header[23] = uint8_t((_height >>  8) & 255);
  header[24] = uint8_t((_height >> 16) & 255);
  header[25] = uint8_t(_height >> 24);
  header[26] = 1;                                // Planes
  header[28] = 24;                               // Bit count

  // create BMP file
  std::ofstream file(filename, std::ios::out | std::ios::binary);
  if (!file) {
    println_err("Error writing file '", filename, "'");
    return -1;
  }

  file.write(reinterpret_cast<char*>(header), sizeof(header));

  auto row = std::make_unique<uint8_t[]>(std::size_t(row_size));
  auto src = _buffer.begin();
  for (int y = 0; y < _height; ++y) {
    uint8_t* out = row.get();
    for (int x = 0; x < _width; ++x) {
      float r = *src++;
      float g = *src++;
      float b = *src++;

      *out++ = uint8_t((std::clamp(b, 0.0f, 1.0f) * 255.0f) + .5f);
      *out++ = uint8_t((std::clamp(g, 0.0f, 1.0f) * 255.0f) + .5f);
      *out++ = uint8_t((std::clamp(r, 0.0f, 1.0f) * 255.0f) + .5f);
    }
    file.write(reinterpret_cast<char*>(row.get()), row_size);
  }

  return 0;
}

void FrameBuffer::clear(const Color& c)
{
  const float r = static_cast<float>(c.red());
  const float g = static_cast<float>(c.green());
  const float b = static_cast<float>(c.blue());

  auto itr = _buffer.begin(), end = _buffer.end();
  while (itr != end) {
    *itr++ = r;
    *itr++ = g;
    *itr++ = b;
  }
}

int FrameBuffer::plot(int x, int y, const Color& c)
{
  if ((x < 0) || (x >= _width) || (y < 0) || (y >= _height)) { return -1; }

  float* ptr = &_buffer[std::size_t(((y * _width) + x) * CHANNELS)];
  *ptr++ = static_cast<float>(c.red());
  *ptr++ = static_cast<float>(c.green());
  *ptr++ = static_cast<float>(c.blue());
  return 0;
}

int FrameBuffer::value(int x, int y, float& r, float& g, float& b) const
{
  if ((x < 0) || (x >= _width) || (y < 0) || (y >= _height)) { return -1; }

  const float* ptr = &_buffer[std::size_t(((y * _width) + x) * CHANNELS)];
  r = *ptr++;
  g = *ptr++;
  b = *ptr++;
  return 0;
}

int FrameBuffer::range(float& min_val, float& max_val) const
{
  if (_buffer.empty()) { return -1; }

  float low = 3.4e38f, high = 0.0f;
  for (auto& v : _buffer) {
    if (v < low)  { low = v; }
    if (v > high) { high = v; }
  }

  min_val = low;
  max_val = high;
  return 0;
}
