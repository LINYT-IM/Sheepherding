#include<random>
#include "sheep.h"
using namespace std;
int a = -1;


pair <float, float> Sheep::getPos(){
    return pair <float, float> (x, y);
}

int Sheep::getSta(){
    return this->status;
}

pair<float, float> Sheep::getV(){
    return currV;
}

void Sheep::setStatus(int stat){
    this->status = stat;
}

void Sheep::playerReset(){
    this->player_index.clear();
}

void Sheep::addPlayer(int ind){
    this->player_index.push_back(ind);
}

void Sheep::setData(int x, int y, int status, int index){
    this->x = x; 
    this->y = y;
    this->status = status;
    this->index = index;
}

int Sheep::getIndex(){
    return this->index;
}

void Sheep::randMove(int limit, pair <float, float> v0){
    random_device rd;
    default_random_engine generator( rd() );
    uniform_real_distribution<float> unif(-0.5, 0.5);
    float hori;
    float verti;
    if(v0.first == 0 && v0.second == 0){
        hori = unif(generator); //-0.5, 0, 0.5
        verti = unif(generator); //-0.5, 0, 0.5
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

void sheepMover(Sheep* sheeps, Player *player, int n, int limit){
    for(int i = 0; i < n; i++){
        switch(sheeps[i].getSta()){
            case 0: 
                sheeps[i].randMove(limit);
                break;
            case 1: 
                if(!(abs(sheeps[i].getV().first) > 0 || abs(sheeps[i].getV().second) > 0)){
                    sheeps[i].calV0(player);
                }
                sheeps[i].runMode(limit);
                break;
            case 2: 
                break;
        }
    }
}

bool checkStatus(Sheep *sheeps, int n){
    for(int i = 0; i < n; i++){
        if(sheeps[i].getSta() != 2){
            return false;
        }
    }
    return true;
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