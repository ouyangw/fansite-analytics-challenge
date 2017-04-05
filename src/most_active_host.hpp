////////////////////////////////////////////////////////////////////////////////
// Maintain the information of N most active host
////////////////////////////////////////////////////////////////////////////////

#ifndef MOST_ACTIVE_HOST_HPP
#define MOST_ACTIVE_HOST_HPP

#include "stat.hpp"
#include <unordered_map>
#include <vector>
#include <string>

class MostActiveHost : public Stat
{
public:
  // N - the number of most active host
  explicit MostActiveHost(size_t N);
  void processLine(const LogEntry &) override;
  std::string getStat() override;

private:
  struct Info {
    size_t count;
    bool isFirstN;
    size_t idx;
    Info(): count(0), isFirstN(false), idx(0) {}
  };
  const size_t N_;
  // map from host to info
  std::unordered_map<std::string, Info> infoMap_;
  // sorted vector of N most active host
  std::vector<std::string> firstN_;
};

#endif
