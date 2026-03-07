#include "Simulator.h"

#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

/*
mkdir + mkfifo          Create temporary directory and two FIFOs (command & data pipe).
fork()                  Create child process for simulator.
execl()                 Replace child process with simulator executable. Arguments match:
        ./bin/sim --cmd-pipe /tmp/bugworld_<pid>/cmd.pipe \
                --data-pipe /tmp/bugworld_<pid>/data.pipe \
                <world_file> <red_bug> <black_bug>
Open pipes in order     cmdFd = write, dataFd = read. Matches simulator’s expectation.
usleep(100ms)           Avoid deadlock because simulator blocks until pipe opened.
sendStep()              Sends text command STEP N\n.
readWorld()             Read text lines from simulator until "END". Stores in WorldState.
stop()                  Sends QUIT, closes pipes, waits child, deletes FIFOs and directory.
*/
Simulator::Simulator(const std::string &world, const std::string &redBug, const std::string &blackBug) : worldFile(world), redBugFile(redBug), blackBugFile(blackBug)
{
    baseDir = "/tmp/bugworld_" + std::to_string(getpid());
    cmdPipe = baseDir + "/cmd.pipe";
    dataPipe = baseDir + "/data.pipe";
}

void Simulator::createPipes()
{
    if (mkdir(baseDir.c_str(), 0700) == -1)
    {
        perror("mkdir failed");
        exit(1);
    }

    if (mkfifo(cmdPipe.c_str(), 0666) == -1)
    {
        perror("mkfifo cmd failed");
        exit(1);
    }

    if (mkfifo(dataPipe.c_str(), 0666) == -1)
    {
        perror("mkfifo data failed");
        exit(1);
    }

    std::cout << "Pipes created successfully!\n";
}

void Simulator::launchSimulator()
{
    simPid = fork();
    // CHILD → launch sim
    if (simPid == 0)
    {
        execl("../bin/sim", "sim",
              "--cmd-pipe", cmdPipe.c_str(),
              "--data-pipe", dataPipe.c_str(),
              worldFile.c_str(),
              redBugFile.c_str(),
              blackBugFile.c_str(),
              NULL);
        perror("exec failed");
        exit(1); // if execl fails
    }
}

bool Simulator::start()
{
    // PARENT

    // Open pipes in correct order

    createPipes();
    launchSimulator();
    std::cout << "Forked simulator with PID: " << simPid << std::endl;
    usleep(500000); // 0.5s to let simulator open pipes

    cmdFd = open(cmdPipe.c_str(), O_WRONLY); // Open in non-blocking mode and retry:
    while ((cmdFd = open(cmdPipe.c_str(), O_WRONLY)) < 0)
    {
        usleep(50000); // wait 50ms
        cmdFd = open(cmdPipe.c_str(), O_WRONLY);
        perror("open cmd");
    }
    dataFd = open(dataPipe.c_str(), O_RDONLY);

    if (cmdFd < 0 || dataFd < 0)
    {
        perror("open data");
        return false;
    }

    return true;
}

void Simulator::sendStep(int n)
{
    // Send STEP command
    std::string command = "STEP " + std::to_string(n) + "\n";
    write(cmdFd, command.c_str(), command.size());
}

WorldState Simulator::readWorld()
{
    // Read response
    std::vector<std::string> lines;
    char buffer[256];
    std::string current;

    while (true)
    {

        int bytes = read(dataFd, buffer, sizeof(buffer) - 1);

        if (bytes == 0)
            break; // simulator closed pipe

        if (bytes < 0)
        {
            perror("read");
            break;
        }

        buffer[bytes] = '\0';
        current += buffer;

        std::cout << "Simulator says:\n"
                  << buffer << std::endl;
        size_t pos;
        while ((pos = current.find('\n')) != std::string::npos)
        {
            std::string line = current.substr(0, pos);
            current.erase(0, pos + 1);

            lines.push_back(line);

            if (line == "END")
            {
                return WorldState(lines);
            }
        }
    }
    return WorldState(lines);
}

void Simulator::stop()
{

    std::string quit = "QUIT\n";
    write(cmdFd, quit.c_str(), quit.size());

    close(cmdFd);
    close(dataFd);

    waitpid(simPid, nullptr, 0);

    unlink(cmdPipe.c_str());  // remove pipe
    unlink(dataPipe.c_str()); // remove pipe
    rmdir(baseDir.c_str());   // remove pipe
}
