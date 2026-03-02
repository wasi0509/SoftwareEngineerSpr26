#include "WorldState.h"
#include <iostream>
#include <sstream>

/*
Parse simulator response line by line.
Extract:
    cycle from "CYCLE N"
    rows & cols from "MAP x y"
    grid from "ROW ..." lines
Print grid to console.
Handles hexagonal offset because simulator adds a leading space on odd rows — we just print what we receive.
*/
WorldState::WorldState(const std::vector<std::string> &lines)
{

    for (const auto &line : lines)
    {

        if (line.rfind("CYCLE", 0) == 0)
        {
            std::stringstream ss(line);
            std::string temp;
            ss >> temp >> cycle;
        }

        else if (line.rfind("MAP", 0) == 0)
        {
            std::stringstream ss(line);
            std::string temp;
            ss >> temp >> rows >> cols;
        }

        else if (line.rfind("ROW", 0) == 0)
        {
            grid.push_back(line.substr(4));
        }
    }
}

void WorldState::print() const
{

    std::cout << "Cycle: " << cycle << "\n";

    for (const auto &row : grid)
        std::cout << row << "\n";

    std::cout << "\n";
}