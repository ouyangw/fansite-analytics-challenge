////////////////////////////////////////////////////////////////////////////////
// Feature 2: 10 most bandwidth demanding URL.
// Brute force calculation:
//   1. Use hashmap to gather data size for each URL
//   2. Dump the hashmap into a vector and partially sort the vector
// This is actually fast because sorting happens only once at the end.
////////////////////////////////////////////////////////////////////////////////

#include "helper_fnc.hpp"
#include <unordered_map>
#include <fstream>
#include <vector>
#include <algorithm>
#include <utility>
#include <iostream>
#include <functional>

// break the tie with lexicographical order
template <typename PairType>
struct GreaterPair {
  bool operator()(const PairType &lhs, const PairType &rhs) {
    return (lhs.first == rhs.first) ? lhs.second < rhs.second
                                    : lhs.first > rhs.first;
  }
};

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

  // map from URL to accured data size
  std::unordered_map<std::string, size_t> map;
  std::string s;
  std::size_t lineCount(0);
  while (true) {
    std::getline(infile, s);
    if (!infile.good()) break;
    LogEntry line(parseLine(s));
    map[line.url] += line.responseSize;
    ++lineCount;
  }
  std::vector<std::pair<size_t, std::string>> vec;
  vec.reserve(map.size());
  for (const auto &p: map)
    vec.emplace_back(p.second, p.first);
  size_t size(vec.size() > 10 ? 10 : vec.size());
  if (size < 10)
    std::sort(vec.begin(), vec.end(),
              GreaterPair<std::pair<size_t, std::string>>());
  else
    std::partial_sort(vec.begin(), vec.begin() + 10, vec.end(),
                      GreaterPair<std::pair<size_t, std::string>>());
  for (size_t i(0); i < size; ++i)
//    outfile << vec[i].second << ',' << vec[i].first << '\n';
    outfile << vec[i].second << '\n';
  infile.close();
  outfile.close();
  std::cout << lineCount << " line processed.\n";
  return 0;
}
