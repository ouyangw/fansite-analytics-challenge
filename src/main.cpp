#include "helper_fnc.hpp"
#include "most_demanding_resource.hpp"
#include <fstream>
#include <iostream>
#include <string>

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
  MostDemandingResource mdr(10);
  std::string s;
  std::size_t lineCount(0);
  while (true) {
    std::getline(infile, s);
    if (!infile.good()) break;
    LogEntry line(parseLine(s));
    mdr.processLine(line);
    ++lineCount;
  }
  outfile << mdr.getStat();
  infile.close();
  outfile.close();
  std::cout << lineCount << " line processed.\n";
  return 0;
}
