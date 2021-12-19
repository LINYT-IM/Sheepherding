#include <iostream>
#include <list> 
#include <stdlib.h>
#include<utility>
#include <cmath>
#include <random> 
#include <climits>
#include <algorithm>
#include "line.h"

using namespace std;


/*
環境設定:250x250
羊隻大小:1x1
隨機移動速率:0.5/s
逃跑速率:v0 = 1.5/s, a = -1/s
R = 3
Player V = 2*v0 = 3/s

*/
 int a = -1;

float distanceObj(pair<float, float> obj1, pair<float, float> obj2){
    // cout << obj2.first << " " << obj1.second << " " << endl;
    return sqrt(pow(obj1.first - obj2.first, 2) + pow(obj1.second - obj2.second, 2));
    
}



struct Player {
    Player(){};
    Player(float x, float y){
        pos = pair<float, float>(x, y);
    }
    pair <float, float> pos;
    pair <float, float> currV;
    Line path;
};



class Sheep{
public:
    Sheep(){};
    Sheep(float x, float y, int status){};
    pair <float, float> getPos(){
        return pair <float, float> (x, y);
    }
    int getSta(){
        return this->status;
    }
    pair<float, float> getV(){
        return currV;
    }
    void setData(int x, int y, int status);
    void setStatus(int stat){
        this->status = stat;
    };
    void randMove(int limit, pair <float, float> v0);
    void calV0(Player* player, int type);
    void runMode(int limit);
    void playerReset(){
        this->player_index.clear();
        this->player_index_p.clear();
    }
    void addPlayer(int ind){
        this->player_index.push_back(ind);
    }
    void addPlayerP(int ind){
        this->player_index_p.push_back(ind);
    }
private:
    int status;
    float x;
    float y;
    pair<float, float> currV;
    // 被哪幾台無人機影響
    vector <int> player_index;
    // path
    vector <int> player_index_p;
};

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

void Sheep::setData(int x, int y, int status){
    this->x = x; 
    this->y = y;
    this->status = status;
}

void Sheep::randMove(int limit, pair <float, float> v0 = pair <float, float>(0, 0)){
    random_device rd;
    default_random_engine generator( rd() );
    uniform_int_distribution<int> unif(-1, 1);
    float hori;
    float verti;
    if(v0.first == 0 && v0.second == 0){
        hori = floor(unif(generator)) / 2.0; //-0.5, 0, 0.5
        verti = floor(unif(generator)) / 2.0; //-0.5, 0, 0.5
        // cout << hori << " " << verti << endl;
    }else{
        hori = v0.first;
        verti = v0.second;
    }
    // cout << hori << " " << verti << endl;
    // 碰撞判斷
    if(this->x + hori > limit || this->x + hori < 0 || this->y + verti > limit || this->y + verti < 0){
        if(this->x + hori > limit){
            this->x = limit;
        }else if(this->x + hori < 0){
            this->x = 0;
        }
        if(this->y + hori > limit){
            this->y = limit;
        }else if(this->y + verti < 0){
            this->y = 0;
        }
    }else{
        this->x += hori;
        this->y += verti;
    }
    
}

void Sheep::calV0(Player *player, int type = 0){
    // 所有無人機合力
    pair <float, float> t_force(0, 0);
    if(type == 0){
        // case 1
        for(int i = 0; i < this->player_index.size(); i++){
            // cout << this->player_index.size();
            t_force.first += this->x - player[this->player_index[i]].pos.first;
            t_force.second += this-> y - player[this->player_index[i]].pos.second;
        }
        t_force.first /= this->player_index.size();
        t_force.second /= this->player_index.size();
    }else{
        for(int i = 0; i < this->player_index_p.size(); i++){
            // cout << this->player_index.size();
            Point vec = projectP(Point(this->x, this->y), player[this->player_index_p[i]].path);
            t_force.first += this->x -  vec.real();
            t_force.second += this-> y - vec.imag();
            // t_force.first += this->x -  player[this->player_index[i]].pos.first;
            // t_force.second += this-> y - player[this->player_index[i]].pos.second;
        }
        t_force.first /= this->player_index_p.size();
        t_force.second /= this->player_index_p.size();

    }

    pair <float, float> dir(t_force.first, t_force.second); 
    if(dir.first == 0 && dir.second == 0){
        // 在正中間
        dir = pair <float, float>((rand() % (3) - 1)*1.5, (rand() % (3) - 1)*1.5);
        // cout << dir.first << " " << dir.second << endl;
    }
    this->currV = dir; 
}

void Sheep::runMode(int limit){
    // cout << this->currV.first << " " << this->currV.second << endl;
    if(this->currV.first != 0 || this->currV.second != 0){
        if(this->currV.first + this->x <= limit && this->currV.first + this->x >= 0){
            this->x += this->currV.first;
        }else{
            if(this->currV.first + this->x > limit){
                this->x = limit;
            }else{
                this->x = 0;
            }
        }
        if(this->currV.second + this->y <= limit && this->currV.second + this->y >= 0){
            this->y += this->currV.second;
        }else{
            if(this->currV.second + this->y > limit){
                this->y = limit;
            }else{
                this->y = 0;
            }
        }
        int curr_a = 1;
        // 減速
        if(this->currV.first != 0){
            curr_a = this->currV.first/abs(this->currV.first)*a;
            if((this->currV.first + curr_a)/this->currV.first <= 0){
                this->currV.first = 0;
            }else{
                this->currV.first += curr_a;
            }
        }
        if(this->currV.second != 0){
            curr_a = this->currV.second/abs(this->currV.second)*a;
            if((this->currV.second + curr_a)/this->currV.second <= 0){
                this->currV.second = 0;
            }else{
                this->currV.second += curr_a;
            }
        }
        
    }
    if(this->currV.first == 0 && this->currV.second == 0){
        this->status = 0;
    }
}

void checkSheepStatus(Sheep *sheeps, int n, Player *player, int limit, int m){
    float dis;
    for(int i = 0; i < n; i++){
        /*
            status = 0: 隨機走動mode(dubug 中暫時讓羊不動)
                   = 1: 驚嚇Mode
                   = 2: 抵達終點
                   = 3: 在路徑中受到驚嚇
        */
       if(sheeps[i].getPos().first == limit && sheeps[i].getPos().second == limit){
            // cout << "set sheep " << i << "'s status to 2\n";  
            sheeps[i].setStatus(2);
       }else{
            dis = INT_MAX;
            // find 
            sheeps[i].playerReset();

            for(int j = 0; j < m; j++){
            // 先判斷無人機在經過的路徑上有沒有影響
                Point p(sheeps[i].getPos().first, sheeps[i].getPos().second);
                if(distancePtoL(p, player[j].path) != -1 && distancePtoL(p, player[j].path) < 1.5){
                    // cout << "sheep " << i << " and player " << j << "'s distance : " <<  distancePtoL(p, player[j].path) << endl;
                    sheeps[i].setStatus(3);
                    sheeps[i].addPlayerP(j);
                }else{
                    dis = distanceObj(sheeps[i].getPos(), player[j].pos);

                    // cout << i << " "  << j << ": " << dis << endl;
                    if(dis <= 1.5){
                        // cout << "because player " << j << ", set sheep " << i << "'s status to 1\n";
                        if(sheeps[i].getSta() != 3){
                            // 優先處理 case3(先發生)
                            sheeps[i].setStatus(1);
                            // sheeps[i].calV0(player);  <- 要等到scan完所有無人機才能算(搬到sheepMover)
                            sheeps[i].addPlayer(j);
                        }
                    }else{
                        if (!(sheeps[i].getSta() == 1 || sheeps[i].getSta() == 3)){
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
}

pair<float, float> calculateGCM(Sheep *sheeps, int n){
    pair<float, float> gcm(0, 0);
    int count = 0;
    for(int i = 0; i < n; i++){
        if(sheeps[i].getSta() != 2){
            gcm.first +=  sheeps[i].getPos().first;
            gcm.second +=  sheeps[i].getPos().second;
            count += 1;
        }
    }
    // cout << "count: " << count << endl;
    gcm.first /= count;
    gcm.second /= count;
    return gcm;
}

pair<float, float> calculateLCM(Sheep *sheeps, int index, int n){
    pair<float, float> lcm(0, 0);
    int count = 0;
    for(int i = 0; i < n; i++){
        if(sheeps[i].getSta() != 2 && i != index){
            lcm.first +=  sheeps[i].getPos().first;
            lcm.second +=  sheeps[i].getPos().second;
            count += 1;
        }
    }
    if(count == 0){
        // 只剩一隻
        lcm = sheeps[index].getPos();
    }else{
        lcm.first /= count;
        lcm.second /= count;
    }
    return lcm;   
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


bool checkStatus(Sheep *sheeps, int n){
    for(int i = 0; i < n; i++){
        if(sheeps[i].getSta() != 2){
            return false;
        }
    }
    return true;
}

void sheepGather(Sheep *sheeps, int far_sheep, pair<float, float> lcm, int limit){
    float x, y;
    if(lcm.first - sheeps[far_sheep].getPos().first != 0){
        x = (lcm.first - sheeps[far_sheep].getPos().first)/abs(lcm.first - sheeps[far_sheep].getPos().first);
    }else{
        x = 0;
    }
    if(lcm.second - sheeps[far_sheep].getPos().second != 0){
        y = (lcm.second - sheeps[far_sheep].getPos().second)/abs(lcm.second - sheeps[far_sheep].getPos().second);
    }else{
        y = 0;
    }
    if(x != 0 && y != 0){
        pair <float, float> dir(0.1*x, 0.1*y);
        sheeps[far_sheep].randMove(limit, dir);
    }

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
            player[j].path.p1 = Point(player[j].pos.first, player[j].pos.second);
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
        player[i].pos.first += player[i].currV.first;
        player[i].pos.second += player[i].currV.second;
        player[i].path.p2 = Point(player[i].pos.first, player[i].pos.second);
    }
}

void sheepMover(Sheep* sheeps, Player *player, int n, int limit){
    for(int i = 0; i < n; i++){
        // cout << "sheep " << i << ", status: " << sheeps[i].getSta() << endl;
        switch(sheeps[i].getSta()){
            case 0: 
                // sheeps[i].randMove(limit);
                break;
            case 1: 
                if(!(abs(sheeps[i].getV().first) > 0 || abs(sheeps[i].getV().second) > 0)){
                    sheeps[i].calV0(player);
                }
                sheeps[i].runMode(limit);
                break;
            case 2: 
                break;
            case 3: 
                sheeps[i].calV0(player, 1);
                sheeps[i].runMode(limit);
                break;

        }
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
    generateRandSheep(sheeps, n, limit, list, randTyp);
    int time = 0;
    int far_sheep = -1;
    bool gameover = false;
    pair <float, float> gcm(0, 0);
    pair <float, float> lcm(0, 0);
    // m:無人機個數
    int m = 2;
    Player *player;
    player = new Player[m];
    for(int i = 0; i < m; i++){
        player[i].pos = pair<float, float>(0, 0);
        player[i].path.p1 = Point(0,0);
        player[i].path.p2 = Point(0,0);
    }
    pair <float, float> target(limit, limit);

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
    // printStatus(sheeps, n, time, player.pos);

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

        gcm = calculateGCM(sheeps, n);
        far_sheep = findFarest(sheeps, gcm, n);
        lcm = calculateLCM(sheeps, far_sheep, n);
        // player movement
        method2(sheeps, player, n, m, gcm, lcm, target, limit, c1, c2, c3);
        // cout << "player moving..." << endl;
        // method3(player, sheeps, target, n, limit, m);
        playerMover(player,limit, m);


        
        // cout << "checking sheep status..." << endl;
        // 確認羊群狀態(配合player path)
        checkSheepStatus(sheeps, n, player, limit, m);
        // 羊群聚集
        // sheepGather(sheeps, far_sheep, lcm, limit);
        // cout << "sheep moving..." << endl;
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
    //     // printStatus(sheeps, n, time - 1, player.pos);
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



