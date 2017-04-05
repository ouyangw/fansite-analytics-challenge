////////////////////////////////////////////////////////////////////////////////
// Feature 4: detect patterns of three consecutive failed log attempts over
//            a time window and record further access for a period of time
// Online calculation:
//   1. Use hashmap to map from host to failed attempts
//   2. Detect pattern and output blocked attempts
////////////////////////////////////////////////////////////////////////////////

#ifndef THREE_FAILURE_DETECTOR_HPP
#define THREE_FAILURE_DETECTOR_HPP

#include "log_entry.hpp"
#include <ostream>
#include <unordered_map>

class ThreeFailureDetector
{
public:
  using Duration = LogEntry::Duration;
  // attempts in the block phase will be streamed to out
  ThreeFailureDetector(const Duration &detectDuration,
                       const Duration &blockDuration, std::ostream &out);
  void processLine(const std::string &);

private:
  struct HostInfo {
    size_t attemptIdx;
    LogEntry::TimePoint attemptsTime[2];
    bool isBlocked;
    LogEntry::TimePoint blockedTill;
    HostInfo()
        : attemptIdx(0)
        , isBlocked(false)
    {
    }
  };

private:
  Duration detectDuration_, blockedDuration_;
  std::ostream &out_;
  // map from host to HostInfo
  std::unordered_map<std::string, HostInfo> hostMap_;
};

#endif
