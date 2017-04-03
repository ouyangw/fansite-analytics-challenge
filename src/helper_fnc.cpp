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
  // get host name or IP address
  size_t idx(s.find_first_of(' ', 0));
  if (idx == string::npos) return line;
  line.host = s.substr(0, idx);
 
  // get timestamp
  idx = s.find_first_of('[', idx);
  if (idx == string::npos) return line;
  {
    ++idx;
    std::tm tm;
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
    line.time = std::chrono::time_point_cast<LogEntry::Duration>(
        std::chrono::system_clock::from_time_t(std::mktime(&tm)));
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
  
  // get response code
  if (idx == string::npos) return line;
  idx = s.find_first_not_of(' ', idx);
  line.responseCode =
      100 * (s[idx] - '0') + 10 * (s[idx + 1] - '0') + s[idx + 2] - '0';
  idx += 3;

  // get response size
  idx = s.find_first_not_of(' ', idx);
  if (idx == string::npos) return line;
  if (s[idx] == '-') return line; // '-' is zero size
  for (; idx < s.size(); ++idx)
    line.responseSize = 10 * line.responseSize + s[idx] - '0';
  
  return line;
}
