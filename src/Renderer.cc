//
// Renderer.cc
// Copyright (C) 2023 Richard Bradley
//

#include "Renderer.hh"
#include "Scene.hh"
#include "FrameBuffer.hh"
#include "RandomGen.hh"
#include "Ray.hh"
#include "Color.hh"
#include "Print.hh"
#include <chrono>
#include <algorithm>
#include <cassert>


// **** Renderer class ****
int Renderer::init(const Scene* s, FrameBuffer* fb)
{
  _scene = s;
  _fb = fb;
  _stats = {};

  // Set up view vectors
  _vnormal = UnitVec(_scene->coi - _scene->eye);
  const Vec3 vup = UnitVec(_scene->vup);
  const Flt vnvup_dot = DotProduct(_vnormal, vup);
  if (IsOne(Abs(vnvup_dot))) {
    println_err("Bad VUP vector");
    return -1;
  }

  const Vec3 vtop = UnitVec(vup - (_vnormal * vnvup_dot));
  const Vec3 vside = UnitVec(CrossProduct(_vnormal, vtop));
  //Vec3 vside = UnitVec(CrossProduct(vtop, _vnormal)); // right-handed coords

  // Default Left-Handed Coords
  // +Y
  //  |  +Z
  //  | /
  //  |/
  //  O--- +X

  // Calculate Screen/Pixel vectors
  const Flt imgW = Flt(s->image_width);
  const Flt imgH = Flt(s->image_height);
  const Flt focalLen = IsPositive(_scene->aperture) ? s->focus : 1.0;
  const Flt ss = std::tan(DegToRad(s->fov * .5)) * focalLen;
  const Flt screenHeight = ss;
  const Flt screenWidth = ss * (imgW / imgH);
  // FIXME: assumes width > height for fov calc
  _pixelX = (vside * screenWidth) / (imgW * .5);
  _pixelY = (vtop * screenHeight) / (imgH * .5);
  _vcenter = s->eye + (_vnormal * focalLen);
  _apertureX = vside * s->aperture;
  _apertureY = vtop * s->aperture;

  // init frame buffer
  _fb->init(s->image_width, s->image_height);

  // setup sample points
  const int sampleX = std::max(s->sample_x, 1);
  const int sampleY = std::max(s->sample_y, 1);

  _samples.clear();
  _samples.reserve(std::size_t(sampleX * sampleY));
  for (int y = 0; y < sampleY; ++y) {
    for (int x = 0; x < sampleX; ++x) {
      const Vec2 pt{(Flt(x)+.5) / Flt(sampleX), (Flt(y)+.5) / Flt(sampleY)};
      _samples.push_back(pt);
    }
  }

  return 0;
}

void Renderer::render(JobState& js, int min_x, int min_y, int max_x, int max_y)
{
  const Flt halfWidth = Flt(_scene->image_width) * .5;
  const Flt halfHeight = Flt(_scene->image_height) * .5;
  const Vec3 eye = _scene->eye;

  const bool use_jitter = IsPositive(_scene->jitter);
  const bool use_aperture = IsPositive(_scene->aperture);
  const int jitterCount =
    use_jitter || use_aperture ? std::max(_scene->samples, 1) : 1;
  const auto samplesInv = static_cast<Color::value_type>(
    1.0 / double(int(_samples.size()) * jitterCount));

  Ray initRay {
    .base = eye,
    .min_length = 0,
    .max_length = VERY_LARGE,
    .time = 0.0,
    .depth = 0
  };

  // start rendering
  for (int y = min_y; y <= max_y; ++y) {
    const Flt yy = Flt(y) - halfHeight;
    for (int x = min_x; x <= max_x; ++x) {
      const Flt xx = Flt(x) - halfWidth;

      Color c{colors::black};
      for (int i = 0; i < jitterCount; ++i) {
        for (const Vec2& pt : _samples) {
          Flt sx = xx + pt.x;
          Flt sy = yy + pt.y;
          if (use_jitter) {
            sx += js.rndJitterX();
            sy += js.rndJitterY();
          }

          Vec3 dir = (_pixelX * sx) + (_pixelY * sy);
          if (use_aperture) {
            const Vec2 r = js.rndAperturePt();
            initRay.base = eye + (_apertureX * r.x) + (_apertureY * r.y);
            dir += _vcenter - initRay.base;
          } else {
            dir += _vnormal;
          }

          initRay.dir = UnitVec(dir);
          c += _scene->traceRay(js, initRay);
        }
      }

      c *= samplesInv;
      _fb->plot(x, y, c);
    }
  }
}

void Renderer::setJobs(int jobs)
{
  if (jobs < 0) { jobs = 0; }
  _jobs.resize(std::size_t(jobs));
}

void Renderer::startJobs()
{
  assert(!_jobs.empty());

  // make render tasks
  const int num = std::max(jobs(), 4) * 20;
  const int height = _scene->region_max[1] - _scene->region_min[1];
  const int inc_y = std::clamp(height / num, 1, 16);
  const int max_y = _scene->region_max[1];
  for (int y = _scene->region_min[1]; y <= max_y; y += inc_y) {
    const int yend = std::min(y + inc_y - 1, max_y);
    _tasks.push_back({_scene->region_min[0], y, _scene->region_max[0], yend});
  }

  println("Jobs: ", jobs(), "   Tasks: ", _tasks.size(),
          "   Task Size: ", _scene->image_width, "x", inc_y);

  // start render jobs
  for (auto& j : _jobs) {
    j.state.init(*_scene);
    j.halt = false;
    j.jobThread = std::thread{&Renderer::jobMain, this, &j};
  }
}

int Renderer::waitForJobs(int timeout_ms)
{
  const std::chrono::milliseconds timeout{timeout_ms};
  std::unique_lock lock{_tasksMutex};
  while (!_tasks.empty()) {
    if (_tasksCV.wait_for(lock, timeout) == std::cv_status::timeout) {
      return int(_tasks.size()); // timeout
    }
  }
  return 0; // done
}

void Renderer::stopJobs()
{
  for (auto& j : _jobs) { j.halt = true; }
  for (auto& j : _jobs) {
    j.jobThread.join();
    _stats += j.state.stats;
  }
}

void Renderer::jobMain(Job* j)
{
  Task t;
  while (!j->halt) {
    {
      std::lock_guard lock{_tasksMutex};
      if (_tasks.empty()) {
	j->halt = true;
	_tasksCV.notify_one();
	break;
      }

      t = _tasks.back();
      _tasks.pop_back();
    }

    render(j->state, t.min_x, t.min_y, t.max_x, t.max_y);
  }
}
