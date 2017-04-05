#include "most_demanding_url_offline.hpp"
#include <algorithm>
#include <vector>
#include <functional>
#include <sstream>

namespace
{
  struct URLUsage {
    size_t bytes;
    std::string url;
    URLUsage(size_t b, std::string u)
        : bytes(b)
        , url(u)
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
  std::stringstream stat;
  std::vector<URLUsage> vec;
  vec.reserve(countMap_.size());
  for (const auto &p: countMap_)
    vec.emplace_back(p.second, p.first);
  size_t size(vec.size() > N_ ? N_ : vec.size());
  std::partial_sort(vec.begin(), vec.begin() + size, vec.end(),
                    std::greater<URLUsage>());
  for (size_t i(0); i < size; ++i)
    stat << vec[i].url << '\n';
  return stat.str();
}
