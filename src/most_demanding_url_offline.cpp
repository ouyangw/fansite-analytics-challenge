////////////////////////////////////////////////////////////////////////////////
// Feature 2: N most bandwidth demanding URL.
// Offline calculation:
//   1. Use hashmap to gather data size for each URL
//   2. Use min-heap to find the N most bandwidth demanding URL
////////////////////////////////////////////////////////////////////////////////

#include "most_demanding_url_offline.hpp"
#include <algorithm>
#include <queue>
#include <stack>
#include <functional>
#include <sstream>

namespace
{
  struct URLUsage {
    std::string url;
    size_t bytes;
    URLUsage(std::string u, size_t b)
        : url(u)
        , bytes(b)
    {
    }
  };

  bool operator>(const URLUsage &lhs, const URLUsage &rhs)
  {
    return lhs.bytes == rhs.bytes ? lhs.url < rhs.url : lhs.bytes > rhs.bytes;
  }
} // anonymous namespace

////////////////////////////////////////////////////////////////////////////////

std::string MostDemandingURLOffline::getStat()
{
  // use min-heap to compute the N most bandwidth demanding URL
  std::priority_queue<URLUsage, std::vector<URLUsage>, std::greater<URLUsage>>
      minHeap;
  auto it(countMap_.begin());
  for (size_t i(0); i < N_ && it != countMap_.end(); ++i, ++it)
    minHeap.emplace(it->first, it->second);
  for (; it != countMap_.end(); ++it) {
    minHeap.emplace(it->first, it->second);
    minHeap.pop();
  }
  // use a stack to reverse the sequence in minHeap
  std::stack<URLUsage> stack;
  while (!minHeap.empty()) {
    stack.emplace(minHeap.top());
    minHeap.pop();
  }
  std::stringstream stat;
  while (!stack.empty()) {
    stat << stack.top().url << '\n';
    stack.pop();
  }
  return stat.str();
}
