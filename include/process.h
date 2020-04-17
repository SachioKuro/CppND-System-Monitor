#ifndef PROCESS_H
#define PROCESS_H

#include <string>
/*
Basic class for Process representation
It contains relevant attributes as shown below
*/
class Process {
 public:
  int Pid() const;                         // DONE: See src/process.cpp
  void Pid(int pid);
  std::string User() const;                // DONE: See src/process.cpp
  void User(const std::string& user);
  std::string Command() const;             // DONE: See src/process.cpp
  void Command(const std::string& cmd);
  float CpuUtilization();                  // DONE: See src/process.cpp
  float LastUtil() const;
  std::string Ram() const;                 // DONE: See src/process.cpp
  long int UpTime() const;                 // DONE: See src/process.cpp
  bool operator<(Process const& a) const;  // DONE: See src/process.cpp

  // DONE: Declare any necessary private members
 private:
  int pid_;
  std::string user_;
  std::string cmd_;
  
  float lastUtil_;
};

#endif
