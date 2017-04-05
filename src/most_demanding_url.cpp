////////////////////////////////////////////////////////////////////////////////
// Feature 2: N most bandwidth demanding URL.
// Online calculation:
//   1. Use hashmap to gather counts for each host
//   2. Maintain a vector for the N most demanding URL
//   3. Bubble up host in the vector in each processing
////////////////////////////////////////////////////////////////////////////////

#include "most_demanding_url.hpp"
#include <utility>
#include <cassert>
#include <sstream>

////////////////////////////////////////////////////////////////////////////////

MostDemandingURL::MostDemandingURL(size_t N): N_(N)
{
  firstN_.reserve(N);
}

////////////////////////////////////////////////////////////////////////////////

void MostDemandingURL::processLine(const LogEntry &line)
{
  infoMap_[line.url].accruedBytes += line.responseSize;
  Info &info(infoMap_[line.url]);
  if (info.isFirstN && info.idx > 0) {
    // bubble up in the vector
    bubbleUp(info.idx);
  } else if (!info.isFirstN) {
    if (firstN_.size() < N_) {
      // the vector is not full, just push
      firstN_.push_back(line.url);
      info.isFirstN = true;
      info.idx = firstN_.size() - 1;
      // new resource might consume large size of data, so bubble up
      bubbleUp(info.idx);
    } else if (info.accruedBytes > infoMap_[firstN_.back()].accruedBytes) {
      // new accrued byte is large than the smallest accured bytes in the vector
      // replace the last entry from the vector and bubble up
      infoMap_[firstN_.back()].isFirstN = false;
      info.isFirstN = true;
      info.idx = N_ - 1;
      firstN_.back() = line.url;
      bubbleUp(N_ - 1);
    }
  }
}

////////////////////////////////////////////////////////////////////////////////

std::string MostDemandingURL::getStat()
{
  std::stringstream ss;
  for (const std::string &s: firstN_)
    ss << s << '\n';
  return ss.str();
}

////////////////////////////////////////////////////////////////////////////////

void MostDemandingURL::bubbleUp(size_t idx) noexcept
{
  Info &info(infoMap_[firstN_[idx]]);
  for (; idx > 0; --idx) {
    if (infoMap_[firstN_[idx - 1]].accruedBytes >= info.accruedBytes)
      break;
    infoMap_[firstN_[idx - 1]].idx = idx;
    info.idx = idx - 1;
    std::swap(firstN_[idx - 1], firstN_[idx]);
  }
}
