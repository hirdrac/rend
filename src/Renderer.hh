//
// Renderer.hh
// Copyright (C) 2022 Richard Bradley
//

#pragma once
#include "JobState.hh"
#include "Types.hh"
#include <condition_variable>
#include <mutex>
#include <thread>
#include <vector>


// **** Types ****
class Scene;
class FrameBuffer;

class Renderer
{
 public:
  int init(const Scene* s, FrameBuffer* fb);
  void render(JobState& js, int min_x, int min_y, int max_x, int max_y);

  // jobs/task methods
  [[nodiscard]] int jobs() const { return int(_jobs.size()); }
  void setJobs(int jobs);
    // number of jobs (thread) to execute render

  void startJobs();
    // creates render tasks and starts all render jobs

  int waitForJobs(int timeout_ms);
    // waits for jobs to finish task or timeout
    // returns number of pending tasks

  void stopJobs();
    // stops active jobs, returns once all jobs are halted

  void setStats(const StatInfo& st) { _stats = st; }
  [[nodiscard]] const StatInfo& stats() const { return _stats; }

 private:
  const Scene* _scene = nullptr;
  FrameBuffer* _fb = nullptr;
  std::vector<Vec2> _samples;
  StatInfo _stats;

  // Calculated Data
  Vec3 _vnormal{INIT_ZERO}, _vcenter{INIT_ZERO};
  Vec3 _pixelX{INIT_ZERO}, _pixelY{INIT_ZERO};
  Vec3 _apertureX{INIT_ZERO}, _apertureY{INIT_ZERO};

  // jobs/task stuff
  struct alignas(64) Job {
    // thread local stuff
    // HitInfo pool
    std::thread jobThread;
    JobState state;
    bool halt = false;
  };
  std::vector<Job> _jobs;

  struct Task {
    // image region to render for task
    int min_x, min_y, max_x, max_y;
  };
  std::vector<Task> _tasks;
  std::mutex _tasksMutex;
  std::condition_variable _tasksCV;

  void jobMain(Job* j);
};
