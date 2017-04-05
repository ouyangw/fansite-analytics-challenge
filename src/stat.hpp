////////////////////////////////////////////////////////////////////////////////
// The base class for statistics.
// Interfaces:
//   processLine - process a line of input
//   getStat - return a string as the stat
////////////////////////////////////////////////////////////////////////////////

#ifndef STAT_HPP
#define STAT_HPP

#include "log_entry.hpp"
#include <string>

class Stat
{
public:
  virtual void processLine(const LogEntry &) = 0;
  virtual std::string getStat() = 0;
};

#endif
