#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>

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

// Read and return the system memory utilization
float LinuxParser::MemoryUtilization() {
  string label, line;
  string memTotal, memFree;
  float memUtilization = 0.0;

  std::ifstream stream(kProcDirectory + kMeminfoFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> label >> memTotal;

    std::getline(stream, line);
    std::istringstream linestream2(line);
    linestream2 >> label >> memFree;
  }

  //https://en.cppreference.com/w/cpp/string/basic_string/stof
  memUtilization = stof(memTotal) - stof(memFree);
  return memUtilization;
}

// Read and return the system uptime
long LinuxParser::UpTime() {
  string line, uptime_system, timespent_idle;

  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> uptime_system >> timespent_idle;
  }

  float upTime = stof(uptime_system);
  return upTime;
}

// Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { return 0; }

// Read and return the number of active jiffies for a PID
long LinuxParser::ActiveJiffies(int pid[[maybe_unused]]) { return 0; }

// Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { return 0; }

// Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { return 0; }

// Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() {
  string cpu, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10;
  string line;

  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> cpu >> v1 >> v2 >> v3 >> v4 >> v5 >> v6 >> v7 >> v8 >> v9 >> v10;
  }

  vector<string> values {v1, v2, v3, v4, v5, v6, v7, v8, v9, v10};
  return values;
}

// Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  string line;
  string key;
  string value;
  int totalprocesses;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "processes") {
          totalprocesses = std::stoi(value);
        }
      }
    }
  }
  return totalprocesses;
}

// Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  string line;
  string key;
  string value;
  int runningprocs;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "procs_running") {
          runningprocs = std::stoi(value);
        }
      }
    }
  }
  return runningprocs;
}

// Read and return the command associated with a process
string LinuxParser::Command(int pid) {
  string line;

  std::ifstream stream(kProcDirectory + to_string(pid) + kCmdlineFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
  }

  return line;
}

// Read and return the memory used by a process
string LinuxParser::Ram(int pid) {
  string line;
  string key;
  string value;
  string value_kilobytes;
  string value_megabytes;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "VmSize:") {
          value_kilobytes = value;
        }
      }
    }
  }
  value_megabytes = std::to_string(0.001 * stof(value_kilobytes));
  return value_megabytes;
}

// Read and return the user ID associated with a process
string LinuxParser::Uid(int pid) {
  string line;
  string key;
  string uid;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> uid) {
        if (key == "Uid:") {
          return uid;
        }
      }
    }
  }
  return uid;
}

// Read and return the user associated with a process
string LinuxParser::User(int pid) {
  // Find the UID associated with the processes then find the user corresponding to that UID
  string user, x, uid_num;
  string line;
  int pid_to_uid = pid;
  string uid = LinuxParser::Uid(pid_to_uid);

  std::ifstream filestream(kPasswordPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> user >> x >> uid_num) {
        if (uid_num == uid) {
          return user;
        }
      }
    }
  }

  return user;
}

// Read and return the uptime of a process
long LinuxParser::UpTime(int pid) {
  long uptime;
  string val;
  string line;
  vector<string> values;

  // https://www.tutorialspoint.com/stringstream-in-cplusplus
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    while(linestream >> val){
      values.push_back(val);
    }
  }

 // Assign and convert to long (should be 22nd value in file)
  uptime = std::stol( values[21] );

  // Divide the clock tick by computer value
  uptime = uptime / sysconf(_SC_CLK_TCK);

  // To be called by Format::Time() to make HH:MM::SS string
  return uptime;
}
