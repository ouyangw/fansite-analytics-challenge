////////////////////////////////////////////////////////////////////////////////
// Feature 4: detect patterns of three consecutive failed log attempts over
//            a time window and record further access for a period of time
// Online calculation:
//   1. Use hashmap to map from host to failed attempts
//   2. Detect pattern and output blocked attempts
////////////////////////////////////////////////////////////////////////////////

#include "three_failure_detector.hpp"
#include "helper_fnc.hpp"

////////////////////////////////////////////////////////////////////////////////

ThreeFailureDetector::ThreeFailureDetector(const Duration &dd,
                                           const Duration &bd,
                                           std::ostream &out)
    : detectDuration_(dd)
    , blockedDuration_(bd)
    , out_(out)
    , hostMap_()
{
}

////////////////////////////////////////////////////////////////////////////////

void ThreeFailureDetector::processLine(const std::string &s)
{
  LogEntry line(parseLine(s));
  HostInfo &info(hostMap_[line.host]);
  if (info.isBlocked) {
    if (line.time <= info.blockedTill) {
      // record blocked entries
      out_ << s << '\n';
      return;
    } else {
      // reset when pass the blocking time
      info.isBlocked = false;
      info.attemptIdx = 0;
    }
  }
  if (line.url != "/login") {
    // not an login, no need to process
    return;
  }
  // successful login reset counter
  if (line.responseCode == 200)
    info.attemptIdx = 0;
  else {
    if (info.attemptIdx == 2) {
      // the third attempt
      if (info.attemptsTime[0] + detectDuration_ >= line.time) {
        info.attemptIdx = 0;
        info.isBlocked = true;
        info.blockedTill = line.time + blockedDuration_;
      } else if (info.attemptsTime[1] + detectDuration_ >= line.time) {
        info.attemptsTime[0] = info.attemptsTime[1];
        info.attemptsTime[1] = line.time;
      } else {
        info.attemptsTime[0] = line.time;
        info.attemptIdx = 1;
      }
      // Note:
      // Here we don't need to consider the first and the second
      // attempts. Even when the second attempt is beyond detection time after
      // the first attempt, the third attempt will eliminate failures outside of
      // detection time.
    } else {
      info.attemptsTime[info.attemptIdx++] = line.time;
    }
  }
}
