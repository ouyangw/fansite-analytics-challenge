////////////////////////////////////////////////////////////////////////////////
// Feature 2: N most bandwidth demanding URL.
// Offline calculation:
//   1. Use hashmap to gather data size for each URL
//   2. Dump the hashmap into a vector and partially sort the vector
////////////////////////////////////////////////////////////////////////////////

#ifndef MOST_DEMANDING_URL_OFFLINE_HPP
#define MOST_DEMANDING_URL_OFFLINE_HPP

#include "stat.hpp"
#include <unordered_map>

class MostDemandingURLOffline : public Stat
{
public:
  explicit MostDemandingURLOffline(size_t N);
  void processLine(const LogEntry &) override;
  std::string getStat() noexcept override;

private:
  size_t N_;
  // map from URL to the bytes consumed
  std::unordered_map<std::string, size_t> countMap_;
};

////////////////////////////////////////////////////////////////////////////////
// inline implementation
////////////////////////////////////////////////////////////////////////////////

inline MostDemandingURLOffline::MostDemandingURLOffline(size_t N)
    : N_(N)
    , countMap_()
{
}

////////////////////////////////////////////////////////////////////////////////

inline void MostDemandingURLOffline::processLine(const LogEntry &line)
{
  countMap_[line.url] += line.responseSize;
}

#endif
