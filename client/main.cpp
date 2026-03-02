#include "Simulator.h"
#include <iostream>

// Read command line
// Creates client
// Runs loop

// g++ main.cpp Simulator.cpp WorldState.cpp -o client -std=c++17
// -->  -std=c++17 ensures C++17 features are supported.
//./client ../worlds/tiny.world ../bugs/simple.bug ../bugs/simple.buggy
// B → black bug, R → red bug, + → red nest, - → black nest, 1..9 → food units.

int main(int argc, char *argv[])
{
    if (argc < 4)
    {
        std::cout << "Usage: ./client <world> <red_bug> <black_bug>\n";
        return 1;
    }

    std::string world = argv[1];
    std::string redBug = argv[2];
    std::string blackBug = argv[3];

    Simulator client(world, redBug, blackBug);

    if (!client.start()) // Start sim, create pipes
    {
        std::cout << "Failed to start simulator\n";
        return 1;
    }

    // Simple loop: run 10 steps
    for (int i = 0; i < 10; i++)
    {
        client.sendStep(1);                    // Send STEP 1 command over the command pipe.
        WorldState state = client.readWorld(); // Reads the simulator response from the data pipe, parses into our internal WorldState object.
        state.print();                         /// Displays the world on terminal.
    }

    client.stop(); // quit sim, close pipes, wait for sim to exit,delete temporary filese
    return 0;
}