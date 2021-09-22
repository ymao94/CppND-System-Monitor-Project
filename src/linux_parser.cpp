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
      if (key == filterMemTotalString)
         total = std::stof(value);
      else if (key == filterMemFreeString)
        {free = std::stof(value);
        return 1 - free / total;
        }
      }
    }
  }
  return 1 - free / total;
}


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


long LinuxParser::Jiffies() { return 0; }


long LinuxParser::ActiveJiffies(int pid[[maybe_unused]]) { return 0; }

long LinuxParser::ActiveJiffies() { return 0; }


long LinuxParser::IdleJiffies() { return 0; }


vector<string> LinuxParser::CpuUtilization() { return {}; }


int LinuxParser::TotalProcesses() { 
  string line;
  string key;
  string value;
  std::ifstream stream(kProcDirectory+kStatFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      while(linestream >> key >> value) {
      if (key == filterProcesses)
        return stoi(value);
      }
    }
  }
  return stoi(value);
 }


int LinuxParser::RunningProcesses() {
  string line;
  string key;
  string value;
  std::ifstream stream(kProcDirectory+kStatFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      while(linestream >> key >> value) {
      if (key == filterRunningProcesses)
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
  // vector<string> state{"S", "R", "D", "Z", "T", "t", "W", "X", "x", "K", "W", "P"};
  string command = "";
  char end{')'};

  std::ifstream stream(kProcDirectory+to_string(pid)+kStatFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> value; // record the pid
      statfile.push_back(value);
      while(linestream >> value && value.back() != end) {
        command += value;
      }
      command += value;
      statfile.push_back(command);

      while(linestream >> value) {
        statfile.push_back(value);
 //       if (std::find(state.begin(), state.end(), value) != state.end()) {
 //         statfile.clear(); // avoid parsing fault for process with name such as (web content)
        }
      }
    }
  return statfile; 

}



string LinuxParser::Command(int pid) {
  string command;
  std::size_t max_size = 40;
  std::ifstream stream(kProcDirectory+to_string(pid)+kCmdlineFilename);
  if (stream.is_open()) {
    std::getline(stream, command);
    if (command.size() > max_size) {
      command.resize(max_size - 3);
      command += "...";
    }
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
        if (key == filterProcMem){
          int ram = stoi(value) / 1000;
          return to_string(ram);
        }
      }
    }
  }
  return value;}

string LinuxParser::Uid(int pid) { 
  string line;
  string key;
  string value;
  std::ifstream stream(kProcDirectory+to_string(pid)+kStatusFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      while(linestream >> key >> value) {
      if (key == filterUID)
        return value;
      }
    }
  }
  return value; 
}


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
}

long LinuxParser::UpTime(int pid) {
 
  vector<string> statfile = StatFileParser(pid);
  if (statfile.size() == 0)
    return 0;
  long starttime = UpTime() - stol(statfile[21]) / sysconf(_SC_CLK_TCK);
  return starttime;
}