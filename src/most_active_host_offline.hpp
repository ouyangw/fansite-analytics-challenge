////////////////////////////////////////////////////////////////////////////////
// Feature 1: N most active host.
// Offline calculation:
//   1. Use hashmap to gather counts for each host
//   2. Dump the hashmap into a vector and partially sort the vector
////////////////////////////////////////////////////////////////////////////////

#ifndef MOST_ACTIVE_HOST_OFFLINE_HPP
#define MOST_ACTIVE_HOST_OFFLINE_HPP

#include "stat.hpp"
#include <unordered_map>

class MostActiveHostOffline : public Stat
{
public:
  // N - the number of most active host
  explicit MostActiveHostOffline(size_t N);
  void processLine(const LogEntry &) override;
  std::string getStat() override;

private:
  const size_t N_;
  // map from host to connection count
  std::unordered_map<std::string, size_t> countMap_;
};

////////////////////////////////////////////////////////////////////////////////
// inline implementations
////////////////////////////////////////////////////////////////////////////////

inline MostActiveHostOffline::MostActiveHostOffline(size_t N)
    : N_(N)
    , countMap_()
{
}

////////////////////////////////////////////////////////////////////////////////

inline void MostActiveHostOffline::processLine(const LogEntry &line)
{
  countMap_[line.host] += 1;
}

#endif
