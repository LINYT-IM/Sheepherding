// #include <cmath> 
#include "drone.h"
#include "line.h"

void playerMover(Player *player, int limit, int m){
    for(int i = 0; i < m; i++){
        // cout << player[i].currV.first << ", " << player[i].currV.second << endl;
        // if(player[i].pos.first + player[i].currV.first > limit || player[i].pos.first + player[i].currV.first < 0){
        //     if(player[i].pos.first + player[i].currV.first > limit){
        //         player[i].pos.first = limit;
        //     }else{
        //         player[i].pos.first = 0;
        //     }
        // }else{
        //     // move
        //     player[i].pos.first += player[i].currV.first;
        // }
        // if(player[i].pos.second + player[i].currV.second > limit || player[i].pos.second + player[i].currV.second < 0){
        //     if(player[i].pos.second + player[i].currV.second > limit){
        //         player[i].pos.second = limit;
        //     }else{
        //         player[i].pos.second = 0;
        //     }
        // }else{
        //     // move
        //     player[i].pos.second += player[i].currV.second;
        // }
        // 無人機無視範圍限制
        player[i].path += sqrt(pow(player[i].currV.first,2)+pow(player[i].currV.second,2));
        player[i].pos.first += player[i].currV.first;
        player[i].pos.second += player[i].currV.second;
        // player[i].path.p2 = Point(player[i].pos.first, player[i].pos.second);
    }
}

using namespace std;

