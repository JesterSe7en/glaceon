#include "InstrumentationTimer.h"

#include "../Core/Logger.h"

namespace glaceon {

// "name" here is the function name if using GLACEON_PROFILE_FUNCTION()
InstrumentationTimer::InstrumentationTimer(const char* name) : name_(name) { start_time_ = std::chrono::high_resolution_clock::now(); }

InstrumentationTimer::~InstrumentationTimer() { Stop(); }

void InstrumentationTimer::Stop() {
  end_time_ = std::chrono::high_resolution_clock::now();
  long long duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time_ - start_time_).count();
  GTRACE("@ {} = InstrumentationTimer: {} us - {} ms", name_, duration, duration * 0.001);
}

}// namespace glaceon