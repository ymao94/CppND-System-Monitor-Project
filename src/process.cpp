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
using namespace std::this_thread;     // sleep_for, sleep_until
using namespace std::chrono;

Process::Process(int pid) : pid_(pid) {}
// TODO: Return this process's ID
int Process::Pid() { return pid_; }

// TODO: Return this process's CPU utilization
float Process::CpuUtilization() const { 
    /*
    string line;
    std::regex re(" ");
        std::ifstream stream("/proc/"+to_string(this->pid_)+"/stat");
    if (stream.is_open()) {
    std::getline(stream, line);
    vector<string> vec = vector<string>(std::sregex_token_iterator{begin(line), end(line), re, -1}, std::sregex_token_iterator{});
 */

    vector<string> vec = LinuxParser::StatFileParser(pid_);

    if (vec.size() == 0)
        return 0.0f;
    
    double total_time = (stol(vec[10]) + stol(vec[11]) + stol(vec[12]) + stol(vec[13])) / sysconf(_SC_CLK_TCK);
    double seconds = LinuxParser::UpTime() - stol(vec[18]) / sysconf(_SC_CLK_TCK); 
    float cpu_usage = total_time / seconds;
    return cpu_usage;
    }

   //in order to avoid problems, I change here to const

// TODO: Return the command that generated this process
string Process::Command() { return LinuxParser::Command(pid_); }

// TODO: Return this process's memory utilization
string Process::Ram() { return LinuxParser::Ram(pid_); }

// TODO: Return the user (name) that generated this process
string Process::User() { return LinuxParser::User(pid_); }

// TODO: Return the age of this process (in seconds)
long Process::UpTime() { return LinuxParser::UpTime(pid_); }

// TODO: Overload the "less than" comparison operator for Process objects
// REMOVE: [[maybe_unused]] once you define the function
bool Process::operator<(Process const& a) const { 
    return (this->CpuUtilization() > a.CpuUtilization());
}