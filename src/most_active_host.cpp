#include "most_active_host.hpp"
#include <utility>
#include <cassert>
#include <sstream>

////////////////////////////////////////////////////////////////////////////////

MostActiveHost::MostActiveHost(size_t N): N_(N)
{
  firstN_.reserve(N);
}

////////////////////////////////////////////////////////////////////////////////

void MostActiveHost::processLine(const LogEntry &line)
{
  infoMap_[line.host].count += 1;
  Info &info(infoMap_[line.host]);
  if (info.isFirstN && info.idx > 0) {
    // find the new position in the vector.
    size_t newIdx(info.idx);
    for (; newIdx > 0; --newIdx)
      if (infoMap_[firstN_[newIdx - 1]].count >= info.count) break;
    assert(infoMap_[firstN_[newIdx]].idx == newIdx);
    const size_t oldIdx(info.idx);
    infoMap_[firstN_[newIdx]].idx = oldIdx;
    info.idx = newIdx;
    std::swap(firstN_[newIdx], firstN_[oldIdx]);
  } else if (!info.isFirstN) {
    if (firstN_.size() < N_) {
      // the vector is not full, just push
      firstN_.push_back(line.host);
      info.isFirstN = true;
      info.idx = firstN_.size() - 1;
    } else if (info.count > infoMap_[firstN_.back()].count) {
      // new count is large than the smallest count in the vector
      // replace one entry from the vector
      size_t newIdx(N_ - 1);
      for (; newIdx > 0; --newIdx)
        if (infoMap_[firstN_[newIdx - 1]].count >= info.count) break;
      infoMap_[firstN_[newIdx]].isFirstN = false;
      info.isFirstN = true;
      info.idx = newIdx;
      firstN_[newIdx] = line.host;
    }
  }
}

////////////////////////////////////////////////////////////////////////////////

std::string MostActiveHost::getStat()
{
  std::stringstream ss;
  for (const std::string &s: firstN_)
    ss << s << ','
       << infoMap_[s].count << '\n';
  return ss.str();
}

