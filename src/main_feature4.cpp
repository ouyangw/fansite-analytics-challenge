////////////////////////////////////////////////////////////////////////////////
// Feature 4: detect patterns of three consecutive failed log attempts over
//            20 seconds
////////////////////////////////////////////////////////////////////////////////

#include "three_failure_detector.hpp"
#include <iostream>
#include <fstream>
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

  std::string s;
  size_t lineProcessed(0);
  ThreeFailureDetector tfd(std::chrono::seconds(20), std::chrono::minutes(5),
                           outfile);
  while (true) {
    std::getline(infile, s);
    if (!infile.good())
      break;
    tfd.processLine(s);
    ++lineProcessed;
  }

  infile.close();
  outfile.close();
  std::cout << lineProcessed;
  if (lineProcessed > 1)
    std::cout << " lines processed.\n";
  else
    std::cout << " line processed.\n";
  return 0;
}
