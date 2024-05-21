#ifndef GLACEON_GLACEON_PROFILER_TIMER_H_
#define GLACEON_GLACEON_PROFILER_TIMER_H_

#include <chrono>

#define PROFILING 1
#if PROFILING
// use line number as name to avoid collisions
#define GLACEON_PROFILE_SCOPE(name) glaceon::InstrumentationTimer timer##__LINE__(name);
// prints out the function
#define GLACEON_PROFILE_FUNCTION() GLACEON_PROFILE_SCOPE(__PRETTY_FUNCTION__)
#else
#define GLACEON_PROFILE_SCOPE(name)
#endif

namespace glaceon {

class InstrumentationTimer {
 public:
  InstrumentationTimer(const char* name);
  ~InstrumentationTimer();

 private:
  void Stop();
  const char* name_;
  std::chrono::time_point<std::chrono::high_resolution_clock> start_time_;
  std::chrono::time_point<std::chrono::high_resolution_clock> end_time_;
};

}// namespace glaceon

#endif// GLACEON_GLACEON_PROFILER_TIMER_H_