////////////////////////////////////////////////////////////////////////////////
// Feature 1: N most active host.
// Offline calculation:
//   1. Use hashmap to gather counts for each host
//   2. Use min-heap to find the N most active host
////////////////////////////////////////////////////////////////////////////////

#include "most_active_host_offline.hpp"
#include <sstream>
#include <algorithm>
#include <queue>
#include <stack>
#include <functional>

namespace
{
  struct HostHit {
    std::string host;
    size_t hits;
    HostHit(std::string h, size_t n)
        : host(h)
        , hits(n)
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
  // use min-heap to compute the N most active hosts
  std::priority_queue<HostHit, std::vector<HostHit>, std::greater<HostHit>>
      minHeap;
  auto it(countMap_.begin());
  for (size_t i(0); i < N_ && it != countMap_.end(); ++i, ++it)
    minHeap.emplace(it->first, it->second);
  for (; it != countMap_.end(); ++it) {
    minHeap.emplace(it->first, it->second);
    minHeap.pop();
  }
  // use a stack to reverse the sequence in minHeap
  std::stack<HostHit> stack;
  while (!minHeap.empty()) {
    stack.emplace(minHeap.top());
    minHeap.pop();
  }
  std::stringstream stat;
  while (!stack.empty()) {
    stat << stack.top().host << ',' << stack.top().hits << '\n';
    stack.pop();
  }
  return stat.str();
}
