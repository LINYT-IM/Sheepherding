//矩形場域
//給十隻羊座標、分群座標(k-means)
//可跑多次算平均時間
//undone修正
//讀檔
//無人機起始座標修正
#include <iostream>
#include <cmath>
#include <vector>
#include <algorithm>
#include <random> 
#include <fstream>
#include <sstream>
using namespace std;
int a = -1;
#define PI 3.1415926
void createFile(string filename){
    fstream f;
    f.open(filename, ios::out | ios::trunc);
    f.close();
}

void readfile(bool *read, string filename, int *N, int*K, int *M,  pair <float, float> *&posi, pair <float, float> *&group_m, pair <float, float> *&meet, vector<int> *&paths, vector<int> &group, float* max_x, float* max_y, float* tar_x, float* tar_y){
    fstream input;
    input.open(filename);
    // No file
    if(!input){
        cout << "Cannot Find The File.";
        *read = false;
        return;
    }
    char c;
    string problemType, line;
    int index ;
    float x,y,dist;
    bool flag = true;
    string token, delimiter = "\t";
    size_t pos;
    *max_x = 0;
    *max_y = 0;
    while(flag){
        input >> c;
        // cout<<c;
        switch(c){
            case 'n':
                input >> *N;
                // cout << *N << endl;
				posi = new pair<float, float>[*N];
                getline(input, line);
                break;
            case 'k':
                input >> *K;
                // cout<< *K << endl;
                paths = new vector<int>[*K];
                group_m = new pair<float, float>[*K];
                getline(input, line);
                break;
            case 'm':
                input >> *M;
                // cout << *M << endl;
				meet = new pair<float, float>[*M];
                getline(input, line);
                break;
            case 's':
                input >> *tar_x >> *tar_y;
                getline(input, line);
                break;
            case 'd':
            {
                input >> index >> x >> y ;
                posi[index] = pair<float, float>(x,y);
                if(x>*max_x){
                    *max_x = x;
                }
                if(y>*max_y){
                    *max_y = y;
                }
                getline(input, line);
                break;
            }
            case 'c':
                input >> index >> x >> y ;
                group_m[index] = pair<float, float>(x,y);
                getline(input, line);
                break;
            case 'h':
                input >> index >> x >> y ;
                meet[index-*K] = pair<float, float>(x,y);
                //cout<<index;
                getline(input, line);
                break;
            case 'g':
                getline(input,line,'\n');
                pos = 0;
                while ((pos = line.find(delimiter)) != string::npos) {
                    token =  line.substr(0, pos);
                    if(token != ""){
                        group.push_back(std::stoi(token));
                    } 
                    line.erase(0, pos + delimiter.length());
                }
                break;
            case 'p':
                // allocate the nodeList
                input >> index;
                paths[index].push_back(index);
                getline(input,line,'\n');
                pos = 0;
                while ((pos = line.find(delimiter)) != string::npos) {
                    token =  line.substr(0, pos);
                    if(token != ""){
                        paths[index].push_back(std::stoi(token));
                    } 
                    line.erase(0, pos + delimiter.length());
                }
                paths[index].push_back(std::stoi(line));
                break;
            
            case 'e':
                // cout << "到ｅ了啦啦啦啦啦啦" << std::endl;
                flag = false;
                break;
        }
    }
    // cout << "read." << endl;
}

//計算距離
float distanceObj(pair<float, float> obj1, pair<float, float> obj2){
    return sqrt(pow(obj1.first - obj2.first, 2) + pow(obj1.second - obj2.second, 2));
}

//無人機
struct Player {
    Player(float x, float y){
        pos = pair<float, float>(x, y);
    }
    pair <float, float> pos;
    pair <float, float> currV;
};

//羊
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
        int getcurrPos(){
            return this->currPos;
        }
        int getIndex(){
            return this->index;
        }
        pair<float, float> getV(){
            return currV;
        }
        
        void setData(int x, int y, int status, int index);
        void setStatus(int stat){
            this->status = stat;
        };
        void setcurrPos(int pos){
            this->currPos = pos;
        }
        void randMove(int limit, int randv);
        void calV0(pair <float, float> player);
        void runMode(int limit);
    private:
        int index;
        int status;
        int currPos;
        float x;
        float y;
        float v;
        pair<float, float> currV;
};

void writeFile(string filename, Sheep * sheeps_posi,int n, Sheep* randsheeps, int n_o, pair<float, float> player){
    fstream f;
    f.open(filename, ios::app);

    for (int it = 0; it < n; it++){
        f << sheeps_posi[it].getIndex() << " " << sheeps_posi[it].getPos().first << " " << sheeps_posi[it].getPos().second << "\t";
    }
    for (int it = 0; it < n_o; it++){
        f << randsheeps[it].getIndex() << " " << randsheeps[it].getPos().first << " " << randsheeps[it].getPos().second << "\t";
    }

    f << player.first << " " << player.second << "\n";
    f.close();
}

void writeFileUndone(string filename, int undone, int time){
    fstream f;
    f.open(filename, ios::app);
    f << time << "\t";
    // for (auto it = undone.begin(); it != undone.end(); ++it){
    //     f << *it << "\t";
    // }
    f << undone << "\t";
    f << "\n";

    f.close();
}

void Sheep::setData(int x, int y, int status, int index){
    this->x = x; 
    this->y = y;
    this->status = status;
    this->currPos = 0;
    this->index = index; 
}

//羊隨機走動 status = 0
void Sheep::randMove(int limit, int randv = 1){
    // int randv = 1 ;
    float hori ;
    float verti ;
    
    default_random_engine generator(rand());
    uniform_real_distribution<float> unif(-0.5, 0.5);
    hori = unif(generator); 
    verti = unif(generator); 
    //cout<<hori<<"\t"<<verti;

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

//羊被追趕 status = 1
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
    /*
    if(this->currV.first != 0){
        this->currV.first = 0 ;
    }

    if(this->currV.second != 0){
        this->currV.second = 0 ;
    }
    */

    if(this->currV.first == 0 && this->currV.second == 0){
        this->status = 0;
    }
}

//設定羊的速度
void Sheep::calV0(pair <float, float> player){
    pair <float, float> dir(this->x - player.first, this->y - player.second); 
    if(dir.first == 0 && dir.second == 0){
        // 在正中間
        dir = pair <float, float>((rand() % (3) - 1)*1.5, (rand() % (3) - 1)*1.5);
    }
    this->currV = dir; 
}

void printStatus(Sheep *sheeps,int n, int time, Player *player, int m, int group_i){
    //cout << "\tTime: " << time << endl;
    cout<<endl<<"group "<<group_i<<endl;
    for(int i = 0; i < n; i++){
        cout << "Sheep " << i << " : (" <<  sheeps[i].getPos().first << ", " << sheeps[i].getPos().second << "), status = " << sheeps[i].getSta() << endl;  
    }
    // for(int i = 0; i < m; i++){
    //     cout << "Player "<< i << " : (" << player[i].pos.first << ", " << player[i].pos.second << ")" << endl;
    // }
}

//產生隨機位置的羊
void generateRandSheep(Sheep *sheeps, int n, int limit, pair <float, float> *list){
    for(int i = 0; i < n; i++){
        int hori = rand()%limit;
        int verti = rand()%limit;
        sheeps[i].setData(hori, verti, 0, -1);
        list[i] = pair<float, float>(hori, verti);
    }
}

//設定羊的狀態
void checkSheepStatus(Sheep *sheeps, int n, pair <float, float> player, pair <float, float> target, pair <float, float> goal){
    float dis;
    float range;
    if(target.first == goal.first && target.second == goal.second){
        range = 2;
    }else{
        range = 5;
    }
    for(int i = 0; i < n; i++){
        /*
            status = 0: 隨機走動mode
                   = 1: 驚嚇Mode
                   = 2: 抵達終點
        */
       dis = distanceObj(sheeps[i].getPos(), target);
       if(dis < range){ 
            sheeps[i].setStatus(2);
       }else{
            dis = distanceObj(sheeps[i].getPos(), player);
            if(dis < 2){
                sheeps[i].setStatus(1);
                sheeps[i].calV0(player);
            }else{
                if (!(sheeps[i].getSta() == 1 && (sheeps[i].getV().first > 0 || sheeps[i].getV().second > 0))){
                    sheeps[i].setStatus(0);
                }else{
                    // 原本還在逃跑:繼續直到v = 0
                }
            }
       }
       
    }
}

//羊移動
void sheepMover(Sheep* sheeps, int n, int limit, pair <float, float> goal){
    for(int i = 0; i < n; i++){
        switch(sheeps[i].getSta()){
            case 0: 
                sheeps[i].randMove(limit);
                break;
            case 1: 
                sheeps[i].runMode(limit);
                break;
            case 2: 
                float dis = distanceObj(sheeps[i].getPos(), goal);
                if (dis > 2){
                    sheeps[i].randMove(limit, 0.5);
                }
                break;
        }
    }
}

//找最遠的羊
int findFarest(Sheep *sheeps, pair<float, float> target, int n){
    float maxD = 0;
    int max = -1;
    for(int i = 0; i < n; i++){
        if(distanceObj(sheeps[i].getPos(), target) >= maxD && sheeps[i].getSta() != 2){
            max = i;
            maxD = distanceObj(sheeps[i].getPos(), target);
        }
    }
    return max;
}

//找最近的羊
int findNearest(float **s_point, float **g_point, float **meet ,vector<int> undone, pair<float, float> target, int n, int total_p){
    float minD = 10000000;
    int min = -1000;
    // cout << target.first << " " << target.second << endl;
    if(undone.size()>0){
        for(int i = 0; i < undone.size(); i++){
            if(undone[i]<0){
                pair<float,float>temp(s_point[undone[i]*(-1)-1][0],s_point[undone[i]*(-1)-1][1]);
                // cout << temp.first << " " << temp.second << endl;
                // cout << distanceObj(temp, target) << endl;
                if(distanceObj(temp, target) <= minD){
                    min = undone[i];
                    minD = distanceObj(temp, target);
                }
            }
            else if(undone[i]<n){
                pair<float,float>temp(g_point[undone[i]][0],g_point[undone[i]][1]);
                if(distanceObj(temp, target) <= minD){
                    min = undone[i];
                    minD = distanceObj(temp, target);
                }
            }else{
                pair<float, float> meetpoint(meet[undone[i]-n][0], meet[undone[i]-n][1]);
                if(distanceObj(meetpoint, target) <= minD ){
                    min = undone[i];
                    minD = distanceObj(meetpoint, target);
                }
            }
        }
    }
    return min;
}

//檢查羊的狀態
bool checkStatus(Sheep *sheeps, int n){
    for(int i = 0; i < n; i++){
        if(sheeps[i].getSta() != 2){
            return false;
        }
    }
    return true;
}

//移動無人機
void playerMover(Player *player, int limit){
    
        player->pos.first += player->currV.first;
    
        // move
        player->pos.second += player->currV.second;

}

//以離目標最遠的羊開始趕
void method3(Player *player, Sheep *sheeps, pair <float, float> target, int n, int limit){
    int index = findFarest(sheeps, target, n);
    float dir_x;
    float dir_y;

    if(sheeps[index].getPos().first - target.first == 0.0){
        dir_x = 0;
    }else{
        dir_x = (sheeps[index].getPos().first - target.first)/abs((sheeps[index].getPos().first - target.first));
    }
    if(sheeps[index].getPos().second - target.second == 0.0){
        dir_y = 0;
    }else{
        dir_y = (sheeps[index].getPos().second - target.second)/abs((sheeps[index].getPos().second - target.second));
    }
    //cout <<"dir_ : "<< dir_x << " " << dir_y << endl;
    pair <float, float> tarPos(sheeps[index].getPos().first + dir_x, sheeps[index].getPos().second + dir_y);
    pair <float, float> dir(tarPos.first - player->pos.first, tarPos.second - player->pos.second);
    //cout << "tar:"<< tarPos.first << " " << tarPos.second << endl;
    //cout << "dir :"<< dir.first << " " << dir.second << endl;
    
    // if(dir.first != 0 && dir.second != 0){
    //     if(abs(dir.first) > 5){
    //         dir.first /= abs(dir.first)/5;
    //     }
    //     if(abs(dir.second) > 5){
    //         dir.second /= abs(dir.second)/5;
    //     }
    // }else{
    //     if(dir.first == 0){
    //         if(abs(dir.second) > 5){
    //             dir.second /= abs(dir.second)/5;
    //         }
    //     }else{
    //         if(abs(dir.first) > 5){
    //             dir.first /= abs(dir.first)/5;
    //         }
    //     }
    // }

    if(dir.first > 5){
        dir.first = 5;
    }else if(dir.first < -5){
        dir.first = -5;
    }
    if(dir.second > 5){
        dir.second = 5;
    }else if(dir.second < -5){
        dir.second = -5;
    }
    
    //cout << "dir :"<< dir.first << " " << dir.second << endl;
    player->currV = pair <float, float>(dir.first, dir.second);
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
        sheeps[far_sheep].randMove(limit);
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

void setPlayerV(Player *player, pair <float, float> tarPos){
    pair <float, float> dir(tarPos.first - player->pos.first, tarPos.second - player->pos.second);

    if(dir.first != 0 && dir.second != 0){
        if(abs(dir.first) > 5){
            dir.first /= abs(dir.first)/5;
        }
        if(abs(dir.second) > 5){
            dir.second /= abs(dir.second)/5;
        }
    }else{
        if(dir.first == 0){
            if(abs(dir.second) > 5){
                dir.second /= abs(dir.second)/5;
            }
        }else{
            if(abs(dir.first) > 5){
                dir.first /= abs(dir.first)/5;
            }
        }
    }
    
    player->currV = pair <float, float>(dir.first, dir.second);
}

int black_box(Player *player, Sheep *sheeps, pair <float, float> target, int n, int limit , Sheep *randsheeps , int n_o, pair <float, float> goal, string filename){
    bool done = false ; 
    int time = 0 ;
    pair <float, float> gcm(0, 0);
    pair <float, float> lcm(0, 0);
    // int far_sheep = -1;
    // Sheep *allsheeps = new Sheep[n+n_o];
    // cout<<endl<<"black box tar:"<<target.first<<","<<target.second;
    // int k = 0 ;

    // for(int i=0;i<n;i++){
    //     allsheeps[k] = sheeps[i];
    //     k++;
    // }
    // for(int j=0;j<n_o;j++){
    //     allsheeps[k] = randsheeps[j];
    //     k++;
    // }
    checkSheepStatus(sheeps, n, player->pos, target, goal);
    done = checkStatus(sheeps, n);
    writeFile(filename, sheeps, n, randsheeps, n_o, player->pos);
    while(!done){

        // gcm = calculateGCM(allsheeps, n);
        // far_sheep = findFarest(allsheeps, gcm, n);
        // lcm = calculateLCM(allsheeps, far_sheep, n);
        
        checkSheepStatus(sheeps, n, player->pos, target, goal);
        
        // cout<<endl<<"black box time: "<<time<<endl;
        // cout<< player->pos.first << "\t" << player->pos.second << endl ;
        // for( int i = 0 ; i < n ; i++){
        //     pair <float, float> test = sheeps[i].getPos() ;
        //     cout<<sheeps[i].getSta()<<"\t" <<test.first << "\t" << test.second <<endl ;
        // }
        sheepMover(sheeps, n, limit, goal);
        sheepMover(randsheeps, n_o, limit, goal);
        
        method3(player, sheeps, target, n, limit);
        //sheepGather(sheeps, far_sheep, lcm, limit);
        //sheepGather(randsheeps, far_sheep, lcm, limit);
        playerMover(player,limit);
        checkSheepStatus(sheeps, n, player->pos, target, goal);

        writeFile(filename, sheeps, n, randsheeps, n_o, player->pos);
        
        done = checkStatus(sheeps, n);
        time += 1;
        //cout<<endl<<time;
    }

    checkSheepStatus(sheeps, n, player->pos, goal, goal);
    return time ; 
}

void generateSheepGroup(Sheep *sheeps, int n, int limit,  float x,float y,float r){
    //random_device rd;
    default_random_engine generator( 2000 );
    //srand(time(NULL));
    float hori, verti;
    int cirR = r; //生成虛擬範圍
        // 聚集
        float angle = rand()%360;
        pair<float, float> center(x,y); //虛擬範圍中心
        // cout << "center: " << center.first << " " << center.second  << endl;
        uniform_real_distribution<float> unif(-cirR, cirR);
        for(int i = 0; i < n; i++){
            // srand(i);
            hori = std::min(unif(generator) + center.first, float(limit));
            hori = std::max(unif(generator) + center.first, (float)(-1*limit));
            verti = std::min(unif(generator) + center.second,float(limit));
            verti = std::max(unif(generator) + center.second,(float)(-1*limit));
            if(hori>limit){
                hori = limit;
            }
            if(hori<0){
                hori = 0;
            }
            if(verti>limit){
                verti = limit;
            }
            if(verti<0){
                verti = 0;
            }
            // hori = rand()%(2*cirR) + center.first - cirR;
            // verti = rand()%(2*cirR) + center.second - cirR;
            sheeps[i].setData(hori, verti, 0, -1);
        }
}




int main(){
    
    //srand(time(NULL));
    int max_limit = 1000 ;
    int total_p;
    int max_n ; //幾群羊
    int n_of_s ; //幾隻羊
    int m ; //中繼點數量
    //float r = 5; //羊群的半徑

    //float s_point [10][2] = {{32,23},{25,20},{28,12},{22,39},{1,30},{32,10},{43,46},{9,8},{31,39},{37,29}};
    //float point [5][2] = { {26.5,39},{29.25,16.25},{40,37.5},{1,30},{9,8} } ;
    //float meet [3][2] = { {25.18, 21.45},{8.99, 8},{7.53, 8.32} } ;
    //int path[5][6] =  {{0,5,6,7,-1},{1,5,6,7,-1},{2,5,6,7,-1},{3,7,-1},{4,6,7,-1} };
    // float point [5][2] = { {5,32},{11,50},{31,9},{5,2},{10,40} } ;
    // float meet [4][2] = { {14.16, 18.68},{10.02, 20.98},{6.44, 22.66},{3.61, 3.6} } ;
    // int path[5][6] =  {{0,7,8,-1},{1,6,7,8,-1},{2,5,6,7,8,-1},{3,8,-1},{4,5,6,7,8,-1} };
    //int group_n[5] = {2,4,2,1,1}; 
    //int group[10] = {1,1,1,0,3,1,2,4,0,2};
    
    int iteration_time = 100;
    for(int input_n = 20; input_n<=20 ; input_n+=5){
        for(int input_index =1 ; input_index<100; input_index++){
            string filename_move  = "movement/S3_case_" + ::to_string(input_n)+"_"+std::to_string(input_index)+"_1.txt";
            string filename_midP  = "midP/S3_case_" + ::to_string(input_n)+"_"+std::to_string(input_index)+"_1.txt";
            string filename = "input/k_results_100/k_input_point_"+std::to_string(input_n)+"_"+std::to_string(input_index)+".txt";
            // cout << filename << endl;
            createFile(filename_move);
            createFile(filename_midP);
            float avg_time = 0;

    
            bool read = true;
            pair <float, float> *nodeList;
            pair <float, float> *group_m;
            pair <float, float> *meet_pos;
            vector<int> *paths;
            vector<int> group_list;
            float max_x, max_y, tar_x, tar_y; 
            
            
            int randseed = 101;
            for(int iteration = 0 ; iteration < iteration_time ; iteration++){
                int time = 0 ;
                srand(randseed);
                readfile(&read, filename, &n_of_s, &max_n, &m, nodeList, group_m, meet_pos, paths, group_list, &max_x, &max_y, &tar_x, &tar_y);
                // cout << tar_x << " " << tar_y << " " <<endl;
            
                total_p = m + max_n ;//中繼點＋羊群數量
                // cout << n_of_s << " " << max_n <<endl;
                for(int limit = max_limit; limit <= max_limit; limit++){
                    for(int n = max_n; n <= max_n; n++){
                        
                        Player player(max_x, max_y);
                        pair <float, float> target(tar_x, tar_y);
                        
                        //羊座標
                        float **s_point = new float*[n_of_s];
                        for(int i=0;i<n_of_s;i++){
                            s_point[i]=new float[2];
                            s_point[i][0] = nodeList[i].first;
                            s_point[i][1] = nodeList[i].second;
                            // cout << s_point[i][0] << " " << s_point[i][1] << endl;
                        }

                        //群中心座標
                        float **g_point = new float*[n];
                        for(int i=0;i<n;i++){
                            g_point[i]=new float[2];
                            g_point[i][0] = group_m[i].first;
                            g_point[i][1] = group_m[i].second;
                        }

                        //中繼點座標
                        float **meet_point = new float*[m];
                        for(int i=0;i<m;i++){
                            meet_point[i]=new float[2];
                            meet_point[i][0] = meet_pos[i].first;
                            meet_point[i][1] = meet_pos[i].second;
                        }
                        // for(int i=0;i<m;i++){
                        //     cout<<meet_point[i][1]<<" "<<meet_point[i][1]<<endl;
                        // }
                        
                        //路徑
                        int **path = new int*[n];
                        for(int i=0;i<n;i++){
                            path[i] = new int[paths[i].size()];
                            for(int j=0;j<paths[i].size();j++){
                                path[i][j] = paths[i][j];
                            }
                        }
                        

                        //分群
                        int *group_index = new int[n_of_s];     //羊在哪一群
                        int *group_n = new int[n]();            //一群有幾隻
                        for(int i=0;i<n_of_s;i++){
                            group_index[i] = group_list[i];
                            group_n[group_index[i]]++;
                        }
                        
                        //記錄群中心的分支
                        vector <vector <int> > leaf;
                        for(int r=0;r<n;r++){
                            vector <int> branch;
                            for(int s =0; s<n_of_s;s++){
                                if(group_index[s]==r){
                                    branch.push_back((s+1)*(-1));
                                }
                            }
                            leaf.push_back(branch);
                            branch.clear();
                        }
                        
                        //紀錄中繼點的分支
                        for(int k=0;k<m;k++){   
                            vector <int> branch;
                            for(int i=0;i<max_n;i++){
                                for(int j=0;j<paths[i].size();j++){
                                    if(path[i][j]== k+max_n ){
                                        branch.push_back(i);
                                        for(int l=0;l<n_of_s;l++){
                                            if(group_index[l]==i){
                                                branch.push_back((l+1)*(-1));
                                            }
                                        }
                                        if(j!=1){
                                            for(int l=j-1;l>0;l--){
                                                if(find(branch.begin(), branch.end(), path[i][l]) == branch.end()){
                                                    branch.push_back(path[i][l]);
                                                }                                
                                            }
                                        }
                                    }
                                }
                            }
                            leaf.push_back(branch);
                            branch.clear();

                        }
                        // for(int k=0;k<m;k++){
                        //     for(int i=0;i<leaf[k].size();i++){
                        //         cout<<leaf[k][i]<<" ";
                        //     }
                        //     cout<<endl;
                        // }
                        
                        
                        Sheep **total_sheeps = new Sheep*[n];
                        int curr; //記錄無人機的目標點
                        int time_pass = 0;
                        bool gameover = false;

                        for(int i=0;i<n;i++){
                            Sheep *sheeps = new Sheep[group_n[i]];
                            int k = 0 ;
                            for(int j=0;j<n_of_s;j++){
                                if(group_index[j]==i){
                                    sheeps[k].setData(s_point[j][0],s_point[j][1], 0, j);
                                    //cout<<s_point[j][0]<<" "<<s_point[j][1]<<endl;
                                    // cout<<sheeps[k].getIndex()<<" ";
                                    k++;
                                }
                            }
                            total_sheeps[i] = sheeps;   
                        }
                        
                        vector <int> undone;
                        vector <int> done;

                        for(int i = 0 ; i<n_of_s ; i++){
                            undone.push_back((i+1)*(-1));
                        }
                            
                        //無人機初始目標（找最近的點）
                        curr = findNearest(s_point, g_point, meet_point, undone, player.pos, n, total_p);
                        
                        
                        while(!undone.empty() && !gameover){
                            // cout<<"time:"<<time <<" curr:"<<curr;
                            // cout<<" ("<<player.pos.first<<","<<player.pos.second<<") ";
                            // cout<<" undone : ";
                            // for (size_t i = 0; i < undone.size(); ++i) {
                            //    cout <<undone[i]<<" ";
                            // }
                            // cout<<" done : ";
                            // for (size_t i = 0; i < done.size(); ++i) {
                            //    cout <<done[i]<<" ";
                            // }
                                
                            if(curr == -1000){
                                //findnearest error
                                cout<< " error! ";
                                gameover = true;
                            }else if(curr<0){
                                //把羊趕到群中心
                                int move_s = curr*(-1)-1;
                                int r_index = 0;
                                Sheep *movesheep = new Sheep[1];
                                Sheep *randsheep = new Sheep[n_of_s-1];
                                
                                for(int i=0;i<n;i++){
                                    for(int j=0;j<group_n[i];j++){
                                        // cout<<total_sheeps[i][j].getIndex()<<" ";
                                        if(total_sheeps[i][j].getIndex() == move_s){
                                            movesheep[0] = total_sheeps[i][j];
                                        }else{
                                            randsheep[r_index] = total_sheeps[i][j];
                                            r_index++;
                                        }
                                    }
                                }
                                
                                pair <float,float> goal(g_point[group_index[move_s]][0], g_point[group_index[move_s]][1]); 
                                // cout<<" to:"<<group_index[move_s];
                                time_pass += black_box(&player,movesheep,goal,1,limit,randsheep, n_of_s-1, target, filename_move);

                                r_index = 0;
                                for(int i=0;i<n;i++){
                                    for(int j=0;j<group_n[i];j++){
                                        if(total_sheeps[i][j].getIndex() == move_s){
                                            total_sheeps[i][j] = movesheep[0] ;
                                        }else{
                                            total_sheeps[i][j] = randsheep[r_index];
                                            r_index++;
                                        }
                                    }
                                }

                                undone.erase(std::remove(undone.begin(), undone.end(), curr), undone.end());
                                done.push_back(curr);  
                                writeFileUndone(filename_midP, curr, time);
                                delete [] randsheep;
                                delete [] movesheep;
                                curr = findNearest(s_point,g_point, meet_point, undone, player.pos, n, total_p);
                                // cout<<" "<<curr;

                            }else if(curr<n){ 
                                //把羊群趕到中繼點
                                // cout<< " state: "<<"1 ";

                                pair <float,float> goal;
                                if(path[curr][1] == -1){
                                    goal = target;
                                }else{
                                    if(path[curr][1]>=n){
                                        goal.first = meet_point[path[curr][1]-n][0];
                                        goal.second = meet_point[path[curr][1]-n][1];
                                    }else{
                                        goal.first = g_point[path[curr][1]][0];
                                        goal.second = g_point[path[curr][1]][1];
                                    }
                                }

                                
                                    
                                int rand_sheep_size = n_of_s-group_n[curr];
                                Sheep *randsheep = new Sheep[rand_sheep_size];
                                int k = 0 ;
                                for(int i=0;i<n;i++){
                                    if(i!=curr){
                                        for(int j=0;j<group_n[i];j++){
                                            randsheep[k] = total_sheeps[i][j];
                                            k++ ;
                                        }
                                    }
                                }

                                // cout<<" to:"<<path[curr][1];
                                // cout<<"\t"<<goal.first<<"\t"<<goal.second<<"\t";
                                // cout<<"\t"<<time_pass<<"\t";
                                time_pass += black_box(&player,total_sheeps[curr],goal,group_n[curr],limit,randsheep, k, target, filename_move);
                                // cout<<"\t"<<time_pass<<"\t";
                                k = 0 ;
                                for(int i=0;i<n;i++){
                                    if(i!=curr){
                                        for(int j=0;j<group_n[i];j++){
                                            total_sheeps[i][j] = randsheep[k];
                                            k++ ;
                                        }
                                    }else{
                                        for(int j=0;j<group_n[i];j++){
                                            total_sheeps[i][j].setcurrPos(1);
                                        }
                                    }
                                }

                                undone.erase(std::remove(undone.begin(), undone.end(), curr), undone.end());
                                done.push_back(curr);  
                                writeFileUndone(filename_midP, curr, time);
                                delete [] randsheep;
                                curr = findNearest(s_point,g_point, meet_point, undone, player.pos, n, total_p);
                                
                            }else{
                                //把在中繼點的趕到下一個中繼點
                                // cout<< " state: "<<"2 ";
                        
                                bool **check = new bool*[n];
                                int rand_n = 0;
                                int move_n = 0;
                                int group_ind = -1 ;
                                    
                                for(int i=0;i<n;i++){
                                    check[i] = new bool[group_n[i]];
                                    for(int j=0;j<group_n[i];j++){
                                        if(path[i][total_sheeps[i][j].getcurrPos()] == curr){
                                            check[i][j] = true;
                                            move_n++;
                                            group_ind = i ;
                                        }else{
                                            check[i][j] = false;
                                            rand_n++;
                                        }
                                    }
                                }
                                    
                                Sheep *movesheep = new Sheep[move_n];
                                Sheep *randsheep = new Sheep[rand_n];
                                int count1 = 0;
                                int count2 = 0;
                            
                                for(int i=0;i<n;i++){
                                    for(int j=0;j<group_n[i];j++){
                                        if(check[i][j]){
                                            movesheep[count1] = total_sheeps[i][j];
                                            count1++;
                                        }else{
                                            randsheep[count2] = total_sheeps[i][j];
                                            count2++;
                                        }
                                    }
                                }

                                pair <float,float> goal;
                                if( path[group_ind][movesheep[count1-1].getcurrPos()+1] == -1){
                                    goal = target;
                                }else{
                                    if(path[group_ind][movesheep[count1-1].getcurrPos()+1] >= n){
                                        goal.first = meet_point[path[group_ind][movesheep[count1-1].getcurrPos()+1]-n][0];
                                        goal.second =  meet_point[path[group_ind][movesheep[count1-1].getcurrPos()+1]-n][1];
                                    }else{
                                         goal.first = g_point[path[group_ind][movesheep[count1-1].getcurrPos()+1]][0];
                                        goal.second =  g_point[path[group_ind][movesheep[count1-1].getcurrPos()+1]][1];
                                    }
                                }
                                
                                // cout<<" to:"<<path[group_ind][movesheep[count1-1].getcurrPos()+1];
                                // pair <float,float> goal(meet[path[group_ind][movesheep[count1-1].getcurrPos()+1]-n][0], meet[path[group_ind][movesheep[count1-1].getcurrPos()+1]-n][1]);
                                //cout<<goal.first<<" "<<goal.second<<endl;
                                time_pass = black_box(&player,movesheep,goal,move_n,limit,randsheep,rand_n,target, filename_move);

                                count1 = 0;
                                count2= 0;
                                for(int i=0;i<n;i++){
                                    for(int j=0;j<group_n[i];j++){
                                        if(check[i][j]){
                                            total_sheeps[i][j] = movesheep[count1];
                                            count1++;
                                            total_sheeps[i][j].setcurrPos(total_sheeps[i][j].getcurrPos()+1);
                                        }else{
                                            total_sheeps[i][j] = randsheep[count2];
                                            count2++;
                                        }
                                    }
                                }

                                delete [] randsheep;
                                delete [] movesheep;
                                undone.erase(std::remove(undone.begin(), undone.end(), curr), undone.end());
                                done.push_back(curr);
                                writeFileUndone(filename_midP, curr, time);
                                curr = findNearest(s_point,g_point, meet_point, undone, player.pos, n, total_p);
                            }

                            //update undone
                            for(int k=0;k<total_p;k++){
                                for(int l=0;l<leaf[k].size();l++){
                                    for(int r=0;r<done.size();r++){
                                        if(done[r]==leaf[k][l]){
                                            leaf[k].erase(std::remove(leaf[k].begin(), leaf[k].end(), done[r]), leaf[k].end());
                                            //cout<<done[r]<<" "<<k+n<<endl;
                                        }
                                    }
                                }

                                if(find(undone.begin(), undone.end(), k) == undone.end()&& find(done.begin(), done.end(), k) == done.end()){
                                    if(leaf[k].empty()){
                                        undone.push_back(k);
                                        curr = findNearest(s_point,g_point,meet_point,undone, player.pos,n ,total_p);
                                    }
                                }
                            }
                            
                            // cout<<" "<<curr;
                            time += time_pass ; 
                            // cout<<" pass_time:"<<time_pass<<endl;
                            time_pass = 0;
                            // if(time>50000){
                            //     gameover = true;
                            //     cout<<"time > 50000";
                            // }
                            // for(int i=0;i<n;i++){
                            //     printStatus(total_sheeps[i],group_n[i], time, &player, 1, i);
                            // }

                        }
                        
                            
                            undone.clear();
                            done.clear();
                            // cout<<"total time : "<< time << endl;
                            avg_time += time;
                    
                    // cout<<"total time : "<< time << endl;
                    // for (int i = 0; i < n; i++){
                    //     cout << "sheep " << i  << " : " << total_sheeps[i]->getPos().first << ", " <<  total_sheeps[i]->getPos().second << ", status: " << total_sheeps[i]->getSta() << endl;
                    // }
                    for( int i=0;i<n_of_s;++i )
                        {
                            delete [] s_point[i];
                            
                        }
                        delete [] s_point;
                    for (int i=0;i< n; i++){
                        delete [] path[i];
                        delete [] g_point[i];
                        delete [] total_sheeps[i];
                    }
                        delete [] g_point;
                        delete [] total_sheeps;
                        delete [] path;
                        for( int i=0;i<m;++i )
                        {
                            delete [] meet_point[i];
                        }
                        delete [] meet_point;
                        delete [] group_n;
                        delete [] group_index;
                        } 
                    }
            
                randseed += 1000;
            }
            avg_time = avg_time/iteration_time;
            cout<<input_n<<"\t"<<input_index<<"\t"<<avg_time<<endl ;



            // allocate memory
            // for (auto i = 0; i < max_n; i++){
            //     paths[i].clear();
            // }
        }
    }
}
