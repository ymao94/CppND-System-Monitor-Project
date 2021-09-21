#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>
#include "linux_parser.h"
#include <chrono>
#include <thread>

#include "process.h"

using std::string;
using std::to_string;
using std::vector;
using std::stol;


Process::Process(int pid) : pid_(pid) {}
int Process::Pid() { return pid_; }

float Process::CpuUtilization() const { 
    vector<string> vec = LinuxParser::StatFileParser(pid_);

    if (vec.size() == 0)
        return 0.0f;
    
    double total_time = (stol(vec[13]) + stol(vec[14]) + stol(vec[15]) + stol(vec[16])) / sysconf(_SC_CLK_TCK);
    double seconds = LinuxParser::UpTime() - stol(vec[21]) / sysconf(_SC_CLK_TCK); 
    float cpu_usage = total_time / seconds;
    return cpu_usage;
    }

   //in order to avoid problems, I change here to const


string Process::Command() { return LinuxParser::Command(pid_); }


string Process::Ram() { return LinuxParser::Ram(pid_); }

string Process::User() { return LinuxParser::User(pid_); }

long Process::UpTime() { return LinuxParser::UpTime(pid_); }

bool Process::operator<(Process const& a) const { 
    return (this->CpuUtilization() > a.CpuUtilization());
}