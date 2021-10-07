#include <iostream>
#include <list> 
#include <stdlib.h>
#include<utility>
#include <cmath>
#include <random> 
#include <climits>
#include <algorithm>
#include <complex>
using namespace std;
typedef complex<float> Point;
#define PI 3.1415926
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

Point rotate(Point org, Point center, float angle){
    Point r(cos(angle/180*M_PI), sin(angle/180*M_PI));
    Point rusult = (org - center)*r + center;
    return rusult;
}
// 線段(用於表示無人機路徑)
struct Line {Point p1, p2;};

struct Player {
    Player(){};
    Player(float x, float y){
        pos = pair<float, float>(x, y);
    }
    pair <float, float> pos;
    pair <float, float> currV;
    Line path;
};

// 點積運算
float dot(Point v1, Point v2)
{
    return v1.real() * v2.real() + v1.imag() * v2.imag();
}

float length(Point p)
{
    return sqrt(p.real() * p.real() + p.imag() * p.imag());
}

// 外積
float cross(Point v1, Point v2)
{
    return v1.real() * v2.imag() - v1.imag() * v2.real();
}

Point projectP(Point a, Line l){
    // l.p1位移至原點
    cout << a << endl;
    cout << l.p1 << " " << l.p2;
    Point pa = a - l.p1;
    Point pq = l.p2 - l.p1;
    cout << "to (0,0)\n";
    cout << pa << endl;
    cout << pq << endl;
    // a投影至直線(p,q)，得到b。
    Point pb = pq * dot(pa, pq) / norm(pq);
    cout << pb;
    Point b = l.p1 + pb;
    return b;
}

// 點到線段的距離
float distancePtoL(Point p, Line s)
{
    // 建立向量求點積，以判斷點在哪一區。
    Point v (s.p2 - s.p1);
    Point v1 (p - s.p1);
    Point v2 (p - s.p2);
 
    // 端點距離區：點到點距離
    // 等於=可以省略
    if (dot(v, v1) <= 0) return length(v1);
    if (dot(v, v2) >= 0) return length(v2);
    // 垂直距離區：點到線距離
    return abs(cross(v, v1)) / length(v);
}

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
    void randMove(int limit, pair <float, float> target, pair<float, float> v0);
    void calV0(Player* player);
    void runMode(int limit, pair<float, float> target);
    void playerReset(){
        this->player_index.clear();
    }
    void addPlayer(int ind){
        this->player_index.push_back(ind);
    }
private:
    int status;
    float x;
    float y;
    pair<float, float> currV;
    // 被哪幾台無人機影響
    vector <int> player_index;
};

void generateRandSheep(Sheep *sheeps, int n, int r, int d, int limit, pair <float, float> *list, int randTyp){
    // 真實隨機
    random_device rd;
    default_random_engine generator( rd() );
    srand(time(NULL));
    float hori, verti;
    int cirR = r; //生成虛擬範圍
    // cout << cirR << endl;
    if(randTyp == 0){
        // 聚集
        float angle = rand()%360;
        pair<float, float> center(d*cos(angle*PI/180.0f), d*sin(angle*PI/180.0f)); //虛擬範圍中心
        // cout << "center: " << center.first << " " << center.second  << endl;
        uniform_real_distribution<float> unif(-cirR, cirR);
        for(int i = 0; i < n; i++){
            // srand(i);
            hori = std::min({unif(generator) + center.first, float(limit)});
            hori = std::max({unif(generator) + center.first, (float)(-1*limit)});
            verti = std::min({unif(generator) + center.second,float(limit)});
            verti = std::max({unif(generator) + center.second,(float)(-1*limit)});
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

void Sheep::randMove(int limit, pair<float,float> target, pair <float, float> v0 = pair <float, float>(0, 0)){
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
    // if(this->x + hori > limit || this->x + hori < 0 || this->y + verti > limit || this->y + verti < 0){
    //     if(this->x + hori > limit){
    //         this->x = limit;
    //     }else if(this->x + hori < 0){
    //         this->x = 0;
    //     }
    //     if(this->y + hori > limit){
    //         this->y = limit;
    //     }else if(this->y + verti < 0){
    //         this->y = 0;
    //     }
    // }else{
    //     this->x += hori;
    //     this->y += verti;
    // }
    if(distanceObj(pair<float,float>(this->x + hori, this->y + verti), target) > limit){
        // 超出地圖範圍
        // cout << "sheep exceed range\n"; 
        // 找到切線方向
        pair <float, float>org(this->x + hori - target.first, this->y + verti - target.second);
        // 算出切線與圓周交點
        float angle = tan(abs(org.first)/abs(org.second));
        float x = limit*cos(angle);
        float y = limit*sin(angle);
        if (org.first < 0) y *= -1;
        if (org.second < 0) x *= -1;
        // 更新羊隻位置
        this->x = x;
        this->y = y;
    }else{
        this->x += hori;
        this->y += verti;
    }
    
}

void Sheep::calV0(Player *player){
    // 所有無人機合力
    pair <float, float> t_force(0, 0);
    for(int i = 0; i < this->player_index.size(); i++){
        // cout << this->player_index.size();
        t_force.first += this->x - player[this->player_index[i]].pos.first;
        t_force.second += this-> y - player[this->player_index[i]].pos.second;
    }
    t_force.first /= this->player_index.size();
    t_force.second /= this->player_index.size();
    pair <float, float> dir(t_force.first, t_force.second); 
    if(dir.first == 0 && dir.second == 0){
        // 在正中間
        dir = pair <float, float>((rand() % (3) - 1)*1.5, (rand() % (3) - 1)*1.5);
        // cout << dir.first << " " << dir.second << endl;
    }
    this->currV = dir; 
}

void Sheep::runMode(int limit, pair<float, float> target){
    // cout << this->currV.first << " " << this->currV.second << endl;
    if(this->currV.first != 0 || this->currV.second != 0){
        // if(this->currV.first + this->x <= limit && this->currV.first + this->x >= 0){
        //     this->x += this->currV.first;
        // }else{
        //     if(this->currV.first + this->x > limit){
        //         this->x = limit;
        //     }else{
        //         this->x = 0;
        //     }
        // }
        // if(this->currV.second + this->y <= limit && this->currV.second + this->y >= 0){
        //     this->y += this->currV.second;
        // }else{
        //     if(this->currV.second + this->y > limit){
        //         this->y = limit;
        //     }else{
        //         this->y = 0;
        //     }
        // }
        // 確認是否超出範圍
        if(distanceObj(pair<float,float>(this->x + this->currV.first, this->y + this->currV.second), target) > limit){
            // 超出地圖範圍
            // cout << "sheep exceed range\n"; 
            // 找到切線方向
            pair <float, float>org(this->x + this->currV.first - target.first, this->y + this->currV.second - target.second);
            // 算出切線與圓周交點
            float angle = tan(abs(org.first)/abs(org.second));
            float x = limit*cos(angle);
            float y = limit*sin(angle);
            if (org.first < 0) y *= -1;
            if (org.second < 0) x *= -1;
            // 更新羊隻位置
            this->x = x;
            this->y = y;
        }else{
            this->x += this->currV.first;
            this->y += this->currV.second;
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

void checkSheepStatus(Sheep *sheeps, int n, Player *player, int limit, int m, pair<float, float> target){
    float dis;
    for(int i = 0; i < n; i++){
        /*
            status = 0: 隨機走動mode
                   = 1: 驚嚇Mode
                   = 2: 抵達終點
        */
    //    if(sheeps[i].getPos().first == target.first && sheeps[i].getPos().second == target.second){
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
        sheeps[far_sheep].randMove(limit, pair<float, float>(0,0), dir);
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

void sheepMover(Sheep* sheeps, Player *player, int n, int limit, pair<float, float> target){
    for(int i = 0; i < n; i++){
        switch(sheeps[i].getSta()){
            case 0: 
                sheeps[i].randMove(limit, target);
                break;
            case 1: 
                if(!(abs(sheeps[i].getV().first) > 0 || abs(sheeps[i].getV().second) > 0)){
                    sheeps[i].calV0(player);
                }
                sheeps[i].runMode(limit, target);
                break;
            case 2: 
                break;
        }
    }
}


// 改圓形地圖(以target為圓心,半徑為limit的圓)
int main(int argc, char *argv[]){
    // init
    int max_n = atoi(argv[1]);
    int max_r = atoi(argv[2]);
    int max_d = atoi(argv[3]);
    int max_limit = atoi(argv[4]);
    for(int limit = max_limit; limit <= max_limit; limit+=20){
        for( int r = max_r; r <= max_r; r++){
            if(limit > r){
                for(int n = max_n; n <= max_n; n+=5){
                    for (int d = max_d; d <= max_d; d++){

                        Sheep *sheeps = new Sheep[n];
                        pair <float, float> list[n];
                        // 分成聚集(0)、分散兩種(1)
                        int randTyp = 0;
                        generateRandSheep(sheeps, n, r, d, limit, list, randTyp);
                        int time = 0;
                        int far_sheep = -1;
                        bool gameover = false;
                        // pair <float, float> gcm(0, 0);
                        // pair <float, float> lcm(0, 0);
                        // m:無人機個數
                        int m = 1;
                        Player *player;
                        player = new Player[m];
                        for(int i = 0; i < m; i++){
                            player[i].pos = pair<float, float>(0, 0);
                        }
                        pair <float, float> target(0, 0);

                        int min = 999999;
                        bool overloaded;
                        // printStatus(sheeps, n, time, player, m);

                        // Game
                        while(!gameover){

                            method3(player, sheeps, target, n, limit, m);
                            playerMover(player,limit, m);


                            
                            // 確認羊群狀態(配合player path)
                            checkSheepStatus(sheeps, n, player, limit, m, target);
                            printStatus(sheeps, n, time, player, m);
                            // 羊群聚集
                            // sheepGather(sheeps, far_sheep, lcm, limit);

                            sheepMover(sheeps, player, n, limit, target);


                            // check Game status (是否所有羊隻都抵達終點)
                            gameover = checkStatus(sheeps, n);
                            // cout << gameover << endl;
                            time += 1;
                            // 防止無限迴圈
                            if(time > 50000){
                                overloaded = true;
                                gameover = true;
                            }
                        }
                        cout << n << "\t" << r << "\t" << d << "\t" << time - 1  << "\t" << endl;
                    }
                }
            }
        }
    }

}



