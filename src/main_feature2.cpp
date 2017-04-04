////////////////////////////////////////////////////////////////////////////////
// Feature 2: 10 most bandwidth demanding URL.
////////////////////////////////////////////////////////////////////////////////

#include "most_demanding_url_offline.hpp"
#include "helper_fnc.hpp"
#include <fstream>
#include <iostream>

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
  std::size_t lineProcessed(0);
  MostDemandingURLOffline mdu(10);
  while (true) {
    std::getline(infile, s);
    if (!infile.good())
      break;
    LogEntry line(parseLine(s));
    mdu.processLine(line);
    ++lineProcessed;
  }
  outfile << mdu.getStat();

  infile.close();
  outfile.close();

  std::cout << lineProcessed;
  if (lineProcessed > 1)
    std::cout << " lines processed.\n";
  else
    std::cout << " line processed.\n";
  return 0;
}
