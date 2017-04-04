#include "helper_fnc.hpp"
#include <ctime>
#include <unordered_map>

using std::string;
using std::size_t;

////////////////////////////////////////////////////////////////////////////////
// parse a line of input
////////////////////////////////////////////////////////////////////////////////

LogEntry parseLine(const string &s) noexcept
{
  // map from month abbreviation to tm_mon
  // tm_mon is the months since January
  static const std::unordered_map<std::string, int> monthMap{
      {"Jan", 0}, {"Feb", 1}, {"Mar", 2}, {"Apr", 3}, {"May", 4},  {"Jun", 5},
      {"Jul", 6}, {"Aug", 7}, {"Sep", 8}, {"Oct", 9}, {"Nov", 10}, {"Dec", 11}};
  LogEntry line;
  {
    // get response size
    // (reason to get size in the beginning is near the end of function)
    size_t idx(s.find_last_not_of(' '));
    if (idx == string::npos) return line;
    if (s[idx] != '-') {
      size_t idxBegin(s.find_last_of(' ', idx));
      if (idxBegin == string::npos) return line;
      ++idxBegin;
      for (; idxBegin < idx + 1; ++idxBegin)
        line.responseSize = line.responseSize * 10 + s[idxBegin] - '0';
    }

    // get response code
    // see reason for response size
    idx = s.find_last_of(' ', idx);
    if (idx == string::npos || idx < 3) return line;
    line.responseCode =
        s[idx - 1] - '0' + 10 * (s[idx - 2] - '0') + 100 * (s[idx - 3] - '0');
  }


  // get host name or IP address
  size_t idx(s.find_first_of(' ', 0));
  if (idx == string::npos) return line;
  line.host = s.substr(0, idx);

  // get timestamp
  idx = s.find_first_of('[', idx);
  if (idx == string::npos) return line;
  {
    ++idx;
    std::tm tm = {0};
    tm.tm_mday = 10 * (s[idx] - '0') + s[idx + 1] - '0';
    idx += 3;
    tm.tm_mon = monthMap.at(s.substr(idx, 3));
    idx += 4;
    tm.tm_year = 1000 * (s[idx] - '0') + 100 * (s[idx + 1] - '0')
                 + 10 * (s[idx + 2] - '0') + s[idx + 3] - '0' - 1900;
    idx += 5;
    tm.tm_hour = 10 * (s[idx] - '0') + s[idx + 1] - '0';
    idx += 3;
    tm.tm_min = 10 * (s[idx] - '0') + s[idx + 1] - '0';
    idx += 3;
    tm.tm_sec = 10 * (s[idx] - '0') + s[idx + 1] - '0';
    tm.tm_isdst = 0;
    line.time = std::chrono::time_point_cast<LogEntry::Duration>(
        std::chrono::system_clock::from_time_t(std::mktime(&tm)));
    // if daylight saving time flag is set, the time will be 1 hour later
    if (tm.tm_isdst > 0)
      line.time -= std::chrono::hours(1);
  }

  // get timezone
  idx += 2;
  idx = s.find_first_not_of(' ', idx);
  size_t idxEnd(s.find_first_of(']', idx));
  line.timezone = std::stoi(s.substr(idx, idxEnd - idx)) / 100;
  idx = idxEnd + 1;

  // get request method
  idx = s.find_first_not_of(' ', idx);
  if (idx == string::npos) return line;
  idx += 1;
  idxEnd = s.find_first_of(' ', idx);
  line.method = s.substr(idx, idxEnd - idx);
  idx = idxEnd;

  // get request URL
  if (idx == string::npos) return line;
  ++idx;
  idxEnd = s.find_first_of(' ', idx);
  line.url = s.substr(idx, idxEnd - idx);
  idx = idxEnd;

  // get request protocol
  if (idx == string::npos) return line;
  ++idx;
  idxEnd = s.find_first_of(' ', idx);
  line.protocol = s.substr(idx, idxEnd - idx - 1);
  idx = idxEnd;

  // see explanation for response size.
//  // get response code
//  if (idx == string::npos) return line;
//  idx = s.find_first_not_of(' ', idx);
//  line.responseCode =
//      100 * (s[idx] - '0') + 10 * (s[idx + 1] - '0') + s[idx + 2] - '0';
//  idx += 3;

  //  response size is gather in the very beginning
  //  because the data is not normalized
  //  for instance,
  //  firewall.dfw.ibm.com - - [20/Jul/1995:07:34:34 -0400] "1/history/apollo/images/" 400 -
  //  it srews up badly
  //  but these entries tend to have a size of 0
  //
//  // get response size
//  idx = s.find_first_not_of(' ', idx);
//  if (idx == string::npos) return line;
//  if (s[idx] == '-') return line; // '-' is zero size
//  for (; idx < s.size(); ++idx)
//    line.responseSize = 10 * line.responseSize + s[idx] - '0';

  return line;
}
