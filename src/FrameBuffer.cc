//
// FrameBuffer.cc
// Copyright (C) 2024 Richard Bradley
//

#include "FrameBuffer.hh"
#include "MathUtil.hh"
#include "Print.hh"
#include <fstream>
#include <algorithm>
#include <memory>
#include <cassert>
#include <png.h>


// **** FrameBuffer Class ****
int FrameBuffer::init(int w, int h, const Color& c)
{
  if (w <= 0 || h <= 0) { return -1; }

  _width = w;
  _rowSize = w * _channels;
  if (_rowSize % 16) {
    // padding row size to 64 byte alignment (16 * sizeof float)
    _rowSize += 16 - (_rowSize % 16);
    assert((_rowSize % 16) == 0);
  }

  _height = h;
  _buffer.reset(new(std::align_val_t{64}) float[std::size_t(_rowSize * h)]);
  clear(c);
  return 0;
}

int FrameBuffer::saveBMP(const std::string& filename) const
{
  if (!_buffer) { return -1; }

  const int bmp_row_pad = (4 - ((_width * _channels) % 4)) % 4;
    // rows must be padded to 4-byte multiple
  const int bmp_row_size = (_width * _channels) + bmp_row_pad;
  const uint32_t file_size = 54 + uint32_t(bmp_row_size * _height);

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
  header[28] = (_channels == 4) ? 32 : 24;       // Bit count

  // create BMP file
  std::ofstream file{filename, std::ios::out | std::ios::binary};
  if (!file) {
    println_err("Error writing bmp file");
    return -1;
  }

  file.write(reinterpret_cast<char*>(header), sizeof(header));

  auto row = std::make_unique<uint8_t[]>(std::size_t(bmp_row_size));
  for (int y = 0; y < _height; ++y) {
    const float* src = bufferRow(y);
    uint8_t* out = row.get();
    for (int x = 0; x < _width; ++x) {
      *out++ = uint8_t((std::clamp(src[2], 0.0f, 1.0f) * 255.0f) + .5f); // B
      *out++ = uint8_t((std::clamp(src[1], 0.0f, 1.0f) * 255.0f) + .5f); // G
      *out++ = uint8_t((std::clamp(src[0], 0.0f, 1.0f) * 255.0f) + .5f); // R
      if (_channels == 4) { // alpha
        *out++ = uint8_t((std::clamp(src[3], 0.0f, 1.0f) * 255.0f) + .5f);
      }
      src += _channels;
    }
    file.write(reinterpret_cast<char*>(row.get()), bmp_row_size);
  }

  return 0;
}

int FrameBuffer::savePNG(const std::string& filename) const
{
  if (!_buffer) { return -1; }

  png_image image{};
  image.version = PNG_IMAGE_VERSION;
  image.width   = static_cast<png_uint_32>(_width);
  image.height  = static_cast<png_uint_32>(_height);
  image.format  = (_channels == 4) ? PNG_FORMAT_RGBA : PNG_FORMAT_RGB;

  const std::size_t size = PNG_IMAGE_SIZE(image);
  assert(size >= std::size_t(_width * _height * _channels));
  auto output = std::make_unique<uint8_t[]>(size);

  uint8_t* out = output.get();
  for (int y = _height - 1; y >= 0; --y) {
    const float* src = bufferRow(y);
    for (int x = 0; x < _width; ++x) {
      *out++ = uint8_t((std::clamp(src[0], 0.0f, 1.0f) * 255.0f) + .5f); // R
      *out++ = uint8_t((std::clamp(src[1], 0.0f, 1.0f) * 255.0f) + .5f); // G
      *out++ = uint8_t((std::clamp(src[2], 0.0f, 1.0f) * 255.0f) + .5f); // B
      if (_channels == 4) { // alpha
        *out++ = uint8_t((std::clamp(src[3], 0.0f, 1.0f) * 255.0f) + .5f);
      }
      src += _channels;
    }
  }

  if (png_image_write_to_file(
        &image, filename.c_str(), 0, output.get(), 0, nullptr) == 0) {
    println("Error writing png file");
    return -1;
  }

  return 0;
}

void FrameBuffer::clear(const Color& c)
{
  for (int y = 0; y < _height; ++y) {
    float* ptr = bufferRow(y);
    for (int x = 0; x < _width; ++x) {
      *ptr++ = c[0];
      *ptr++ = c[1];
      *ptr++ = c[2];
      if (_channels == 4) { *ptr++ = c[3]; }
    }
  }
}

int FrameBuffer::plot(int x, int y, const Color& c)
{
  if (!inRange(x, y)) { return -1; }

  float* ptr = pixel(x, y);
  ptr[0] = c[0];
  ptr[1] = c[1];
  ptr[2] = c[2];
  if (_channels == 4) { ptr[3] = c[3]; }
  return 0;
}

Color FrameBuffer::value(int x, int y) const
{
  if (!inRange(x, y)) { return colors::black; }

  const float* ptr = pixel(x, y);
  return {ptr[0], ptr[1], ptr[2]};
}

int FrameBuffer::range(float& min_val, float& max_val) const
{
  min_val = max_val = 0;
  if (!_buffer) { return -1; }

  float low = _buffer[0], high = _buffer[0];
  const int width = _width * _channels;
  for (int y = 0; y < _height; ++y) {
    const float* ptr = bufferRow(y);
    const float* end = ptr + width;
    while (ptr != end) {
      const float v = *ptr++;
      if (v < low)  { low = v; }
      if (v > high) { high = v; }
    }
  }

  min_val = low;
  max_val = high;
  return 0;
}
