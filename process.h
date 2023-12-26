#ifndef PROCESS_H
#define PROCESS_H

#include <iomanip>
#include <string>
#include <vector>
#include <chrono>
#include <thread>
#include <time.h>
#include <iostream>
#include <fstream>
#include <csignal>
#include <unistd.h>
#include <sys/resource.h>
#include <sys/types.h>
using namespace std;
class Process
{
private:
//: id процесу, ім’я процесу, стан процесу, пріоритет процесу,
//час виконання та іншої інформації.
    //SIGSEGV
    pid_t processID;
    int processId;
    bool running = false;
    string processName;
    int processExecutionTime;
    int procPrio;
    string procState;
    string processPriority;
     std::chrono::time_point<std::chrono::high_resolution_clock> start_time;
public:
     Process(vector<double> a){
         processID = fork();

             if(processID < 0){
                 running = false;
             } else if(processID == 0){

                 if(a.size() != 1){
                 char start[20];
                 char end[20];
                 char N[20];
                 char steps[20];
                 snprintf(start, sizeof(start), "%f", a[0]);
                 snprintf(end, sizeof(end), "%f", a[1]);
                 snprintf(steps, sizeof(steps), "%f", a[2]);
                 snprintf(N, sizeof(N), "%f", a[3]);
                 const char* path = "/home/work/Tabulation_OC";
                 //char* const args[] = {path, start, end, steps, N, NULL};
                 //char* const args[] = {path, NULL};
                 //execl("/home/work/Tabulation_OC","/home/work/Tabulation_OC", &start, &end, &steps, &N, NULL);
                 //execv("/home/work/Tabulation_OC", args);
                 //execl(path, path, start, end, steps, N, NULL);
                 //execl("/home/work/Tabulation_OC", NULL);
                 //execve(path, args, environ);
                 execl(path, path, start, end, steps, N, NULL);
                 }
                if(a[0] == 0 && a.size() == 1) {
                 execv("/home/work/Palindromes", args);
                }
             }
                  else {
                 start_time = std::chrono::high_resolution_clock::now();
                 //processId = (int)processID;

                 std::ifstream commFile("/proc/" + std::to_string(processID) + "/comm");
                 running = true;
                 if (commFile) {
                     commFile >> processName;
                     commFile.close();
                 } else {
                     processName = "";
                 }

                 //processName = "Palindromes";

                 }
             //

    }

     ~Process(){
      kill(processID, SIGKILL);
    }
     Process(pid_t pid) {
         processId = pid;
         start_time = std::chrono::high_resolution_clock::now();
     }
     int getID(){
         return processId;
     }
     pid_t getPID_T(){
         return processID;
     }
     bool getRunning(){
         return running;
     }

     void setRunning(){
         running = !running;
     }
     string getProcessState(){
         return procState;
     }
     string getPriority(){
         return processPriority;
     }

     string getProcName(){
         return processName;
     }
     int getTime()
     {   auto current_time = std::chrono::high_resolution_clock::now();
         auto elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>(current_time - start_time);
         return elapsed_time.count();
     }
     string beautifulShow(){
         int elapsed_time_seconds = getTime() / 1000; // Convert milliseconds to seconds
         int hours = elapsed_time_seconds / 3600;
         int minutes = (elapsed_time_seconds % 3600) / 60;
         int seconds = elapsed_time_seconds % 60;
         stringstream a;
         a <<  std::setfill('0') << std::setw(2) << hours << ":"
           << std::setfill('0') << std::setw(2) << minutes << ":" <<
             std::setfill('0') << std::setw(2) << seconds;

         return a.str();
     }

     int DeathCheck(){
         if(!kill(processID, 0)){
                 return 0;
         }
                 running = false;
                 return 1;
      //   return 1;
     }



     void renewData(){
        processExecutionTime = getTime();
        procPrio = getpriority(PRIO_PROCESS, processID);
        processPriority = getPriorityDescription();
        getProcState();
        if(procState == "X"){
                 running = false;
        }
     }

     string getPriorityDescription() {
        string description;

        switch (getpriority(PRIO_PROCESS, processID)) {
        case SCHED_IDLE:
                 description = "IDLE";
                 break;
        case SCHED_BATCH:
                 description = "LOW";
                 break;
        case SCHED_OTHER:
                 description = "NORMAL";
                 break;
        case SCHED_RR:
                 description = "REALTIME";
                 break;
        case SCHED_FIFO:
                 description = "HIGH";
                 break;
        default:
                 description = "NORMAL";
                 break;
        }

        return description;
     }

     void getProcState(){
                 std::string proc_status_file = "/proc/" + std::to_string(processID) + "/status";
                 std::ifstream status_file(proc_status_file);

                 if (status_file.is_open()) {
                 std::string line;
                 while (std::getline(status_file, line)) {
                     if (line.find("State:") != std::string::npos) {
                         std::istringstream iss(line);
                         std::string field;
                         iss >> field >> procState;
                         break;
                     }
                 }
                 status_file.close();
                 }
     }



     void setStrPriority(){}
     bool SuspendProcess() {
                 if(kill(processID, SIGSTOP) == 0){
                 return 0;
                 } return 1;
     }
     int KillProcess(){

                 if(kill(processID, SIGKILL)==0){
                 //waitpid(processID);
                 running = false;
                 return 0;
                 } else{return 1;}
     }
     bool ResumeProcess() {
                 if(kill(processID, SIGCONT) == 0){
                 return 0;
                 } return 1;
     }
     void setPriority(int a){
                 setpriority(PRIO_PROCESS, processID, a);
     }
     bool setAffinity(string affin){
                 cpu_set_t cpuset;
                 CPU_ZERO(&cpuset); // Initialize the CPU set to empty

                 // Loop through the binary string and set CPU affinity for each core
                 for (size_t i = 0; i < affin.size(); i++) {
                 if (affin[i] == '1') {
                     CPU_SET(i, &cpuset);
                 }
                 }

                 // Set the CPU affinity for the specified process
                 int result = sched_setaffinity(processID, sizeof(cpuset), &cpuset);
                 return result;

     }
};

#endif // PROCESS_H
