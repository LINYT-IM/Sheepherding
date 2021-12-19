#ifndef DRONE_H
#define DRONE_H
#include "sheep.h"
#include "line.h"
#include <utility>
struct Line;

class Sheep;

struct Player {
    Player(){};
    Player(float x, float y){
        pos = std::pair<float, float>(x, y);
        status = 0;
        idle = true;
        curr = -2;
    }
    std::pair <float, float> pos;
    std::pair <float, float> currV;
    // Line path;
    float path;
    // in which group
    int status;
    int curr;
    bool idle;
};

void playerMover(Player *player, int limit, int m);

#endif