//
// Renderer.cc
// Copyright (C) 2021 Richard Bradley
//

#include "Renderer.hh"
#include "Scene.hh"
#include "FrameBuffer.hh"
#include "Ray.hh"
#include "Color.hh"
#include "Logger.hh"
#include "Print.hh"
#include <chrono>
#include <algorithm>
#include <cstdlib>


// **** helper functions ****
static inline double rnd_jitter()
{
#if defined(__MINGW32__) || defined(__MINGW64__)
  return (double(rand()) / double(RAND_MAX)) - .5;
#else
  return drand48() - .5; // range [-.5,+.5)
#endif
}


// **** Renderer class ****
int Renderer::init(Scene* s, FrameBuffer* fb)
{
  _scene = s;
  _fb = fb;
  _stats = {};

  // Set up view vectors
  const Vec3 vnormal = UnitVec(_scene->coi - _scene->eye);
  const Vec3 vup = UnitVec(_scene->vup);
  if (IsZero(DotProduct(vnormal, vup) - 1.0)) {
    LOG_ERROR("Bad VUP vector");
    return -1;
  }

  const Vec3 vtop = UnitVec(vup - (vnormal * DotProduct(vnormal, vup)));
  const Vec3 vside = UnitVec(CrossProduct(vnormal, vtop));
  //Vec3 vside = UnitVec(CrossProduct(vtop, vnormal)); // right-handed coords

  // Default Left-Handed Coords
  // +Y
  //  |  +Z
  //  | /
  //  |/
  //  O--- +X

  // Calculate Screen/Pixel vectors
  const Flt imgW = Flt(_scene->image_width);
  const Flt imgH = Flt(_scene->image_height);
  const Flt ss = std::tan((_scene->fov * .5) * math::DEG_TO_RAD<Flt>);
  const Flt screenHeight = ss;
  const Flt screenWidth = ss * (imgW / imgH);
  const Flt screenDistance = 1.0;
  _pixelX = (vside * screenWidth) / (imgW * .5);
  _pixelY = (vtop * screenHeight) / (imgH * .5);
  _rayDir = vnormal * screenDistance;

  // init frame buffer
  _fb->init(_scene->image_width, _scene->image_height);

  // setup sub-pixel samples
  // (uniform pixel sub-sampling for now)
  _samples.clear();
  for (int y = 0; y < _scene->samples_y; ++y) {
    for (int x = 0; x < _scene->samples_x; ++x) {
      _samples.push_back({(Flt(x)+.5) / Flt(_scene->samples_x),
			  (Flt(y)+.5) / Flt(_scene->samples_y)});
    }
  }

  return 0;
}

int Renderer::render(int min_x, int min_y, int max_x, int max_y,
		     HitCache* freeCache, StatInfo* stats)
{
  const Vec3 px = _pixelX, py = _pixelY, rd = _rayDir;
  const Flt halfWidth = Flt(_scene->image_width) * .5;
  const Flt halfHeight = Flt(_scene->image_height) * .5;
  const auto samples_inv =
    static_cast<Color::value_type>(1.0 / double(_samples.size()));

  const Flt jitterX = _scene->jitter / Flt(_scene->samples_x);
  const Flt jitterY = _scene->jitter / Flt(_scene->samples_y);
  const bool use_jitter = IsPositive(_scene->jitter);

  Ray initRay;
  initRay.base = _scene->eye;
  initRay.min_length = 0;
  initRay.max_length = VERY_LARGE;
  initRay.time = 0.0;
  initRay.depth = 0;
  initRay.freeCache = freeCache;
  initRay.stats = stats ? stats : &_stats;

  // start rendering
  for (int y = min_y; y <= max_y; ++y) {
    const Flt yy = Flt(y) - halfHeight;
    for (int x = min_x; x <= max_x; ++x) {
      const Flt xx = Flt(x) - halfWidth;

      Color c{colors::black};
      for (const Vec2& pt : _samples) {
        Flt sx = xx + pt.x;
        Flt sy = yy + pt.y;
        if (use_jitter) {
          sx += rnd_jitter() * jitterX;
          sy += rnd_jitter() * jitterY;
        }
        initRay.dir = rd + (px * sx) + (py * sy);
          // dir not normalized for performance

        c += _scene->traceRay(initRay);
      }

      c *= samples_inv;
      _fb->plot(x, y, c);
    }
  }

  return 0;
}

int Renderer::setJobs(int jobs)
{
  if (jobs < 0) { jobs = 0; }

  std::size_t oldSize = _jobs.size();
  _jobs.resize(std::size_t(jobs));
  std::size_t newSize = _jobs.size();
  if (newSize > oldSize) {
    for (std::size_t i = oldSize; i < newSize; ++i) {
      _jobs[i] = std::make_unique<Job>();
    }
  }
  return 0;
}

int Renderer::startJobs()
{
  // make render tasks
  int num = std::max(jobs(), 4) * 20;
  int inc_y = std::clamp(_scene->image_height / num, 2, 16);
  int max_y = _scene->image_height - 1;
  for (int y = 0; y <= max_y; y += inc_y) {
    int yy = std::min(y + inc_y - 1, max_y);
    _tasks.push_back({0, y, _scene->image_width - 1, yy});
  }

  println("Jobs: ", jobs(), "   Tasks: ", _tasks.size(),
          "   Task Size: ", _scene->image_width, "x", inc_y);

  // start render jobs
  for (auto& j : _jobs) {
    j->stats = {};
    j->halt = false;
    j->jobThread = std::thread(&Renderer::jobMain, this, j.get());
  }

  return 0;
}

int Renderer::waitForJobs(int timeout_ms)
{
  std::chrono::milliseconds timeout(timeout_ms);
  std::unique_lock lock(_tasksMutex);
  while (!_tasks.empty()) {
    if (_tasksCV.wait_for(lock, timeout) == std::cv_status::timeout) {
      return int(_tasks.size()); // timeout
    }
  }
  return 0; // done
}

int Renderer::stopJobs()
{
  for (auto& j : _jobs) { j->halt = true; }
  for (auto& j : _jobs) {
    j->jobThread.join();
    _stats += j->stats;
  }
  return 0;
}

void Renderer::jobMain(Job* j)
{
  Task t;
  while (!j->halt) {
    {
      std::lock_guard lock(_tasksMutex);
      if (_tasks.empty()) {
	j->halt = true;
	_tasksCV.notify_one();
	break;
      }

      t = _tasks.back();
      _tasks.pop_back();
    }

    render(t.min_x, t.min_y, t.max_x, t.max_y, &j->hitCache, &j->stats);
  }
}