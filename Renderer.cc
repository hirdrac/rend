//
// Renderer.cc - revision 1 (2019/1/10)
// Copyright(C) 2019 by Richard Bradley
//

#include "Renderer.hh"
#include "Scene.hh"
#include "FrameBuffer.hh"
#include "Ray.hh"
#include "Color.hh"
#include "Logger.hh"
#include <iostream>
#include <chrono>
#include <algorithm>


// **** Renderer class ****
int Renderer::init(Scene* s, FrameBuffer* fb)
{
  _scene = s;
  _fb = fb;

  // Set up view vectors
  Vec3 vnormal = UnitVec(_scene->coi - _scene->eye);
  Vec3 vup = UnitVec(_scene->vup);
  if (IsZero(DotProduct(vnormal, vup) - 1.0)) {
    LOG_ERROR("Bad VUP vector");
    return -1;
  }

  Vec3 vtop = UnitVec(vup - (vnormal * DotProduct(vnormal, vup)));
  Vec3 vside = UnitVec(CrossProduct(vnormal, vtop));

  // Calculate Screen/Pixel vectors
  Flt ss = std::tan((_scene->fov * .5) * MathVal<Flt>::DEG_TO_RAD);
  Flt screenHeight = ss;
  Flt screenWidth = ss * (Flt(_scene->image_width) / Flt(_scene->image_height));
  Flt screenDistance = 1.0;
  _pixelX = vside * screenWidth;
  _pixelY = vtop * screenHeight;
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
		     DList<HitInfo>* freeCache)
{
  Vec3 px = _pixelX, py = _pixelY, rd = _rayDir;
  Flt halfWidth  = Flt(_scene->image_width)  * .5;
  Flt halfHeight = Flt(_scene->image_height) * .5;

  Ray initRay;
  initRay.base = _scene->eye;
  initRay.max_length = VERY_LARGE;
  initRay.depth = 0;
  initRay.time = 0.0;
  initRay.freeCache = freeCache;

  // start rendering
  for (int y = min_y; y <= max_y; ++y) {
    Flt yy = Flt(y) - halfHeight;
    for (int x = min_x; x <= max_x; ++x) {
      Flt xx = Flt(x) - halfWidth;

      Color c = Color::black;
      for (const Vec2& pt : _samples) {
        Vec3 tx = px * ((xx + pt.x) / halfWidth);
        Vec3 ty = py * ((yy + pt.y) / halfHeight);
        initRay.dir = UnitVec(rd + tx + ty);

	Color result;
	_scene->traceRay(initRay, result);
        c += result;
      }

      c /= _samples.size();
      Flt cr = 0, cg = 0, cb = 0;
      c.getRGB(cr, cg, cb);
      _fb->plot(x, y, cr, cg, cb);
    }
  }

  return 0;
}

int Renderer::setJobs(int jobs)
{
  _jobs.resize(jobs);
  return 0;
}

int Renderer::startJobs()
{
#if 1
  // make render tasks
  int max_y = _scene->image_height - 1;
  int inc_y = std::max(2, int(_scene->image_height / (_jobs.size() * 10)));
  for (int y = 0; y <= max_y; y += inc_y) {
    int yy = std::min(y + inc_y - 1, max_y);
    _tasks.push_back({0, y, _scene->image_width - 1, yy});
  }
  std::cout << "tasks: " << _tasks.size() << "   size: " << inc_y << '\n';

  // start render jobs
  for (Job& j : _jobs) {
    j.halt = false;
    j.jobThread = std::thread(&Renderer::jobMain, this, &j);
  }
#else
  // start render jobs (alternating scanlines for each job)
  int y = 0;
  _jobsRunning = _jobs.size();
  for (Job& j : _jobs) {
    j.halt = false;
    j.jobThread = std::thread(&Renderer::jobMain2, this, &j, y++, _jobs.size());
  }
#endif
  return 0;
}

int Renderer::waitForJobs(int timeout_ms)
{
  std::chrono::milliseconds timeout(timeout_ms);
  std::unique_lock<std::mutex> lock(_tasksMutex);
  //_tasksCV.wait(lock);
  //return _tasks.size();

  //if (_tasksCV.wait_for(lock, timeout, [&]{return _tasks.empty();})) {
  //  return 0; // done;
  //} else {
  //  return _tasks.size(); // timeout
  //}

#if 1
  while (!_tasks.empty()) {
    if (_tasksCV.wait_for(lock, timeout) == std::cv_status::timeout) {
      return _tasks.size(); // timeout
    }
  }
#else
  while (_jobsRunning > 0) {
    if (_tasksCV.wait_for(lock, timeout) == std::cv_status::timeout) {
      return 1; // timeout
    }
  }
#endif

  return 0; // done
}

int Renderer::stopJobs()
{
  for (Job& j : _jobs) { j.halt = true; }
  for (Job& j : _jobs) { j.jobThread.join(); }
  return 0;
}

void Renderer::jobMain(Job* j)
{
  Task t;
  while (!j->halt) {
    {
      std::lock_guard<std::mutex> lock(_tasksMutex);
      if (_tasks.empty()) {
	j->halt = true;
	_tasksCV.notify_one();
	break;
      }

      t = _tasks.back();
      _tasks.pop_back();
    }

    render(t.min_x, t.min_y, t.max_x, t.max_y, &j->hitCache);
  }
  //std::cerr << "thread halt\n";
}

#if 0
void Renderer::jobMain2(Job* j, int start_y, int inc_y)
{
  int x_max = _scene->image_width - 1;
  int y_max = _scene->image_height - 1;
  for (int y = start_y; y <= y_max; y += inc_y) {
    render(0, y, x_max, y, &j->hitCache);
  }

  j->halt = true;
  --_jobsRunning;
  _tasksCV.notify_one();
}
#endif
