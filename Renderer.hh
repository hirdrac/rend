//
// Renderer.hh - revision 1 (2019/1/10)
// Copyright(C) 2019 by Richard Bradley
//

#ifndef Renderer_hh
#define Renderer_hh

#include "Intersect.hh"
#include "Types.hh"
#include <condition_variable>
#include <mutex>
#include <thread>
#include <vector>
//#include <atomic>


// **** Types ****
class Scene;
class FrameBuffer;

class Renderer
{
 public:
  int init(Scene* s, FrameBuffer* fb);
  int render(int min_x, int min_y, int max_x, int max_y,
	     DList<HitInfo>* freeCache = nullptr);

  // jobs/task methods
  int jobs() const { return _jobs.size(); }
  int setJobs(int jobs);
    // number of jobs (thread) to execute render

  int startJobs();
    // creates render tasks and starts all render jobs

  int waitForJobs(int timeout_ms);
    // waits for jobs to finish task or timeout
    // returns number of pending tasks

  int stopJobs();
    // stops active jobs, returns once all jobs are halted

 private:
  Scene* _scene;
  FrameBuffer* _fb;
  std::vector<Vec2> _samples;

  // Calculated Data
  Vec3 _pixelX, _pixelY, _rayDir;

  // jobs/task stuff
  struct Job {
    // thread local stuff
    // HitInfo pool
    std::thread jobThread;
    DList<HitInfo> hitCache;
    volatile bool halt = false;
  };
  std::vector<Job> _jobs;

  struct Task {
    // image region to render for task
    int min_x, min_y, max_x, max_y;
  };
  std::vector<Task> _tasks;
  std::mutex _tasksMutex;
  std::condition_variable _tasksCV;
  //std::atomic<int> _jobsRunning;

  void jobMain(Job* j);
  //void jobMain2(Job* j, int start_y, int inc_y);
};

#endif
