#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "process.h"
#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

// DONE: Return this process's ID
int Process::Pid() const { return this->pid_; }

void Process::Pid(int pid) { this->pid_ = pid; }

// DONE: Return this process's CPU utilization
float Process::CpuUtilization() { 
  this->lastUtil_ = LinuxParser::CpuUtilization(this->pid_);
  return this->lastUtil_;
}

float Process::LastUtil() const { return this->lastUtil_; }

// DONE: Return the command that generated this process
string Process::Command() const { return this->cmd_; }

void Process::Command(const string& cmd) { this->cmd_ = cmd; }

// DONE: Return this process's memory utilization
string Process::Ram() const { return LinuxParser::Ram(this->pid_); }

// DONE: Return the user (name) that generated this process
string Process::User() const { return this->user_; }

void Process::User(const string& user) { this->user_ = user; }

// DONE: Return the age of this process (in seconds)
long int Process::UpTime() const { return LinuxParser::UpTime(this->pid_); }

// DONE: Overload the "less than" comparison operator for Process objects
// REMOVE: [[maybe_unused]] once you define the function
bool Process::operator<(Process const& a) const { return this->lastUtil_ < a.LastUtil(); }
