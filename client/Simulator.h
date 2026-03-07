#ifndef SIMULATOR_H
#define SIMULATOR_H

#include "WorldState.h"
#include <string>

/*
handles pipes and launching sim
baseDir     /tmp/bugworld_<pid>; a temporary directory for FIFOs.
cmdPipe     baseDir/cmd.pipe, used to send commands to simulator.
dataPipe    baseDir/data.pipe, used to read responses from simulator.
cmdFd &     dataFd → file descriptors returned by open().
simPid      PID of the simulator child process.

Methods map directly to the 7 steps:
    1.create pipes → createPipes()
    2.launch sim → launchSimulator()
    3.open pipes in correct order → start()
    4.send commands → sendStep()
    5.read response → readWorld()
    6.parse & display → WorldState
    7.cleanup → stop()
*/

class Simulator
{
public:
    Simulator(const std::string &world,
              const std::string &redBug,
              const std::string &blackBug);

    bool start();           // start simulator, and open pipes
    void sendStep(int n);   // send step n
    WorldState readWorld(); // read response until END, parse into WorldState
    void stop();            // quit sim, close pipes, wait for sim to exit,delete temporary files
private:
    std::string worldFile;
    std::string redBugFile;
    std::string blackBugFile;

    std::string baseDir;  // Temporary directory in /tmp
    std::string cmdPipe;  /// Command pipe path
    std::string dataPipe; // Data pipe path

    int cmdFd;  // mkdir + mkfifo
    int dataFd; /// fork + execl
    pid_t simPid;

    void createPipes();
    void launchSimulator();
};

#endif