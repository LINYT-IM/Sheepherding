#ifndef SHEEP_H
#define SHEEP_H
#include "drone.h"

#include <vector>
#include <utility>
struct Player;

class Sheep{
public:
    Sheep(){};
    Sheep(float x, float y, int status);
    std::pair <float, float> getPos();
    int getSta();
    std::pair<float, float> getV();
    void setData(int x, int y, int status, int index);
    void setStatus(int stat);
    void randMove(int limit, std::pair <float, float> v0 = std::pair <float, float>(0, 0));
    void calV0(Player* player);
    void runMode(int limit);
    void playerReset();
    void addPlayer(int ind);
    int getIndex();
private:
    int status;
    int index;
    float x;
    float y;
    std::pair<float, float> currV;
    // 被哪幾台無人機影響
    std::vector <int> player_index;
};

void sheepGather(Sheep *sheeps, int far_sheep, std::pair<float, float> lcm, int limit);

void sheepMover(Sheep* sheeps, Player *player, int n, int limit);

bool checkStatus(Sheep *sheeps, int n);

std::pair<float, float> calculateGCM(Sheep *sheeps, int n);

std::pair<float, float> calculateLCM(Sheep *sheeps, int index, int n);

#endif