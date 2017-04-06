////////////////////////////////////////////////////////////////////////////////
// Feature 1: 10 most active host.
// Feature 2: 10 most bandwidth demanding URL.
// Feature 3: 10 busiest 1 hour period
// Feature 4: detect patterns of three consecutive failed log attempts over
//            20 seconds
////////////////////////////////////////////////////////////////////////////////

#include "most_active_host_offline.hpp"
#include "most_demanding_url_offline.hpp"
#include "busiest_time_window_offline.hpp"
#include "three_failure_detector.hpp"
#include "helper_fnc.hpp"
#include <fstream>
#include <iostream>

int main(int argc, char **argv)
{
  if (argc < 6) {
    std::cerr << "Error: not enough arguments: <program> <infile> <feature1> "
                 "<feature2> <feature3> <feature4>\n";
    return 1;
  }
  std::ifstream infile(argv[1]);
  if (!infile.good()) {
    std::cerr << "Error: cannot open file: " << argv[1] << '\n';
    return 2;
  }
  std::ofstream outfile1(argv[2]);
  if (!outfile1.good()) {
    std::cerr << "Error: cannot open file: " << argv[2] << '\n';
    return 3;
  }
  std::ofstream outfile2(argv[3]);
  if (!outfile2.good()) {
    std::cerr << "Error: cannot open file: " << argv[3] << '\n';
    return 4;
  }
  std::ofstream outfile3(argv[4]);
  if (!outfile3.good()) {
    std::cerr << "Error: cannot open file: " << argv[4] << '\n';
    return 5;
  }
  std::ofstream outfile4(argv[5]);
  if (!outfile4.good()) {
    std::cerr << "Error: cannot open file: " << argv[5] << '\n';
    return 6;
  }

  std::string s;
  std::size_t lineProcessed(0);

  MostActiveHostOffline mah(10);
  MostDemandingURLOffline mdu(10);
  BusiestTimeWindowOffline btw(10, std::chrono::hours(1));
  ThreeFailureDetector tfd(std::chrono::seconds(20), std::chrono::minutes(5),
                           outfile4);

  while (true) {
    std::getline(infile, s);
    if (!infile.good())
      break;
    LogEntry line(parseLine(s));
    mah.processLine(line);
    mdu.processLine(line);
    btw.processLine(line);
    tfd.processLine(s);
    ++lineProcessed;
  }

  infile.close();

  outfile1 << mah.getStat();
  outfile1.close();

  outfile2 << mdu.getStat();
  outfile2.close();

  outfile3 << btw.getStat();
  outfile3.close();

  outfile4.close();

  std::cout << lineProcessed;
  if (lineProcessed > 1)
   std::cout << " lines processed.\n";
  else
    std::cout << " line processed.\n";
  return 0;
}
