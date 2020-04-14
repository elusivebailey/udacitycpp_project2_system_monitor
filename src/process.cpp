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

// Added Process Class constructor
Process::Process(int pid) : pid_(pid) {}

// Return this process's ID
int Process::Pid() {
  return (this->pid_);
}

// Return this process's CPU utilization
float Process::CpuUtilization() const {
  float cpu_usage, total_time, seconds;
  string val;
  string line;
  vector<string> values;

  const std::string kProcDirectory{"/proc/"};
  const std::string kStatFilename{"/stat"};

  // Should probably be in LinuxParser
  // https://www.tutorialspoint.com/stringstream-in-cplusplus
  std::ifstream stream(kProcDirectory + to_string(this->pid_) + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    while(linestream >> val){
      values.push_back(val);
    }
  }

 // Assign and convert to floats (14, 15, 16, 17, and 22)
  float uptime    = (float)LinuxParser::UpTime();
  float utime     = std::stof(values[13]);
  float stime     = std::stof(values[14]);
  float cutime    = std::stof(values[15]);
  float cstime    = std::stof(values[16]);
  float starttime = std::stof(values[21]);

  // https://stackoverflow.com/questions/16726779/how-do-i-get-the-total-cpu-usage-of-an-application-from-proc-pid-stat/16736599#16736599
  total_time = utime + stime;
  total_time = total_time + cutime + cstime;
  seconds = uptime - (starttime / sysconf(_SC_CLK_TCK));
  cpu_usage = 100 * ((total_time / sysconf(_SC_CLK_TCK)) / seconds);

  return cpu_usage;
}

// Return the command that generated this process
string Process::Command() {
  return LinuxParser::Command(this->pid_);
}

// Return this process's memory utilization
string Process::Ram() {
  return LinuxParser::Ram(this->pid_);
}

// Return the user (name) that generated this process
string Process::User() {
  return LinuxParser::User(this->pid_);
}

// Return the age of this process (in seconds)
long int Process::UpTime() {
  return LinuxParser::UpTime(this->pid_);
}

// Overload the "less than" comparison operator for Process objects
bool Process::operator<(Process const& a) const {
  return a.CpuUtilization() < this->CpuUtilization();
}
