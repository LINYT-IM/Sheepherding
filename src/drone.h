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
    }
    std::pair <float, float> pos;
    std::pair <float, float> currV;
    Line path;
};

void playerMover(Player *player, int limit, int m);

#endif