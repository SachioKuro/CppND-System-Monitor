#include <sstream>
#include <string>

#include "format.h"

using std::string;

// DONE: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds) {
  long hours = seconds / 3600;
  // remove elapsed hours from seconds
  seconds = seconds % 3600;
  int minutes = seconds / 60;
  // remove elapsed minutes from seconds
  seconds = seconds % 60;

  std::stringstream ss;
  ss << hours
     << ":"
     // prepend minutes with 0 if it is single-digit
     << ((minutes < 10) ? "0" : "") << minutes
     << ":"
     // prepend seconds with 0 if it is single-digit
     << ((seconds < 10) ? "0" : "") << seconds;

  return ss.str();
}
