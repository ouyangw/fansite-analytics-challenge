////////////////////////////////////////////////////////////////////////////////
// Feature 4: detect patterns of three consecutive failed log attempts over
//            20 seconds
// Online calculation:
//   1. Use hashmap to map from host to failed attempts
//   2. Detect pattern and output blocked attempts
////////////////////////////////////////////////////////////////////////////////

#include "helper_fnc.hpp"
#include <unordered_map>
#include <fstream>
#include <iostream>
#include <string>

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

int main(int argc, char **argv)
{
  if (argc < 3) {
    std::cerr << "Error: not enough arguments: <program> infile outfile\n";
    return 1;
  }
  std::ifstream infile(argv[1]);
  if (!infile.good()) {
    std::cerr << "Error: cannot open file: " << argv[1] << '\n';
    return 2;
  }
  std::ofstream outfile(argv[2]);
  if (!outfile.good()) {
    std::cerr << "Error: cannot open file: " << argv[2] << '\n';
    return 3;
  }

  std::size_t lineProcessed(0); // number of line processed.
  std::chrono::seconds twentySec(20);
  std::chrono::minutes fiveMin(5);
  // map from host to HostInfo
  std::unordered_map<std::string, HostInfo> hostMap;
  std::string s;
  while (true) {
    std::getline(infile, s);
    if (!infile.good())
      break;
    ++lineProcessed;
    LogEntry line(parseLine(s));
    HostInfo &info(hostMap[line.host]);
    if (info.isBlocked) {
      if (line.time <= info.blockedTill) {
        // record blocked entries
        outfile << s << '\n';
        continue;
      } else {
        // reset when pass the blocking time
        info.isBlocked = false;
        info.attemptIdx = 0;
      }
    }
    if (line.url != "/login") { 
      // not an login, no need to process
      continue;
    }
    // successful login reset counter
    if (line.responseCode == 200)
      hostMap[line.host].attemptIdx = 0;
    else {
      if (info.attemptIdx == 2) {
        // the third attempt
        if (info.attemptsTime[0] + twentySec >= line.time) {
          info.attemptIdx = 0;
          info.isBlocked = true;
          info.blockedTill = line.time + fiveMin;
        } else if (info.attemptsTime[1] + twentySec >= line.time) {
          info.attemptsTime[0] = info.attemptsTime[1];
          info.attemptsTime[1] = line.time;
        } else {
          info.attemptsTime[0] = line.time;
          info.attemptIdx = 1;
        }
        // Note: 
        // Here we don't need to consider the first and the second
        // attempts. Even when the second attempt is beyond 20 seconds after the
        // first attempt, the third attempt will eliminate failures outside of 
        // 20 seconds.
      } else {
        info.attemptsTime[info.attemptIdx++] = line.time;
      }
    }
  }

  infile.close();
  outfile.close();
  std::cout << lineProcessed << " line processed.\n";
  return 0;
}
