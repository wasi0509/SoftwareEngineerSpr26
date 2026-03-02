#ifndef WORLD_STATE_H
#define WORLD_STATE_H

#include <vector>
#include <string>

// WorldState stores parsed world from the simulator response.

class WorldState
{
public:
    WorldState() {}
    WorldState(const std::vector<std::string> &lines);

    void print() const;

private:
    int cycle;
    int rows;
    int cols;
    std::vector<std::string> grid;
};

#endif