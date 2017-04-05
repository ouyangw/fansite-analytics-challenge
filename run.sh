#!/usr/bin/env bash

# one example of run.sh script for implementing the features using python
# the contents of this script could be replaced with similar files from any major language

# I'll execute my programs, with the input directory log_input and output the files in the directory log_output
# python ./src/process_log.py ./log_input/log.txt ./log_output/hosts.txt ./log_output/hours.txt ./log_output/resources.txt ./log_output/blocked.txt

g++ -std=c++11 -O2 src/main.cpp src/helper_fnc.cpp src/most_active_host_offline.cpp src/most_demanding_url_offline.cpp src/busiest_time_window_offline.cpp src/three_failure_detector.cpp -o main.exe
./main.exe ./log_input/log.txt ./log_output/hosts.txt ./log_output/resources.txt ./log_output/hours.txt ./log_output/blocked.txt
