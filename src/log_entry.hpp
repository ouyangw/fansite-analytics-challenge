////////////////////////////////////////////////////////////////////////////////
// The struct to represent each entry (line) of log
////////////////////////////////////////////////////////////////////////////////

#ifndef LOG_ENTRY_HPP
#define LOG_ENTRY_HPP

#include <string>
#include <chrono>

struct LogEntry
{
  using Duration =
      std::chrono::duration<std::chrono::system_clock::duration::rep>;
  using TimePoint =
      std::chrono::time_point<std::chrono::system_clock, Duration>;
  std::string host;
  TimePoint time;
  short timezone;
  std::string method;
  std::string url;
  std::string protocol;
  short responseCode;
  size_t responseSize;

  LogEntry()
      : host()
      , time()
      , timezone(0)
      , method()
      , url()
      , protocol()
      , responseCode(0)
      , responseSize(0)
  {
  }

};

#endif
