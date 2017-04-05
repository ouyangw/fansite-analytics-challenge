////////////////////////////////////////////////////////////////////////////////
// Feature 2: N most bandwidth demanding URL.
////////////////////////////////////////////////////////////////////////////////

#ifndef MOST_DEMANDING_RESOURCE_HPP
#define MOST_DEMANDING_RESOURCE_HPP

#include "stat.hpp"
#include <unordered_map>
#include <vector>
#include <string>

class MostDemandingResource : public Stat
{
public:
  // N - the number of most demanding resource
  explicit MostDemandingResource(size_t N);
  void processLine(const LogEntry &) override;
  std::string getStat() override;

private:
  void bubbleUp(size_t index) noexcept;

private:
  struct Info {
    size_t accruedBytes;
    bool isFirstN;
    size_t idx;
    Info(): accruedBytes(0), isFirstN(false), idx(0) {}
  };
  const size_t N_;
  // map from resource to info
  std::unordered_map<std::string, Info> infoMap_;
  // sorted vector of N most demanding resource
  std::vector<std::string> firstN_;
};

#endif
