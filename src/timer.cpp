#include "timer.h"

#include <algorithm>
#include <utility>

using namespace std;
//--------------------------------------------------------------------------------

auto timers() -> TimerManager& {
  static TimerManager timers;
  return timers;
}

std::vector<FunctionTiming> TimerProfiling::s_functionTimings;


auto operator<(const FunctionTiming& a, const FunctionTiming& b) -> GBool {
  if(a.getDeltaCpuTime() == b.getDeltaCpuTime()) {
    return (a.getInitCpuTime() < b.getInitCpuTime());
  }
  return (a.getDeltaCpuTime() < b.getDeltaCpuTime());
}

TimerProfiling::~TimerProfiling() {
  const clock_t exitCpuTime         = cpuTime();
  const GDouble exitWallTime        = wallTime();
  const GDouble thresholdPercentage = 0.5;
  stringstream  sstream;
  sstream << "    CPU      WALL   FUNCTION                    >> profile: '" << m_name << "' <<";
  const string header = sstream.str();
  for(std::size_t i = 0; i < header.size(); i++) {
    gridgen_log << "_";
  }
  gridgen_log << endl;
  gridgen_log << header << endl;
  for(std::size_t i = 0; i < header.size(); i++) {
    gridgen_log << "-";
  }
  gridgen_log << endl;
  GInt counter    = 0;
  GInt supCounter = 0;
  if(!s_functionTimings.empty()) {
    sort(s_functionTimings.begin(), s_functionTimings.end());
    reverse(s_functionTimings.begin(), s_functionTimings.end());
    for(auto& s_functionTiming : s_functionTimings) {
      if(s_functionTiming.getInitCpuTime() < m_initCpuTime) {
        continue;
      }
      const GDouble relCpuTime = 100.0 * getCpuTimeSecs(s_functionTiming.getDeltaCpuTime())
                                 / max(1e-15, getCpuTimeSecs(exitCpuTime - m_initCpuTime));
      const GDouble relWallTime =
          100.0 * s_functionTiming.getDeltaWallTime() / max(1e-15, (exitWallTime - m_initWallTime));
      if(relCpuTime < thresholdPercentage) {
        supCounter++;
        continue;
      }
      char buffer[7];
      sprintf(buffer, "%6.2f", relCpuTime);
      char buffer2[7];
      sprintf(buffer2, "%6.2f", relWallTime);
      gridgen_log << buffer << "%   " << buffer2 << "%   " << s_functionTiming.getName() << endl;
      counter++;
    }
    if(supCounter > 0) {
      gridgen_log << "  .....     .....   (" << supCounter << " shorter timings with CPU<" << thresholdPercentage
                  << "% were suppressed)" << endl;
    }
  }
  if(counter == 0) {
    gridgen_log << "No timings recorded for timer '" << m_name << "'." << endl;
  }
  for(std::size_t i = 0; i < header.size(); i++) {
    gridgen_log << "-";
  }
  gridgen_log << endl;
  gridgen_log << "Total cpu time:  " << printTime(getCpuTimeSecs(exitCpuTime - m_initCpuTime)) << endl;
  gridgen_log << "Total wall time: " << printTime(exitWallTime - m_initWallTime) << endl;
  for(std::size_t i = 0; i < header.size(); i++) {
    gridgen_log << "_";
  }
  gridgen_log << endl;
}

auto TimerProfiling::getTimingId(const string& name) -> GInt {
  GInt tId = -1;
  if(!s_functionTimings.empty()) {
    for(std::vector<FunctionTiming>::size_type i = 0; i < s_functionTimings.size(); i++) {
      if(s_functionTimings[i].getName() == name) {
        tId = i;
      }
    }
  }
  if(tId < 0) {
    tId = static_cast<GInt>(s_functionTimings.size());
    s_functionTimings.emplace_back(name);
  }
  ASSERT(tId > -1, "Non-existing timer");
  return tId;
}

auto TimerProfiling::printTime(GDouble secs) -> GString {
  stringstream time;
  time.str("");
  GDouble rem = secs;
  if(rem > 86400.0) {
    const GDouble div = floor(rem / 86400.0);
    time << (static_cast<GInt>(div)) << " days, ";
    rem -= div * 86400.0;
  }
  if(rem > 3600.0) {
    const GDouble div = floor(rem / 3600.0);
    time << (static_cast<GInt>(div)) << " hours, ";
    rem -= div * 3600.0;
  }
  if(rem > 60.0) {
    const GDouble div = floor(rem / 60.0);
    time << (static_cast<GInt>(div)) << " mins, ";
    rem -= div * 60.0;
  }
  time << rem << " secs";
  const GString ret = time.str();
  return ret;
}


FunctionTiming::FunctionTiming(string name)
  : m_initCpuTime(cpuTime()),
    m_deltaCpuTime(0),
    m_tmpCpuTime(0),
    m_initWallTime(wallTime()),
    m_deltaWallTime(0.0),
    m_tmpWallTime(-1.0),
    m_name(std::move(name)) {}

FunctionTiming::~FunctionTiming() { m_name = "<deleted>"; }

FunctionTiming& FunctionTiming::operator=(const FunctionTiming& t) {
  m_initCpuTime   = t.m_initCpuTime;
  m_deltaCpuTime  = t.m_deltaCpuTime;
  m_tmpCpuTime    = t.m_tmpCpuTime;
  m_initWallTime  = t.m_initWallTime;
  m_deltaWallTime = t.m_deltaWallTime;
  m_tmpWallTime   = t.m_tmpWallTime;
  m_name          = t.m_name;
  return *this;
}

void FunctionTiming::in() {
  m_tmpCpuTime  = cpuTime();
  m_tmpWallTime = wallTime();
}

void FunctionTiming::out() {
  if(m_tmpCpuTime > 0) {
    m_deltaCpuTime += (cpuTime() - m_tmpCpuTime);
  }
  if(m_tmpWallTime > 0.0) {
    m_deltaWallTime += (wallTime() - m_tmpWallTime);
  }
  m_tmpCpuTime  = -1;
  m_tmpWallTime = -1.0;
}
