#ifndef GRIDGENERATOR_TIMER_H
#define GRIDGENERATOR_TIMER_H


#include <chrono>
#include <cmath>
#include <ctime>
#include <iomanip>
#include <string>
#include <sys/times.h>
#include <unistd.h>
#include <utility>
#include <vector>
#include "constants.h"
#include "macros.h"
#include "math/mathfunctions.h"
#include "types.h"


#define NEW_TIMER_GROUP(id, groupName) const GInt id = timers().newGroup(groupName)
#define NEW_TIMER(id, timerName, groupId) const GInt id = timers().newGroupTimer(timerName, groupId)
#define NEW_SUB_TIMER(id, timerName, timerId) const GInt id = timers().newSubTimer(timerName, timerId)
#define NEW_TIMER_GROUP_STATIC(id, groupName) static const GInt id = timers().newGroup(groupName)
#define NEW_TIMER_STATIC(id, timerName, groupId) static const GInt id = timers().newGroupTimer(timerName, groupId)
#define NEW_SUB_TIMER_STATIC(id, timerName, timerId) static const GInt id = timers().newSubTimer(timerName, timerId)
#define NEW_TIMER_GROUP_NOCREATE(id, groupName) id = timers().newGroup(groupName)
#define NEW_TIMER_NOCREATE(id, timerName, groupId) id = timers().newGroupTimer(timerName, groupId)
#define NEW_SUB_TIMER_NOCREATE(id, timerName, timerId) id = timers().newSubTimer(timerName, timerId)
#define START_TIMER(timerId) timers().startTimer(timerId)
#define RECORD_TIMER_START(timerId) timers().recordTimerStart(timerId, AT_)
#define RECORD_TIMER_STOP(timerId) timers().recordTimerStop(timerId, AT_)
#define RETURN_TIMER(timerId) timers().returnTimer(timerId)
#define RETURN_TIMER_TIME(timerId) timers().returnTimerTime(timerId)
#define STOP_TIMER(timerId) timers().stopTimer(timerId)
#define STOP_ALL_TIMERS() timers().stopAllTimers()
#define RECORD_TIMER(timerId) timers().recordTimer(timerId)
#define RECORD_TIMERS() timers().recordTimers()
#define RECORD_ALL_TIMER() timers().recordAllTimer()
#define STOP_ALL_RECORD_TIMERS() timers().stopAllRecordTimers()
#define DISPLAY_TIMER(timerId) timers().displayTimer(timerId)
#define DISPLAY_TIMER_INTERM(timerId) timers().displayTimerNoToggleDisplayed(timerId)
#define DISPLAY_TIMER_OFFSET(timerId, ivl)                                                                             \
  if(globalTimeStep % ivl == 0) {                                                                                      \
    timers().recordTimerStop(timerId);                                                                                 \
    timers().displayTimerNoToggleDisplayed(timerId);                                                                   \
    timers().recordTimerStart(timerId);                                                                                \
  }
#define DISPLAY_ALL_GROUP_TIMERS(groupId) timers().displayAllTimers(groupId)
#define DISPLAY_ALL_TIMERS() timers().displayAllTimers()
#define RESET_TIMER(timerId) timers().resetTimer(timerId)
#define RESET_TIMERS() timers().resetTimers()
#define RESET_RECORD(timerId) timers().resetRecord(timerId)
#define RESET_ALL_RECORDS() timers().resetRecords()
#define SET_TIMER(timeValue, timerId) timers().setTimer(timeValue, timerId)


/// \brief TimerManager manages all Timers and allows primitive profiling.
///
/// Usage:
/// - NEW_TIMER(string name) creates a new timer with name "name" and
/// returns its index (a GInt that you can use to access it).
/// - RESET_TIMER(GInt timerId): resets timerId.
/// - START_TIMER(timerId)/STOP_TIMER(timerId) work as expected.
/// - DISPLAY_TIMER(timerId) writes the timerId name and time to the log.
/// - DISPLAY_ALL_TIMERS: displays all timers.
///
/// Example:
///
/// RESET_TIMER;
/// GInt myTimer;
/// myTimer = NEW_TIMER("My timer");
///
/// START_TIMER(myTimer);
/// f1(); // Function will be timed.
/// STOP_TIMER(myTimer);
/// f2(); // Function will not be timed.
/// START_TIMER(myTimer);
/// f3(); // Function will be timed.
/// STOP_TIMER(myTimer);
///
/// DISPLAY_TIMER(myTimer);
///
///
using chronoTimePoint = std::chrono::time_point<std::chrono::system_clock, std::chrono::duration<double>>;

class TimerManager {
  friend TimerManager& timers();

 public:
  inline auto newGroup(const std::string groupName) -> GInt;
  inline auto newGroupTimer(const std::string& timerName, const GInt groupId) -> GInt;
  inline auto newSubTimer(const std::string& timerName, const GInt timerId) -> GInt;
  inline auto returnTimer(const GInt timerId) -> GDouble;
  inline auto returnTimerTime(const GInt timerId) -> GDouble;
  inline void startTimer(const GInt timerId, const GString& pos = "");       // start
  inline void stopTimer(const GInt timerId, const GString& pos = "");        // stop
  inline void resetTimer(const GInt timerId);                                // reset
  inline void recordTimer(const GInt timerId);                               // record
  inline void recordTimerStart(const GInt timerId, const GString& pos = ""); // reset + start
  inline void recordTimerStop(const GInt timerId, const GString& pos = "");  // stop + record
  inline void recordTimers();
  inline void stopAllTimers();
  inline void stopAllRecordTimers(); // stop all + record stopped
  inline void displayTimer(const GInt timerId);
  inline void displayTimerNoToggleDisplayed(const GInt timerId);
  inline void displayAllTimers();
  inline void displayAllTimers(const GInt groupId);
  inline void resetTimers();
  inline void resetRecord(const GInt timerId);
  inline void resetRecords();

  // delete: copy construction, and copy assignment
  TimerManager(TimerManager&) = delete;
  TimerManager& operator=(const TimerManager&) = delete;

 private:
  TimerManager()  = default;
  ~TimerManager() = default;


  struct Timer {
    Timer(std::string n, const GInt g, const GInt id, const GInt p)
      : name(std::move(n)),
        group(g),
        timerId(id),
        parent(p),
        recordedTime(0),
        status(Timer::Uninitialized),
        subTimers(0),
        displayed(false) {}
    std::string     name;  ///< Timer Name
    GInt            group; ///< Group Id
    GInt            timerId = -1;
    GInt            parent  = -1; ///< Parent timer id
    chronoTimePoint cpuTime;      ///< CPU time
    chronoTimePoint oldCpuTime;   ///< Old CPU time (for timer restart)
    GDouble         recordedTime; ///< Time recorded on the timer.
    GInt            status;       ///< Timer's status, see enum:
    enum { Uninitialized = 0, Running = 1, Stopped = 0 };
    std::vector<GInt> subTimers{};
    GBool             displayed;
  };

  std::vector<std::string> m_groups;
  std::vector<Timer>       m_timers;
  using TIt = std::vector<Timer>::iterator;


  static inline auto        time() -> chronoTimePoint;
  inline void               displayTimer_(const GInt    timerId,
                                          const GBool   toggleDisplayed = true,
                                          const GInt    tIndent         = 0,
                                          const GDouble superTime       = -1.0);
  inline void               displayTimerHeader_();
  inline void               displayTimerGroupHeader_(const GInt groupId);
  [[nodiscard]] inline GInt indent(const GInt pIndent) const { return pIndent + 2; };
};

auto timers() -> TimerManager&;


inline auto TimerManager::time() -> chronoTimePoint {
  using clock = std::chrono::system_clock;
  return clock::now();
}

inline void TimerManager::resetRecord(const GInt timerId) { m_timers[timerId].recordedTime = 0.0; }

inline void TimerManager::resetRecords() {
  for(std::size_t timerId = 0, e = m_timers.size(); timerId != e; ++timerId) {
    resetRecord(static_cast<GInt>(timerId));
  }
}

inline void TimerManager::recordTimerStop(const GInt timerId, const GString& pos) {
  if(timerId < 0) {
    return;
  }
#ifdef TIMER_SYNC
  MPI_Barrier(MPI_COMM_WORLD);
#endif
  stopTimer(timerId, pos);
  recordTimer(timerId);
}

inline void TimerManager::recordTimer(const GInt timerId) { m_timers[timerId].recordedTime += returnTimer(timerId); }
inline void TimerManager::recordTimers() {
  for(std::size_t timerId = 0, e = m_timers.size(); timerId != e; ++timerId) {
    recordTimer(timerId);
  }
}

inline void TimerManager::resetTimer(const GInt timerId) { m_timers[timerId].status = Timer::Stopped; }

inline void TimerManager::resetTimers() {
  for(std::size_t timerId = 0, e = m_timers.size(); timerId != e; ++timerId) {
    resetTimer(timerId);
  }
}

/// Creates a new timer group and returns its groupId.
inline auto TimerManager::newGroup(const std::string name) -> GInt {
  m_groups.push_back(name);
  return m_groups.size() - 1;
}

/// Creates a new timer and returns its timerId.
inline auto TimerManager::newGroupTimer(const std::string& name, const GInt groupId) -> GInt {
  ASSERT(static_cast<std::size_t>(groupId) < m_groups.size() && groupId > -1,
         "groupId: " + std::to_string(groupId) + " does not exists | name: " + name);
  const GInt newTimerId = m_timers.size();
  m_timers.emplace_back(name, groupId, newTimerId, -1);
  return newTimerId;
}

/// Creates a new timer and returns its timerId.
inline auto TimerManager::newSubTimer(const std::string& name, const GInt timerId) -> GInt {
  if(timerId < 0) {
    return -1;
  }

  ASSERT(static_cast<std::size_t>(timerId) < m_timers.size(),
         "timerId " + std::to_string(timerId) + " does not exist when trying to create subtimer with name " + name);

  const GInt groupId    = m_timers[timerId].group;
  const GInt newTimerId = m_timers.size();
  m_timers.emplace_back(name, groupId, newTimerId, timerId);
  m_timers[timerId].subTimers.push_back(newTimerId);
  return newTimerId;
}

inline void TimerManager::recordTimerStart(const GInt timerId, const GString& pos) {
  if(timerId < 0) {
    return;
  }
#ifdef TIMER_SYNC
  MPI_Barrier(MPI_COMM_WORLD);
#endif
  resetTimer(timerId);
  startTimer(timerId, pos);
}

inline void TimerManager::startTimer(const GInt timerId, const GString& pos) {
  ASSERT(m_timers[timerId].status != Timer::Running,
         "The timer " + m_timers[timerId].name + " with id: " + std::to_string(timerId)
             + " can't be started because it is already running! " + pos);
  std::ignore = pos;

  chronoTimePoint t            = time();
  m_timers[timerId].oldCpuTime = m_timers[timerId].cpuTime;
  m_timers[timerId].cpuTime    = t;
  m_timers[timerId].status     = Timer::Running;
}


/// Returns the timer Value.
inline auto TimerManager::returnTimer(const GInt timerId) -> GDouble {
#ifdef TIMER_RANK_AVG
  const GDouble t       = m_timers[timerId].cpuTime.time_since_epoch().count();
  GDouble       tmp_rcv = 0.0;
  MPI_Reduce(&t, &tmp_rcv, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
  return tmp_rcv / MPI::globalNoDomains();
#else
  return m_timers[timerId].cpuTime.time_since_epoch().count();
#endif
}

// Returns the recorded time
inline auto TimerManager::returnTimerTime(const GInt timerId) -> GDouble { return m_timers[timerId].recordedTime; }

/// Stops the timer and sets its final value.
inline void TimerManager::stopTimer(const GInt timerId, const GString& pos) {
  if(timerId < 0) {
    return;
  }

  if(m_timers[timerId].status == Timer::Running) {
    const chronoTimePoint t   = time();
    m_timers[timerId].cpuTime = t - m_timers[timerId].cpuTime + m_timers[timerId].oldCpuTime;
    m_timers[timerId].status  = Timer::Stopped;

    std::ignore = pos;
  } else {
    const GString msg = "The timer '" + m_timers[timerId].name + "' can't be stopped because it is not running! " + pos;
    std::cerr << msg << std::endl;
  }
}


// Stops all timers.
inline void TimerManager::stopAllTimers() {
  for(std::size_t i = 0, e = m_timers.size(); i != e; ++i) {
    if(m_timers[i].status == Timer::Running) {
      stopTimer(i, AT_);
    }
  }
}

// Stops all timers and record the timers that were stopped
inline void TimerManager::stopAllRecordTimers() {
  // for(std::size_t i = 0, e = m_timers.size(); i != e; ++i) {
  for(GInt i = m_timers.size() - 1; i >= 0; i--) {
    if(m_timers[i].status == Timer::Running) {
#ifdef TIMER_SYNC
      MPI_Barrier(MPI_COMM_WORLD);
#endif
      stopTimer(i, AT_);
      recordTimer(i);
    }
  }
}


inline void TimerManager::displayTimer_(const GInt    timerId,
                                        const GBool   toggleDisplayed,
                                        const GInt    tIndent,
                                        const GDouble superTime) {
  GBool running = false;
  if(m_timers[timerId].displayed) {
    return;
  }

  if(m_timers[timerId].status == Timer::Running) {
    running = true;
    stopTimer(timerId, AT_);
  }
  gridgen_log.width(50);
  gridgen_log.setf(std::ios::left);
  std::stringstream indentedName;

  // Calculate time relative to the parent timer
  GDouble percentage = NAN;
  if(superTime < 0.0) {
    // If the parent time is less than zero, that means that there is no parent timer
    // and the percentage should be 100%
    percentage = 100 * 1.0;
  } else if(approx(superTime, 0.0, GDoubleEps)) {
    // If the parent time is approximately zero, that probably means that the timer was never
    // run - therefore the percentage is set to 0%
    percentage = 0.0;
  } else {
    // Otherwise calculate the percentage as the fraction of this timer vs. the parent timer times 100%
    percentage = 100 * m_timers[timerId].recordedTime / superTime;
  }

  indentedName << std::string(tIndent, ' ');
  indentedName << "[" << std::fixed << std::setprecision(1) << std::setw(4) << std::setfill('0') << std::right
               << percentage << std::left << "%] ";
  indentedName << m_timers[timerId].name;
  gridgen_log << indentedName.str() << std::right;
  gridgen_log.precision(6);
  gridgen_log.width(20);
  gridgen_log << m_timers[timerId].recordedTime << std::left << " [sec]";
  if(toggleDisplayed) m_timers[timerId].displayed = true;
  gridgen_log << std::endl;
  for(std::size_t sub = 0, last = m_timers[timerId].subTimers.size(); sub < last; ++sub) {
    const GInt new_indent = indent(tIndent);
    displayTimer_(m_timers[timerId].subTimers[sub], toggleDisplayed, new_indent, m_timers[timerId].recordedTime);
  }
  if(running) {
    startTimer(timerId);
  }
}

inline void TimerManager::displayTimerHeader_() {}

inline void TimerManager::displayTimerGroupHeader_(const GInt groupId) {
  gridgen_log << "--------------------------------------------------------------------------------" << std::endl;
  gridgen_log.width(50);
  gridgen_log.precision(12);
  gridgen_log.setf(std::ios::left);
  gridgen_log << "Group";
  gridgen_log.width(40);
  gridgen_log << m_groups[groupId] << std::endl;
}

inline void TimerManager::displayAllTimers() {
  ASSERT(!m_timers.empty(), "ERROR: no timers have been created!");
  for(std::size_t groupId = 0, e = m_groups.size(); groupId != e; ++groupId) {
    displayAllTimers(groupId);
  }
}

inline void TimerManager::displayAllTimers(const GInt groupId) {
  ASSERT(m_timers.size() > 0, "ERROR: no timers have been created!");
  ASSERT(static_cast<std::size_t>(groupId) < m_groups.size() && groupId > -1, "ERROR: groupId does not exists");
  for(std::size_t timerId = 0, e = m_timers.size(); timerId != e; ++timerId) {
    m_timers[timerId].displayed = false;
  }
  displayTimerGroupHeader_(groupId);
  displayTimerHeader_();
  for(std::size_t timerId = 0, e = m_timers.size(); timerId != e; ++timerId) {
    if(m_timers[timerId].group == groupId) {
      displayTimer_(timerId);
    }
  }
  for(std::size_t timerId = 0, e = m_timers.size(); timerId != e; ++timerId) {
    m_timers[timerId].displayed = false;
  }
}

inline void TimerManager::displayTimer(const GInt timerId) {
  ASSERT(static_cast<std::size_t>(timerId) < m_timers.size(), "ERROR: timer timerId does not exist");
  displayTimerHeader_();
  displayTimer_(timerId);
}

inline void TimerManager::displayTimerNoToggleDisplayed(const GInt timerId) {
  ASSERT(static_cast<std::size_t>(timerId) < m_timers.size(), "ERROR: timer timerId does not exist");
  displayTimerHeader_();
  displayTimer_(timerId, false);
}

class TimerProfiling;
class FunctionTiming;


inline auto cpuTime() -> clock_t { return clock(); }

inline auto wallTime() -> GDouble { return MPI_Wtime(); }

class FunctionTiming {
 public:
  explicit FunctionTiming(std::string name);
  ~FunctionTiming();
  auto               operator=(const FunctionTiming& t) -> FunctionTiming&;
  void               in();
  void               out();
  [[nodiscard]] auto getInitCpuTime() const -> clock_t { return m_initCpuTime; }
  [[nodiscard]] auto getDeltaCpuTime() const -> clock_t { return m_deltaCpuTime; }
  [[nodiscard]] auto getInitWallTime() const -> GDouble { return m_initWallTime; }
  [[nodiscard]] auto getDeltaWallTime() const -> GDouble { return m_deltaWallTime; }
  [[nodiscard]] auto getName() const -> std::string { return m_name; }

 private:
  clock_t     m_initCpuTime;
  clock_t     m_deltaCpuTime;
  clock_t     m_tmpCpuTime;
  GDouble     m_initWallTime;
  GDouble     m_deltaWallTime;
  GDouble     m_tmpWallTime;
  std::string m_name;
};

auto operator<(const FunctionTiming& a, const FunctionTiming& b) -> GBool;


class TimerProfiling {
 public:
  explicit TimerProfiling(std::string name)
    : m_initCpuTime(cpuTime()), m_initWallTime(wallTime()), m_name(std::move(name)) {}
  ~TimerProfiling();
  static auto getTimingId(const std::string& name) -> GInt;
  static auto getCpuTimeSecs(clock_t cput) -> GDouble {
    return (static_cast<GDouble>(cput) / static_cast<GDouble>(CLOCKS_PER_SEC));
  }
  static auto                        printTime(GDouble secs) -> GString;
  static std::vector<FunctionTiming> s_functionTimings;

 private:
  const clock_t     m_initCpuTime;
  const GDouble     m_initWallTime;
  const std::string m_name;
};

#endif // GRIDGENERATOR_TIMER_H
