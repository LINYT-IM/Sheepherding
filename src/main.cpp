#include <iostream>
#include <list> 
#include <stdlib.h>
#include <utility>
#include <cmath>
#include <random> 
#include <climits>
#include <algorithm>
#include "line.h"
#include "sheep.h"
#include "drone.h"
using namespace std;


/*
環境設定:250x250
羊隻大小:1x1
隨機移動速率:0.5/s
逃跑速率:v0 = 1.5/s, a = -1/s
R = 3
Player V = 2*v0 = 3/s

*/
//  int a = -1;

float distanceObj(pair<float, float> obj1, pair<float, float> obj2){
    // cout << obj2.first << " " << obj1.second << " " << endl;
    return sqrt(pow(obj1.first - obj2.first, 2) + pow(obj1.second - obj2.second, 2));
    
}


void generateRandSheep(Sheep *sheeps, int n, int limit, pair <float, float> *list, int randTyp){
    // 真實隨機
    random_device rd;
    default_random_engine generator( rd() );
    int hori, verti;
    int cirR = rand()%(int(limit / 4)) + 1; //生成虛擬範圍
    // cout << cirR << endl;
    if(randTyp == 0){
        // 聚集
        pair<float, float> center(rand()%(limit-2*cirR) + cirR, rand()%(limit-2*cirR) + cirR); //虛擬範圍中心
        // cout << center.first << " " << center.second  << endl;
        for(int i = 0; i < n; i++){
            // srand(i);
            uniform_real_distribution<float> unif(0, 2*cirR);
            hori = unif(generator) + center.first - cirR;
            verti = unif(generator) + center.second - cirR;
            // hori = rand()%(2*cirR) + center.first - cirR;
            // verti = rand()%(2*cirR) + center.second - cirR;
            sheeps[i].setData(hori, verti, 0);
            list[i] = pair<float, float>(hori, verti);
        }
    }else if(randTyp == 1){
        // 分散
        int group = floor(n / 4) + 1; //決定幾個圈圈
        // cout << group << endl;
        int x,y;
        uniform_real_distribution<float> unif(cirR, limit-cirR);
        pair <float, float> centers[group];
        for(int i = 0; i < group; i++){
            // center each circle;
            x = unif(generator);
            y = unif(generator);
            centers[i] = pair <float, float>(x, y);
        }
        int cir = 0; //第幾個圈圈
        int count = 0;
        uniform_real_distribution<float> unif2(-cirR, cirR);
        for(int i = 0; i < n; i++){
            // cout << centers[i].first << " " << centers[i].second << endl;
            hori = unif2(generator) + centers[cir].first; 
            verti = unif2(generator) + centers[cir].second;
            sheeps[i].setData(hori, verti, 0);
            list[i] = pair<float, float>(hori, verti);
            count += 1;
            if(count >= 4){
                count = 0;
                cir+=1;
            }
        }
    }else{
        // random
        for(int i = 0; i < n; i++){
            srand(i);
            uniform_real_distribution<float> unif(0.0, limit);
            hori = unif(generator);
            verti = unif(generator);
            // hori = rand()%limit;
            // verti = rand()%limit;
            sheeps[i].setData(hori, verti, 0);
            list[i] = pair<float, float>(hori, verti);
        }
    }
}


void checkSheepStatus(Sheep *sheeps, int n, Player *player, int limit, int m, pair<float, float> target){
    float dis;
    for(int i = 0; i < n; i++){
        /*
            status = 0: 隨機走動mode
                   = 1: 驚嚇Mode
                   = 2: 抵達終點
        */
    //    if(sheeps[i].getPos().first ==  target.first && sheeps[i].getPos().second == target.second){
       if(distanceObj(sheeps[i].getPos(), target) <= 2){
            // cout << "set sheep " << i << "'s status to 2\n";  
            sheeps[i].setStatus(2);
       }else{
            dis = INT_MAX;
            // find 
            sheeps[i].playerReset();

            // 先判斷無人機在經過的路徑上有沒有影響
            for(int j = 0; j < m; j++){
                dis = distanceObj(sheeps[i].getPos(), player[j].pos);

                // cout << i << " "  << j << ": " << dis << endl;
                if(dis <= 1.5){
                    // cout << "because player " << j << ", set sheep " << i << "'s status to 1\n";
                    sheeps[i].setStatus(1);
                    // sheeps[i].calV0(player);  <- 要等到scan完所有無人機才能算(搬到sheepMover)
                    sheeps[i].addPlayer(j);
                }else{
                    if (!(sheeps[i].getSta() == 1)){
                        // cout << "because player " << j << ", set sheep " << i << "'s status to 0\n";
                        sheeps[i].setStatus(0);
                    }else{
                        // 原本還在逃跑:繼續直到v = 0
                    }
                }
            }
       }
       
    }
}


int findFarest(Sheep *sheeps, pair<float, float> gcm, int n, vector <int> n_ava = {}){
    float maxD = 0;
    int max = -1;
    // cout << *(n_ava.begin()) << endl;
    for(int i = 0; i < n; i++){
        // 符合所有條件(最遠、還沒到終點、沒有被標記(多台無人機時))
        if(distanceObj(sheeps[i].getPos(), gcm) >= maxD && sheeps[i].getSta() != 2 && !(find(n_ava.begin(), n_ava.end(), i) != n_ava.end())){
            max = i;
            maxD = distanceObj(sheeps[i].getPos(), gcm);
        }
    }
    return max;
}


void method3(Player *player, Sheep *sheeps, pair <float, float> target, int n, int limit, int m){
    // 不可選擇的羊隻(更改指定無人機方法)
    vector <int> n_ava;
    int index[m];
    int curr_index;
    float dir_x, dir_y;
    for(int j = 0; j < m; j++){
        index[j] = findFarest(sheeps, target, n, n_ava);
        // cout << index << endl;
        n_ava.push_back(index[j]);

    }
    // 決定指派給哪台無人機
    for(int i = 0; i < m; i++){
        for(int j = i+1; j < m; j++){
            if(distanceObj(sheeps[index[i]].getPos(), player[j].pos)+distanceObj(sheeps[index[j]].getPos(), player[i].pos) <
            distanceObj(sheeps[index[i]].getPos(), player[i].pos)+distanceObj(sheeps[index[j]].getPos(), player[j].pos)){
                // swap
                curr_index = index[i];
                index[i] = index[j];
                index[j] = curr_index;
            }
        }        
    }
    for(int j = 0; j < m; j++){
        // 決定去哪個位置
        if(sheeps[index[j]].getPos().first - target.first == 0){
            dir_x = 0;
        }else{
            dir_x = (sheeps[index[j]].getPos().first - target.first)/abs((sheeps[index[j]].getPos().first - target.first));
        }
        if(sheeps[index[j]].getPos().second - target.second == 0){
            dir_y = 0;
        }else{
            dir_y = (sheeps[index[j]].getPos().second - target.second)/abs((sheeps[index[j]].getPos().second - target.second));
        }

        pair <float, float> tarPos(sheeps[index[j]].getPos().first + dir_x, sheeps[index[j]].getPos().second + dir_y);
        pair <float, float> dir(tarPos.first - player[j].pos.first, tarPos.second - player[j].pos.second);
        // cout << dir_x << " " << dir_y << endl;
        if(dir.first != 0 && dir.second != 0){
            if(abs(dir.first) > 3){
                dir.first /= abs(dir.first)/3;
            }
            if(abs(dir.second) > 3){
                dir.second /= abs(dir.second)/3;
            }
        }else{
            if(dir.first == 0){
                if(abs(dir.second) > 3){
                    dir.second /= abs(dir.second)/3;
                }
            }else{
                if(abs(dir.first) > 3){
                    dir.first /= abs(dir.first)/3;
                }
            }
        }
        // cout << dir.first << " " << dir.second << endl;
        player[j].currV = pair <float, float>(dir.first, dir.second);
        player[j].path.p1 = Point(player[j].pos.first, player[j].pos.second);
    }
}


void method2(Sheep* sheeps, Player *player, int n, int m, pair<float, float> gcm, pair<float, float> lcm, pair <float, float> target, int limit, float c1, float c2 , float c3, float R = 1.5){
    if(gcm.first == lcm.first && gcm.second == lcm.second){
        // 只剩一隻
        // cout << "Change to method3\n";
        method3(player, sheeps, target, n , limit, m);
    }else{
        // float c1, c2, c3, R;
        // c1 = 1;
        // c2 = 1;
        // c3 = 1;
        // R = 1.5;
        float fn = c1*R*pow(n ,2/3);
        float dis1, dis2;
        // cout << fn << endl;
        pair<float, float> dir;
        vector<int> n_ava;
        for(int j = 0; j < m; j++){    
            if(distanceObj(sheeps[findFarest(sheeps, gcm, n, n_ava)].getPos(), gcm) < fn){
                // pd
                // cout << "pd" << endl;
                // 兩台的話可以試試兩邊包夾?
                Point cen(lcm.first, lcm.second);
                Point d(lcm.first + (lcm.first - target.first)*c2*R*n,lcm.second + (lcm.second - target.second)*c2*R*n);
                Point pos1 = rotate(d, cen, 30);
                Point pos2 = rotate(d, cen, -30);
                dis1 = distanceObj(player[j].pos, pair<float, float> (pos1.real(), pos1.imag()));
                dis2 = distanceObj(player[j].pos, pair<float, float> (pos2.real(), pos2.imag()));
                if(dis1 < dis2){
                    dir = pair <float, float>(pos1.real() - player[j].pos.first, pos1.imag() - player[j].pos.second);
                }else{
                    dir = pair <float, float>(pos1.real() - player[j].pos.first, pos1.imag() - player[j].pos.second);
                }
                    
            }else{
                // pc
                // cout << "pc" << endl;
                dir = pair <float, float>(lcm.first + (sheeps[findFarest(sheeps,gcm,n, n_ava)].getPos().first - lcm.first)*c3*R, lcm.second + (sheeps[findFarest(sheeps,gcm,n, n_ava)].getPos().second - lcm.second)*c3*R);
                n_ava.push_back(findFarest(sheeps, gcm, n, n_ava));    
            }
            // calculate player velocity
            float v_x = dir.first - player[j].pos.first;
            float v_y = dir.second - player[j].pos.second;
            if(v_x != 0 && v_y != 0){
                if(abs(v_x) > 3){
                    v_x /= abs(v_x)/3;
                }
                if(abs(v_y) > 3){
                    v_y /= abs(v_y)/3;
                }
                // player[j].currV = pair <float, float>(v_x, v_y);
            }else{
                if(v_x == 0 && v_y == 0){
                    // player[j].currV = pair <float, float>(0, 0);
                }else if(v_x == 0){
                    if(v_y > 3){
                        v_y /= abs(v_y) / 3;
                    }
                    v_x = 0;
                    // player[j].currV = pair <float, float>(0, v_y);
                }else{
                    if(v_x >3){
                        v_x /= abs(v_x) / 3;
                    }
                    v_y = 0;
                    // player[j].currV = pair <float, float>(v_x, 0);
                }

            }
            // 圓弧形跑法（增加垂直向量）
            Point orig(v_x, v_y);
            Point verti = rotate(orig, Point(0, 0), 90);
            // cout << orig.real() << " " <<  orig.imag() << endl;
            // cout << verti.real() << " " <<  verti.imag() << endl;
            verti.real(verti.real() / 10);
            verti.imag(verti.imag() / 10);
            Point res = orig + verti;
            // player[j].currV = pair <float, float>(v_x, v_y);
            player[j].currV = pair <float, float>(res.real(), res.imag());

        }
    }
}


void printStatus(Sheep *sheeps,int n, int time, Player *player, int m){
    cout << "\tTime: " << time << endl;
    for(int i = 0; i < n; i++){
        cout << "Sheep " << i << " : (" <<  sheeps[i].getPos().first << ", " << sheeps[i].getPos().second << "), status = " << sheeps[i].getSta() << endl;  
    }
    for(int i = 0; i < m; i++){
        cout << "Player "<< i << " : (" << player[i].pos.first << ", " << player[i].pos.second << ")" << endl;
    }
}



int main(int argc, char *argv[]){
    // init
    int max_limit = atoi(argv[2]);
    int max_n = atoi(argv[1]);
    for(int limit = max_limit; limit <= max_limit; limit++){
        for(int n = max_n; n <= max_n; n++){
            // for(int rnd = 0; rnd < 5; rnd++){

    // srand(time(NULL));
            // cout << "limit : " << limit << ", n : " << n << endl;

    Sheep *sheeps = new Sheep[n];
    pair <float, float> list[n];
    // 分成聚集(0)、分散兩種(1)
    int randTyp = 0;
    // generateRandSheep(sheeps, n, limit, list, randTyp);
    list[0] = pair<float, float>(31,9);
    list[1] = pair<float, float>(10,40);
    list[2] = pair<float, float>(5,32);
    list[3] = pair<float, float>(11,50);
    list[4] = pair<float, float>(5,2);
    sheeps[0].setData(31,9,0);
    sheeps[1].setData(10,40,0);
    sheeps[2].setData(5,32,0);
    sheeps[3].setData(11,50,0);
    sheeps[4].setData(5,2,0);
    int time = 0;
    int far_sheep = -1;
    bool gameover = false;
    pair <float, float> gcm(0, 0);
    pair <float, float> lcm(0, 0);
    // m:無人機個數
    int m = 1;
    Player *player;
    player = new Player[m];
    for(int i = 0; i < m; i++){
        player[i].pos = pair<float, float>(0, 0);
    }
    pair <float, float> target(14, 18);

    // float c1 = atof(argv[3]);
    // float c2 = atof(argv[4]);
    // float c3 = atof(argv[5]);
    float c1 = 1.33;
    float c2 = 0.03;
    float c3 = 0.9;
    int min = 999999;
    float best_c1 = -1; 
    float best_c2 = -1; 
    float best_c3 = -1; 
    bool overloaded;
    printStatus(sheeps, n, time, player, m);
    cout << "target: (" << target.first << ", " << target.second << ")\n";
    // Game
    // while(c1 <= 1){
    //     c2 = 0.01;
    //     c3 = 0.01;
    //     while(c2 <= 2){
    //         c3 = 0.01;
    //         while(c3 <= 2){
                // cout << c1 << " " << c2 << " " << c3 << endl;
        // overloaded = false;
        // time = 0;
        // gameover = false;
        // for(int i = 0; i < n; i++){
        //     sheeps[i].setData(list[i].first, list[i].second, 0);
        // }
        // player.pos = pair <float, float>(0,0);
        // player.currV = pair <float, float>(0,0);
        // printStatus(sheeps, n, time, player, m);
    while(!gameover){
        // printStatus(sheeps, n, time, player, m);

        // gcm = calculateGCM(sheeps, n);
        // far_sheep = findFarest(sheeps, gcm, n);
        // lcm = calculateLCM(sheeps, far_sheep, n);
        // player movement
        // method2(sheeps, player, n, m, gcm, lcm, target, limit, c1, c2, c3);

        method3(player, sheeps, target, n, limit, m);
        playerMover(player,limit, m);


        
        // 確認羊群狀態(配合player path)
        checkSheepStatus(sheeps, n, player, limit, m, target);
        // 羊群聚集
        // sheepGather(sheeps, far_sheep, lcm, limit);
        sheepMover(sheeps, player, n, limit);


        // check Game status (是否所有羊隻都抵達終點)
        gameover = checkStatus(sheeps, n);
        // cout << gameover << endl;
        time += 1;
        if(time > 50000){
            overloaded = true;
            gameover = true;
        }
    }
    // if(time <= min && !overloaded){
    //     min = time;
    //     best_c1 = c1;
    //     best_c2 = c2;
    //     best_c3 = c3;
    printStatus(sheeps, n, time, player, m);
    //     // cout << c1 << " " << c2 << " " << c3 << endl;
    // }
    // c3+= 0.01;
    // }
    // c2+=0.01;
    // }
    // c1+= 0.01;
    // }
    // printStatus(sheeps, n, time - 1, player, m);
    cout << c1 << "\t" << c2 << "\t" << c3  << "\t" << time - 1 << endl;
    // cout << "best_c1: " << best_c1 << ", best_c2: " << best_c2 << ", best_c3: " << best_c3 << ", best time: " << min - 1 << endl; 
            // }
            }
    }

}



