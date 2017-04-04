////////////////////////////////////////////////////////////////////////////////
// Feature 3: 10 busiest 1 hour period
// Caution: time window can start anywhere but not limited to the time points in
//          the log file.
// Offline calculation:
//   1. Use min-heap to keep track of 10 busiest periods outside 1 hour.
//   2. Use a queue to keep track of periods within 1 hour.
//   3. Dump queue into min-heap and get final result.
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
  LogEntry::TimePoint time;
  TimeHit(size_t h, const LogEntry &line)
      : timeStr()
      , hits(h)
      , time(line.time)
  {
    // construct the string representation of time: timeStr
    char tmp[21];
    std::time_t t(std::chrono::system_clock::to_time_t(time));
    // system_clock take into account the time zone in runtime
    std::strftime(tmp, 21, "%d/%b/%G:%H:%M:%S", std::localtime(&t));
    timeStr.assign(tmp);
    if (line.timezone < 0)
      timeStr += " -";
    else
      timeStr += " -";
    const short absTimezone(std::abs(line.timezone));
    if (absTimezone > 9)
      timeStr += std::to_string(absTimezone);
    else
      timeStr += "0" + std::to_string(absTimezone);
    timeStr += "00";
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
  // We only need to keep track of the hits of first element
  // because the hits number will be sequential: the element will have exactly
  // one fewer hit than the element in front of it.
  std::queue<TimeHit> activeQueue;

  const std::chrono::hours oneHour(1);
  // number of statistics needed.
  const size_t N(10);
  std::size_t lineCount(0);
  
  // insert the first entry into activeQueue so activeQueue is not empty
  std::string s;
  std::getline(infile, s);
  if (!infile.good()) {
    // infile is empty, just return
    infile.close();
    outfile.close();
    return 0;
  }
  ++lineCount;
  LogEntry line(parseLine(s));
  activeQueue.emplace(1, line);
  while (true) {
    std::getline(infile, s);
    if (!infile.good())
      break;
    ++lineCount;
    line = parseLine(s);
    // push new time point if not the same as previous time point
    if (line.time != activeQueue.back().time)
      activeQueue.emplace(1, line);
    // finalize time points
    // not considering the new time point yet
    while (line.time - activeQueue.front().time > oneHour) {
      // the element next to the front will have exactly one fewer hit
      size_t nextHits(activeQueue.front().hits - 1);
      minHeap.push(activeQueue.front());
      activeQueue.pop();
      activeQueue.front().hits = nextHits;
      // pop heap when full
      if (minHeap.size() > N)
        minHeap.pop();
    }
    // add one more hit to the front for the new time point
    activeQueue.front().hits += 1;
  }
  // dump activeQueue into minHeap
  while (!activeQueue.empty() &&
         (minHeap.size() < N || minHeap.top() < activeQueue.front())) {
    size_t nextHits(activeQueue.front().hits - 1);
    minHeap.push(activeQueue.front());
    activeQueue.pop();
    if (!activeQueue.empty())
      activeQueue.front().hits = nextHits;
    if (minHeap.size() > N)
      minHeap.pop();
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
