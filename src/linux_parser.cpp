#include <dirent.h>
#include <unistd.h>
#include <string>
//#include <vector>
#include <iostream>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

//upper part

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
  string os, version, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
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

// TODO: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() { 
  string line;
  string key;
  string value;
  string kb;
  float total, free;
  std::ifstream stream(kProcDirectory+kMeminfoFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      while(linestream >> key >> value >> kb) {
      if (key == "MemTotal:")
         total = std::stof(value);
      else if (key == "MemFree:")
        {free = std::stof(value);
        return 1 - free / total;
        }
      }
    }
  }
  return 1 - free / total;
}

// TODO: Read and return the system uptime
long LinuxParser::UpTime() { 
  string uptime, idle_time;
  string line;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> uptime >> idle_time;
  }

  long up_time = std::stol(uptime); // convert the string "uptime" to data type long
  return up_time;

}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { return 0; }

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid[[maybe_unused]]) { return 0; }

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { return 0; }

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { return 0; }

// TODO: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() { return {}; }

// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() { 
  string line;
  string key;
  string value;
  std::ifstream stream(kProcDirectory+kStatFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      while(linestream >> key >> value) {
      if (key == "processes")
        return stoi(value);
      }
    }
  }
  return stoi(value);
 }

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  string line;
  string key;
  string value;
  std::ifstream stream(kProcDirectory+kStatFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      while(linestream >> key >> value) {
      if (key == "procs_running")
        return stoi(value);
      }
    }
  }
  return stoi(value);
}

//lower part

//helper function, parse the stat file of the stat file of each process
//the information !after! the third term is stored in a vector of strings
vector<string> LinuxParser::StatFileParser(int pid) {
  string line;
  string value;
  vector<string> statfile;
  vector<string> state{"S", "R", "D", "Z", "T", "t", "W", "X", "x", "K", "W", "P"};

  std::ifstream stream(kProcDirectory+to_string(pid)+kStatFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      while(linestream >> value) {
        statfile.push_back(value);
        if (std::find(state.begin(), state.end(), value) != state.end()) {
          statfile.clear(); // avoid parsing fault for process with name such as (web content)
        }
      }
    }
  }
  return statfile; 

}



// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid) {
  string command;
  std::ifstream stream(kProcDirectory+to_string(pid)+kCmdlineFilename);
  if (stream.is_open()) {
    std::getline(stream, command);
  }
  return command; }

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid) { 
  string line;
  string key;
  string value;
  std::ifstream stream(kProcDirectory+to_string(pid)+kStatusFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      while(linestream >> key >> value) {
        if (key == "VmSize:"){
          int ram = stoi(value) / 1000;
          return to_string(ram);
        }
      }
    }
  }
  return value;}

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid) { 
  string line;
  string key;
  string value;
  std::ifstream stream(kProcDirectory+to_string(pid)+kStatusFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      while(linestream >> key >> value) {
      if (key == "Uid:")
        return value;
      }
    }
  }
  return value; 
}

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid) { 
  string line;
  string key;
  string value;
  string x;

  string UID = Uid(pid);

  std::ifstream stream(kPasswordPath);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while(linestream >> key >> x >> value) {
      if (value == UID)
        return key;
      }
    }
  }
  return key; 

 /* string line;
  const std::regex re(":");

  string UID = Uid(pid);

  std::ifstream stream(kPasswordPath);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      vector<string> vec = vector<string>(std::sregex_token_iterator{begin(line), end(line), re, -1}, std::sregex_token_iterator{});
      // vector<string> vec = (std::sregex_token_iterator{begin(line), end(line), re, -1}, std::sregex_token_iterator{});
      if (vec[2] == UID) {
        return vec[0];
        }
      }
    }*/

  }


// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid) {
 /* string line;
  std::regex re(" ");

  std::ifstream stream(kProcDirectory+to_string(pid)+kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    vector<string> vec = vector<string>(std::sregex_token_iterator{begin(line), end(line), re, -1}, std::sregex_token_iterator{});
    return stol(vec[21])/sysconf(_SC_CLK_TCK);
  }*/
  vector<string> statfile = StatFileParser(pid);
  if (statfile.size() == 0)
    return 0;
  long starttime = UpTime() - stol(statfile[18]) / sysconf(_SC_CLK_TCK);
  return starttime;
}