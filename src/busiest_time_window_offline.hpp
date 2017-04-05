////////////////////////////////////////////////////////////////////////////////
// Feature 3: N busiest in time window
// Caution: time window can start anywhere but not limited to the time points in
//          the log file.
// Offline calculation:
//   1. Use min-heap to keep track of N busiest periods outside time window.
//   2. Use a queue to keep track of periods within time window.
//   3. Use a fake time that is twice the time window later than the last line
//      to pop the elements in queue into min-heap.
////////////////////////////////////////////////////////////////////////////////

#ifndef BUSIEST_TIME_WINDOW_OFFLINE_HPP
#define BUSIEST_TIME_WINDOW_OFFLINE_HPP

#include "stat.hpp"
#include <queue>

class BusiestTimeWindowOffline : public Stat
{
public:
//  using Duration = std::chrono::duration<std::chrono::system_clock::rep>;
  using Duration = LogEntry::Duration;
  explicit BusiestTimeWindowOffline(size_t N, const Duration &window);
  void processLine(const LogEntry &) override;
  std::string getStat() noexcept override;

private:
  void init(const LogEntry &);
  void finalizeOldTimePoint(const LogEntry::TimePoint &time);

private:
  struct TimeHit {
    size_t hits;
    std::string timeStr;
    TimeHit(size_t h, const LogEntry::TimePoint &time,
            const std::string &timezoneStr);
    bool operator<(const TimeHit &other) const noexcept
    {
      return hits == other.hits ? timeStr < other.timeStr : hits > other.hits;
    }
  };


private:
  bool isInitialized_;
  LogEntry::TimePoint initialTime_, prevTime_;
  std::string timezoneStr_;
  const size_t N_;
  const Duration timeWindow_;
  // min heap for finalized time points which are outside of 1 hour window
  std::priority_queue<TimeHit> minHeap_;
  // queue for active time points which are inside the 1 hour window
  // we keep track of number of duplicated time point so the queue won't be
  // filled with duplicated time points
  std::queue<std::pair<LogEntry::TimePoint, size_t>> activeQueue_;
  // actual number of hit
  // actual length of activeQueue if consider duplicates.
  std::size_t hits_;
};

////////////////////////////////////////////////////////////////////////////////
// inline implementations
////////////////////////////////////////////////////////////////////////////////

inline BusiestTimeWindowOffline::TimeHit::TimeHit(
    size_t h, const LogEntry::TimePoint &time, const std::string &timezoneStr)
    : timeStr()
    , hits(h)
{
  // construct the string representation of time: timeStr
  char tmp[21];
  std::time_t t(std::chrono::system_clock::to_time_t(time));
  // system_clock take into account the time zone in runtime
  std::strftime(tmp, 21, "%d/%b/%G:%H:%M:%S", std::localtime(&t));
  timeStr.assign(tmp);
  timeStr += timezoneStr;
}

////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////

#endif
