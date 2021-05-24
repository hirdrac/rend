//
// FrameBuffer.cc
// Copyright (C) 2021 Richard Bradley
//
// Implementation of FrameBuffer module
//

#include "FrameBuffer.hh"
#include "MathUtility.hh"
#include "Logger.hh"
#include <fstream>
#include <algorithm>


/**** FrameBuffer Class ****/
// Member Functions
int FrameBuffer::init(int w, int h)
{
  _width = w;
  _height = h;
  _pixels = w * h;
  _buffer.resize(_pixels * 3);
  return 0;
}

int FrameBuffer::saveBMP(const std::string& filename) const
{
  int file_size = 54 + (_pixels * 3);

  // Set up 24bit BMP header
  uint8_t header[54] = {};
  header[ 0] = 66;                      // BMP type
  header[ 1] = 77;
  header[ 2] = file_size         & 255; // File size
  header[ 3] = (file_size >>  8) & 255;
  header[ 4] = (file_size >> 16) & 255;
  header[ 5] = (file_size >> 24) & 255;
  header[10] = 54;                      // Bitmap start
  header[14] = 40;                      // BITMAPINFOHEADER size
  header[18] = _width          & 255;   // BMP width
  header[19] = (_width  >>  8) & 255;
  header[20] = (_width  >> 16) & 255;
  header[21] = (_width  >> 24) & 255;
  header[22] = _height         & 255;   // BMP height
  header[23] = (_height >>  8) & 255;
  header[24] = (_height >> 16) & 255;
  header[25] = (_height >> 24) & 255;
  header[26] = 1;                       // Planes
  header[28] = 24;                      // Bit count

  // create BMP file
  std::ofstream file(filename, std::ios::out | std::ios::binary);
  if (!file) {
    LOG_ERROR("Error writing file '", filename, "'");
    return -1;
  }

  file.write(reinterpret_cast<char*>(header), sizeof(header));

  auto itr = _buffer.begin(), end = _buffer.end();
  while (itr != end) {
    float r = *itr++;
    float g = *itr++;
    float b = *itr++;

    file << uint8_t((std::clamp(b, 0.0f, 1.0f) * 255.0f) + .5f);
    file << uint8_t((std::clamp(g, 0.0f, 1.0f) * 255.0f) + .5f);
    file << uint8_t((std::clamp(r, 0.0f, 1.0f) * 255.0f) + .5f);
  }

  return 0;
}

int FrameBuffer::clear(float r, float g, float b)
{
  auto itr = _buffer.begin(), end = _buffer.end();
  while (itr != end) {
    *itr++ = r;
    *itr++ = g;
    *itr++ = b;
  }
  return 0;
}

int FrameBuffer::plot(int x, int y, float r, float g, float b)
{
  if ((x < 0) || (x >= _width) || (y < 0) || (y >= _height)) {
    return -1;
  }

  float* ptr = &_buffer[((y * _width) + x) * 3];
  *ptr++ = r;
  *ptr++ = g;
  *ptr++ = b;
  return 0;
}

int FrameBuffer::line(int x0, int y0, int x1, int y1,
		      float r, float g, float b)
{
  int dir_x   = Sgn(x1 - x0), dir_y   = Sgn(y1 - y0);
  int delta_x = abs(x1 - x0), delta_y = abs(y1 - y0);
  int step    = 0;

  if (delta_x < delta_y) {
    while (y0 != y1) {
      plot(x0, y0, r, g, b);
      y0   += dir_y;
      step += delta_x;

      if (step >= delta_y) {
	step -= delta_y;
	x0   += dir_x;
      }
    }
  } else if (delta_y < delta_x) {
    while (x0 != x1) {
      plot(x0, y0, r, g, b);
      x0   += dir_x;
      step += delta_y;

      if (step >= delta_x) {
	step -= delta_x;
	y0   += dir_y;
      }
    }
  } else {
    while (x0 != x1) {
      plot(x0, y0, r, g, b);
      x0 += dir_x;
      y0 += dir_y;
    }
  }

  return 0;
}

int FrameBuffer::value(int x, int y, float& r, float& g, float& b) const
{
  if ((x < 0) || (x >= _width) || (y < 0) || (y >= _height)) {
    return -1;
  }

  const float* ptr = &_buffer[((y * _width) + x) * 3];
  r = *ptr++;
  g = *ptr++;
  b = *ptr++;
  return 0;
}

int FrameBuffer::range(float& min_val, float& max_val) const
{
  if (_buffer.empty()) {
    return -1;
  }

  auto itr = _buffer.begin(), end = _buffer.end();
  float low  = *itr;
  float high = *itr++;

  for (; itr != end; ++itr) {
    low  = std::min(low,  *itr);
    high = std::max(high, *itr);
  }

  min_val = low;
  max_val = high;
  return 0;
}
