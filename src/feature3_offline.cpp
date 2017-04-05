////////////////////////////////////////////////////////////////////////////////
// Feature 3: 10 busiest 1 hour period
// Caution: time window can start anywhere but not limited to the time points in
//          the log file.
// Offline calculation:
//   1. Use min-heap to keep track of 10 busiest periods outside 1 hour.
//   2. Use a queue to keep track of periods within 1 hour.
//   3. Use a fake time that is 2 hours later than the last line to pop the
//      elements in queue into min-heap.
////////////////////////////////////////////////////////////////////////////////

#include "helper_fnc.hpp"
#include <queue>
#include <stack>
#include <fstream>
#include <vector>
#include <algorithm>
#include <utility>
#include <iostream>
#include <functional>

struct TimeHit {
  std::string timeStr;
  size_t hits;
  TimeHit(size_t h, const LogEntry::TimePoint &time,
          const std::string &timezoneStr)
      : timeStr()
      , hits(h)
  {
    // construct the string representation of time: timeStr
    char tmp[21];
    std::time_t t(std::chrono::system_clock::to_time_t(time));
    // system_clock take into account the time zone in runtime
    std::strftime(tmp, 21, "%d/%b/%G:%H:%M:%S", std::localtime(&t));
    timeStr.assign(tmp);
    timeStr += timezoneStr;
  }
};

bool operator<(const TimeHit &lhs, const TimeHit &rhs)
{
  return (lhs.hits == rhs.hits) ? lhs.timeStr < rhs.timeStr
                                : lhs.hits > rhs.hits;
}

int main(int argc, char **argv)
{
  if (argc < 3) {
    std::cerr << "Error: not enough arguments: <program> infile outfile\n";
    return 1;
  }
  std::ifstream infile(argv[1]);
  if (!infile.good()) {
    std::cerr << "Error: cannot open file: " << argv[1] << '\n';
    return 2;
  }
  std::ofstream outfile(argv[2]);
  if (!outfile.good()) {
    std::cerr << "Error: cannot open file: " << argv[2] << '\n';
    return 3;
  }

  // min heap for finalized time points which are outside of 1 hour window
  std::priority_queue<TimeHit> minHeap;
  // queue for active time points which are inside the 1 hour window
  // we keep track of number of duplicated time point so the queue won't be
  // filled with duplicated time points
  std::queue<std::pair<LogEntry::TimePoint, size_t>> activeQueue;
  // actual number of hit
  // actual length of activeQueue if consider duplicates.
  std::size_t hits(0);

  // auxiliary variables
  const std::chrono::hours oneHour(1);
  // number of statistics needed.
  const size_t N(10);
  std::size_t lineCount(0);
  std::string timezoneStr;
  bool isExtraLine(false);
  
  // calculate initial time point and compute timezone string
  std::string s;
  std::getline(infile, s);
  if (!infile.good()) {
    // infile is empty, just return
    infile.close();
    outfile.close();
    return 0;
  }
  LogEntry line(parseLine(s));
  ++lineCount;
  const LogEntry::TimePoint initialTime(line.time);
  { // compute timezone string (time zone should not change in one file)
    if (line.timezone < 0)
      timezoneStr = " -";
    else
      timezoneStr = " +";
    const short absTimezone(std::abs(line.timezone));
    if (absTimezone > 9)
      timezoneStr += std::to_string(absTimezone);
    else
      timezoneStr += "0" + std::to_string(absTimezone);
    timezoneStr += "00";
  }
  // insert the first time point in queue
  activeQueue.emplace(initialTime, 1);
  hits += 1;
  while (!isExtraLine) {
    std::getline(infile, s);
    if (!infile.good()) {
      // add a fake line with 2 hour later time so that all element in
      // activeQueue will be popped
      isExtraLine = true;
      line.time += 2 * oneHour;
    }
    else {
      ++lineCount;
      line = parseLine(s);
    }

    // add count of duplicate, add total hits, no change otherwise
    if (line.time == activeQueue.back().first) {
      activeQueue.back().second += 1;
      hits += 1;
      continue;
    }

    // finalize old time points
    if (!activeQueue.empty() &&
        line.time - activeQueue.front().first > oneHour) {
      // offset to move the beginning of the time window
      std::chrono::seconds secOffset(0);
      // window begins before inital time must be adjusted
      if (activeQueue.back().first - oneHour < initialTime)
        secOffset = initialTime + oneHour - activeQueue.back().first;
      // the time span we need to consider pushing into heap
      const std::chrono::seconds timeSpan(line.time - activeQueue.back().first);
      // push new elements to heap
      for (; secOffset < timeSpan; ++secOffset) {
        // the beginning of time window
        const LogEntry::TimePoint wbegin(activeQueue.back().first -
                                         (oneHour - secOffset));
        // construct a new element
        TimeHit newTH(hits, wbegin, timezoneStr);
        // later new element will be smaller
        // early termination
        if (minHeap.size() == N && !(minHeap.top() < newTH))
          break;
        minHeap.push(newTH);
        if (minHeap.size() > N)
          minHeap.pop();
        // decrease total hits when meeting the front time point
        if (wbegin == activeQueue.front().first) {
          hits -= activeQueue.front().second;
          activeQueue.pop();
          if (activeQueue.empty())
            break;
        }
      }
      // pop those remained because of early termination of the loop above
      while (!activeQueue.empty() &&
             line.time - activeQueue.front().first > oneHour) {
        hits -= activeQueue.front().second;
        activeQueue.pop();
      }
    }

    // add new time point
    activeQueue.emplace(line.time, 1);
    hits += 1;
  }

  // reverse the sequence of TimeHit in minHeap for output
  std::stack<TimeHit> stack;
  while (!minHeap.empty()) {
    stack.push(minHeap.top());
    minHeap.pop();
  }

  while (!stack.empty()) {
    outfile << stack.top().timeStr << ',' << stack.top().hits << '\n';
    stack.pop();
  }

  infile.close();
  outfile.close();
  std::cout << lineCount << " line processed.\n";
  return 0;
}
