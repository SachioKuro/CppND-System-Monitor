#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <iostream>
#include <ctime>
//#include <filesystem>
#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// BONUS-DONE: Update this to use std::filesystem
// After installing GCC 9
/*
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  std::filesystem::path dir = kProcDirectory;
  for (const auto& file : std::filesystem::directory_iterator(dir)) {
    if (std::filesystem::is_directory(file)) {
      string dir_name(file.path().filename());
      if (dir_name != "" && std::all_of(dir_name.begin(), dir_name.end(), isdigit)) {
        int pid = stoi(dir_name);
        pids.push_back(pid);
      }
    }
  }
  return pids;
}
*/

// Extracts the value of a meminfo file line
int getNextValueFromLine(std::ifstream& stream) {
  string line, key;
  int value = 0;
  std::getline(stream, line);
  std::istringstream linestream(line);
  linestream >> key >> value;
  return value;
}

// DONE: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() {
  int memTotal, memFree, buffers, cached;
  std::ifstream fs(kProcDirectory + kMeminfoFilename);
  if (fs.is_open()) {
    memTotal = getNextValueFromLine(fs);
    memFree = getNextValueFromLine(fs);
    getNextValueFromLine(fs);
    buffers = getNextValueFromLine(fs);
    cached = getNextValueFromLine(fs);
  }

  return (memTotal != 0) ? (memTotal - memFree - (buffers + cached)) / (float)memTotal
                         : 0;
}

// DONE: Read and return the system uptime
long LinuxParser::UpTime() { 
  long uptime;
  string line;
  std::ifstream fs(kProcDirectory + kUptimeFilename);
  if (fs.is_open()) {
    std::getline(fs, line);
    std::istringstream iss(line);
    iss >> uptime;
  }
  return uptime;
}

// DONE: Read and return CPU utilization
float LinuxParser::CpuUtilization() {
  string line, key;
  long user, nice, system, idle, iowait, irq, softirq, steal;
  static long prevIdle = 0, prevTotal = 0;
  float util;
  std::ifstream fs(kProcDirectory + kStatFilename);
  if (fs.is_open()) {
    std::getline(fs, line);
    std::istringstream iss(line);
    iss >> key >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal;

    long calcIdle = idle + iowait;
    long calcNonIdle = user + nice + system + irq + softirq + steal;
    long calcTotal = calcIdle + calcNonIdle;

    long calcTotalD = calcTotal - prevTotal;
    long calcIdleD = calcIdle - prevIdle;

    prevIdle = calcIdle;
    prevTotal = calcTotal;

    util = (float)((calcTotalD > 0) ? (calcTotalD - calcIdleD) / (double)calcTotalD : 0);
  }

  return util;
}

// DONE: Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  string line, key;
  int totalProcs;
  std::ifstream fs(kProcDirectory + kStatFilename);
  if (fs.is_open()) {
    while(getline(fs, line)) {
      std::istringstream iss(line);
      iss >> key;
      if (key == "processes") {
        iss >> totalProcs;
        break;
      }
    }
  }

  return totalProcs;
}

// DONE: Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  string line, key;
  int runningProcs;
  std::ifstream fs(kProcDirectory + kStatFilename);
  if (fs.is_open()) {
    while(getline(fs, line)) {
      std::istringstream iss(line);
      iss >> key;
      if (key == "procs_running") {
        iss >> runningProcs;
        break;
      }
    }
  }

  return runningProcs;
}

// DONE: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid) {
  string cmd = "";

  std::ifstream fs(kProcDirectory + "/" + to_string(pid) + kCmdlineFilename);
  if (fs.is_open()) {
    getline(fs, cmd);
  }

  return cmd;
}

// DONE: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid) {
  string line, key;
  long mem;
  string memStr;
  std::ifstream fs(kProcDirectory + "/" + to_string(pid) + kStatusFilename);
  if (fs.is_open()) {
    while (getline(fs, line)) {
      std::istringstream iss(line);
      iss >> key;
      if (key == "VmSize:") {
        iss >> mem;
        if (mem > 1000)
    	  memStr = to_string(mem / 1000) + " mB";
        else
          memStr = to_string(mem) + "kB";
        break;
      }
    }
  }  

  return memStr;
}

// DONE: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid) {
  string line, key, uid;
  std::ifstream fs(kProcDirectory + "/" + to_string(pid) + kStatusFilename);
  if (fs.is_open()) {
    while (getline(fs, line)) {
      std::istringstream iss(line);
      iss >> key;
      if (key == "Uid:") {
        iss >> uid;
        break;
      }
    }
  }

  return uid;
}

// DONE: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid) {
  string uid = Uid(pid);

  string line, username = "unknown", pw, potentialUid;
  std::ifstream fs(kPasswordPath);
  if (fs.is_open()) {
    while (getline(fs, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream iss(line);
      iss >> username >> pw >> potentialUid;
      if (uid == potentialUid)
        break;
    }
  }

  return username;
}

// DONE: Read and return CPU utilization
float LinuxParser::CpuUtilization(int pid) {
  string line, trash;
  long utime = 0, stime = 0, cutime = 0, cstime = 0, starttime = 0;

  std::ifstream fs(kProcDirectory + "/" + to_string(pid) + kStatFilename);
  if (fs.is_open()) {
    getline(fs, line);
    std::istringstream iss(line);
    for (auto i = 0; i < 13; ++i)
      iss >> trash;
    iss >> utime >> stime >> utime >> cstime;
    for (auto i = 0; i < 4; ++i)
      iss >> trash;
    iss >> starttime;
  }
  
  utime     /= sysconf(_SC_CLK_TCK);
  stime     /= sysconf(_SC_CLK_TCK);
  cutime    /= sysconf(_SC_CLK_TCK);
  cstime    /= sysconf(_SC_CLK_TCK);
  starttime /= sysconf(_SC_CLK_TCK);
  
  long seconds = UpTime() - starttime;
  return (float)(((utime + stime + cutime + cstime) / (double)seconds));
}

// DONE: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid) {
  string line, trash;
  long uptime;

  std::ifstream fs(kProcDirectory + "/" + to_string(pid) + kStatFilename);
  if (fs.is_open()) {
    getline(fs, line);
    std::istringstream iss(line);
    for (auto i = 0; i < 21; ++i)
      iss >> trash;
    iss >> uptime;
  }
  return UpTime() - (uptime / sysconf(_SC_CLK_TCK));
}