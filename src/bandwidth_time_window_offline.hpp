////////////////////////////////////////////////////////////////////////////////
// Feature 5: N most bandwidth demanding time window
// Caution: time window can start anywhere but not limited to the time points in
//          the log file.
////////////////////////////////////////////////////////////////////////////////

#ifndef BANDWIDTH_TIME_WINDOW_OFFLINE_HPP
#define BANDWIDTH_TIME_WINDOW_OFFLINE_HPP

#include "stat.hpp"
#include <queue>

class BandwidthTimeWindowOffline : public Stat
{
public:
//  using Duration = std::chrono::duration<std::chrono::system_clock::rep>;
  using Duration = LogEntry::Duration;
  explicit BandwidthTimeWindowOffline(size_t N, const Duration &window);
  void processLine(const LogEntry &) override;
  std::string getStat() override;

private:
  void init(const LogEntry &);
  void finalizeOldTimePoint(const LogEntry::TimePoint &time);

private:
  struct TimeByte {
    size_t bytes;
    std::string timeStr;
    TimeByte(size_t h, const LogEntry::TimePoint &time,
            const std::string &timezoneStr);
    bool operator<(const TimeByte &other) const noexcept
    {
      return bytes == other.bytes ? timeStr < other.timeStr
                                  : bytes > other.bytes;
    }
  };


private:
  bool isInitialized_;
  LogEntry::TimePoint initialTime_, prevTime_;
  std::string timezoneStr_;
  const size_t N_;
  const Duration timeWindow_;
  // min heap for finalized time points which are outside of 1 hour window
  std::priority_queue<TimeByte> minHeap_;
  // queue for active time points which are inside the 1 hour window
  // we keep track of number of duplicated time point so the queue won't be
  // filled with duplicated time points
  std::queue<std::pair<LogEntry::TimePoint, size_t>> activeQueue_;
  // actual number of hit
  // actual length of activeQueue if consider duplicates.
  std::size_t bytes_;
};

////////////////////////////////////////////////////////////////////////////////
// inline implementations
////////////////////////////////////////////////////////////////////////////////

inline BandwidthTimeWindowOffline::TimeByte::TimeByte(
    size_t h, const LogEntry::TimePoint &time, const std::string &timezoneStr)
    : timeStr()
    , bytes(h)
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
