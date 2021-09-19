#include "processor.h"
#include <vector>
#include <string>
#include <fstream>
#include <regex>
#include <unistd.h>
#include <chrono>
#include <thread>

using std::stol;
using std::string;
using std::vector;
using namespace std::this_thread;     // sleep_for, sleep_until
using namespace std::chrono;
// TODO: Return the aggregate CPU utilization

const std::string kProcDirectory{"/proc/"};
const std::string kStatFilename{"/stat"};

//helper function, return the idle time and non-idle time
vector<float> CPU_Parser() {
    vector<float> time_count; //vector of idle time and non-idle time
    string line;
    string key;
    string  user, nice, system, idle, iowait, irq, softirq, steal, guest, guest_nice;

    std::ifstream stream(kProcDirectory+kStatFilename);
    if (stream.is_open()) {
        while (std::getline(stream, line)) {
            std::istringstream linestream(line);
            linestream >> key >> user >> nice >> system  >> idle >> iowait >> irq >> softirq >> steal >> guest >> guest_nice;
            if (key == "cpu") {
                long usertime = stol(user) - stol(guest);
                long nicetime = stol(nice) - stol(guest_nice);
                long idletime = stol(idle) + stol(iowait);
                long non_idletime = usertime + nicetime + stol(system) + stol (irq) + stol(softirq) + stol(steal);
                time_count.push_back(idletime);
                time_count.push_back(non_idletime);
                return time_count;
            }
        }
    }
    return time_count;
}



float Processor::Utilization() {
    int sleep_time = 1000;
    vector<float> pre_time_count = CPU_Parser(); 
    sleep_for(milliseconds(sleep_time)); // I used usleep(sleep_time) here, but it didn't work.
    vector<float> time_count = CPU_Parser();
    // calculate the CPU utilization during the sleep time
    float uti = (time_count[1] - pre_time_count[1]) / (time_count[0] + time_count[1] - pre_time_count[0] - pre_time_count[1]); 
    return uti; 
    }