////////////////////////////////////////////////////////////////////////////////
// Feature 3: N busiest in time window
// Caution: time window can start anywhere but not limited to the time points in
//          the log file.
// Offline calculation:
//   1. Use min-heap to keep track of N busiest periods outside time window.
//   2. Use a queue to keep track of periods within time window.
//   3. Use a fake time that is twice the time window later than the last line
//      to pop the elements in queue into min-heap.
////////////////////////////////////////////////////////////////////////////////

#include "busiest_time_window_offline.hpp"
#include <stack>
#include <sstream>

////////////////////////////////////////////////////////////////////////////////

BusiestTimeWindowOffline::BusiestTimeWindowOffline(size_t N,
                                                   const Duration &window)
    : isInitialized_(false)
    , initialTime_()
    , prevTime_()
    , timezoneStr_()
    , N_(N)
    , timeWindow_(window)
    , minHeap_()
    , activeQueue_()
    , hits_(0)
{
}

////////////////////////////////////////////////////////////////////////////////

void BusiestTimeWindowOffline::processLine(const LogEntry &line)
{
  // record time point
  prevTime_ = line.time;

  // if not initialized, initialize and return
  if (!isInitialized_) {
    isInitialized_ = true;
    init(line);
    return;
  }

  // add count of duplicate, add total hits, no change otherwise
  if (line.time == activeQueue_.back().first) {
    activeQueue_.back().second += 1;
    hits_ += 1;
    return;
  }

  finalizeOldTimePoint(line.time);

  // add new time point
  activeQueue_.emplace(line.time, 1);
  hits_ += 1;
}

////////////////////////////////////////////////////////////////////////////////

std::string BusiestTimeWindowOffline::getStat()
{
  // dump elements in activeQueue_ by using a out-of-range time point
  finalizeOldTimePoint(prevTime_ + 2 * timeWindow_);

  std::stringstream stat;
  // reverse the sequence of TimeHit in minHeap for output
  std::stack<TimeHit> stack;
  while (!minHeap_.empty()) {
    stack.push(minHeap_.top());
    minHeap_.pop();
  }

  while (!stack.empty()) {
    stat << stack.top().timeStr << ',' << stack.top().hits << '\n';
    stack.pop();
  }

  return stat.str();
}

////////////////////////////////////////////////////////////////////////////////

void BusiestTimeWindowOffline::init(const LogEntry &line)
{
  initialTime_ = line.time;
  { // compute timezone string (time zone should not change in one file)
    if (line.timezone < 0)
      timezoneStr_ = " -";
    else
      timezoneStr_ = " +";
    const short absTimezone(std::abs(line.timezone));
    if (absTimezone > 9)
      timezoneStr_ += std::to_string(absTimezone);
    else
      timezoneStr_ += "0" + std::to_string(absTimezone);
    timezoneStr_ += "00";
  }
  activeQueue_.emplace(initialTime_, 1);
  hits_ += 1;
}

////////////////////////////////////////////////////////////////////////////////

void BusiestTimeWindowOffline::finalizeOldTimePoint(
    const LogEntry::TimePoint &time)
{
  if (activeQueue_.empty() || time - activeQueue_.front().first <= timeWindow_)
    return;
  // finalize old time points
  // offset to move the beginning of the time window
  std::chrono::seconds secOffset(0);
  // window begins before inital time must be adjusted
  if (activeQueue_.back().first - timeWindow_ < initialTime_)
    secOffset = initialTime_ + timeWindow_ - activeQueue_.back().first;
  // the time span we need to consider pushing into heap
  const std::chrono::seconds timeSpan(time - activeQueue_.back().first);
  // push new elements to heap
  for (; secOffset < timeSpan; ++secOffset) {
    // the beginning of time window
    const LogEntry::TimePoint wbegin(activeQueue_.back().first -
                                     (timeWindow_ - secOffset));
    // construct a new element
    TimeHit newTH(hits_, wbegin, timezoneStr_);
    // later new element will be larger (larger means smaller hits!)
    // early termination
    if (minHeap_.size() == N_ && !(newTH < minHeap_.top()))
      break;
    minHeap_.push(newTH);
    if (minHeap_.size() > N_)
      minHeap_.pop();
    // decrease total hits when meeting the front time point
    if (wbegin == activeQueue_.front().first) {
      hits_ -= activeQueue_.front().second;
      activeQueue_.pop();
      if (activeQueue_.empty())
        break;
    }
  }
  // pop those remained because of early termination of the loop above
  while (!activeQueue_.empty() &&
         time - activeQueue_.front().first > timeWindow_) {
    hits_ -= activeQueue_.front().second;
    activeQueue_.pop();
  }
}
