////////////////////////////////////////////////////////////////////////////////
// test parseLine
////////////////////////////////////////////////////////////////////////////////

#include "helper_fnc.hpp"
#include <fstream>
#include <iostream>
#include <string>
#include <ctime>
#include <iomanip>
#include <cmath>

int main(int argc, char **argv)
{
  if (argc < 3) {
    std::cerr << "Error: not enough arguments: <program> infile outfile\n";
    return 1;
  }
  std::ifstream infile(argv[1]);
  if (!infile.good()) {
    std::cerr << "Error: cannot open input file: " << argv[1] << '\n';
    return 2;
  }
  std::ofstream outfile(argv[2]);
  if (!outfile.good()) {
    std::cerr << "Error: cannot open output file: " << argv[2] << '\n';
    return 3;
  }
  std::string s;
  char time[21];
  LogEntry firstLine;
  size_t i(0);
  while (true) {
    std::getline(infile, s);
    if (!infile.good()) break;
    LogEntry line(parseLine(s));
    if (i == 0) firstLine = line;
    std::time_t t(std::chrono::system_clock::to_time_t(line.time));
    // system_clock take into account the time zone in runtime
    // std::tm tm(*std::localtime(&t));
    std::strftime(time, 21, "%d/%b/%G:%H:%M:%S", std::localtime(&t));
    outfile << line.host << " - - "
            << '[' << time << ' ';
    if (line.timezone < 0)
      outfile << '-';
    outfile << std::setfill('0') << std::setw(2)
            << std::abs(line.timezone) << "00] "
            << '"' << line.method << ' '
            << line.url << ' '
            << line.protocol << "\" "
            << line.responseCode << ' '
            << line.responseSize << '\n';
//    outfile << (line.time - firstLine.time).count() << '\n';
    ++i;
  }
  infile.close();
  outfile.close();
  return 0;
}
