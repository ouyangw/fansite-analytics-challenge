////////////////////////////////////////////////////////////////////////////////
// Feature 1: N most active host.
// Offline calculation:
//   1. Use hashmap to gather counts for each host
//   2. Dump the hashmap into a vector and partially sort the vector
////////////////////////////////////////////////////////////////////////////////

#include "most_active_host_offline.hpp"
#include <sstream>
#include <algorithm>
#include <vector>
#include <functional>

namespace
{
  struct HostHit {
    size_t hits;
    std::string host;
    HostHit(size_t n, std::string h)
        : hits(n)
        , host(h)
    {
    }
  };

  bool operator>(const HostHit &lhs, const HostHit &rhs)
  {
    return lhs.hits == rhs.hits ? lhs.host < rhs.host : lhs.hits > rhs.hits;
  }
} // anonymous namespace

////////////////////////////////////////////////////////////////////////////////

std::string MostActiveHostOffline::getStat()
{
  // dump data in map to vector and partially sort the vector
  std::stringstream stat;
  std::vector<HostHit> vec;
  vec.reserve(countMap_.size());
  for (const auto &p: countMap_)
    vec.emplace_back(p.second, p.first);
  const size_t size(vec.size() > N_ ? N_ : vec.size());
  std::partial_sort(vec.begin(), vec.begin() + size, vec.end(),
                    std::greater<HostHit>());
  for (size_t i(0); i < size; ++i)
    stat << vec[i].host << ',' << vec[i].hits << '\n';
  return stat.str();
}
