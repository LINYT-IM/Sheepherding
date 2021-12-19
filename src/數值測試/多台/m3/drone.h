#ifndef DRONE_H
#define DRONE_H
#include "sheep.h"
#include "line.h"
#include <utility>
struct Line;

class Sheep;

struct Player {
    Player(){};
    Player(float x, float y, float p){
        pos = std::pair<float, float>(x, y);
        path = p;
    }
    std::pair <float, float> pos;
    std::pair <float, float> currV;
    float path;
    int sta;
    // Line path;
};

void playerMover(Player *player, int limit, int m);

#endif