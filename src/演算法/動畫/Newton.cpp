//矩形場域
//舊牛頓法
//undone修正
//讀檔
//可跑多次算平均時間
//無人機起始位置為最大的x、y座標
#include <iostream>
#include <cmath>
#include <vector>
#include <algorithm>
#include <random>
#include <fstream> 
using namespace std;
int a = -1;

void createFile(string filename){
    fstream f;
    f.open(filename, ios::out | ios::trunc);
    f.close();
}

void readfile(bool *read, string filename, int *N, int *M,  pair <float, float> *&posi,pair <float, float> *&meet, vector<int> *&paths, float* max_x, float* max_y){
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
    int index;
    float x,y,dist;
    bool flag = true;
    string token, delimiter = "\t";
    size_t pos;
    while(flag){
        input >> c;
        //cout<<c;
        switch(c){
            case 'n':
                input >> *N;
                // cout << *N << endl;
				posi = new pair<float, float>[*N];
                paths = new vector<int>[*N];
                getline(input, line);
                break;
            case 'm':
                input >> *M;
                // cout << *M << endl;
				meet = new pair<float, float>[*M];
                getline(input, line);
                break;
            case 's':
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
                getline(input, line);
                break;
            case 'h':
                input >> index >> x >> y ;
                meet[index-*N] = pair<float, float>(x,y);
                getline(input, line);
                break;
            case 'g':
                //input >> x >> y;
                //getline(input, line);
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
                flag = false;
                break;
        }
    }
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
        pair<float, float> getV(){
            return currV;
        }
        int getIndex(){ return this->index;}
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

void writeFileUndone(string filename, vector<int> undone, int time){
    fstream f;
    f.open(filename, ios::app);
    f << time << "\t";
    for (auto it = undone.begin(); it != undone.end(); ++it){
        f << *it << "\t";
    }
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
    float hori;
    float verti;
    default_random_engine generator(rand());
    uniform_real_distribution<float> unif(-0.5, 0.5);
    hori = unif(generator); 
    verti = unif(generator); 
    // cout<<hori<<"\t"<<verti;

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
    if(dir.first == 0.0 && dir.second == 0.0){
        // 在正中間
        dir = pair <float, float>((rand() % (3) - 1)*1.5, (rand() % (3) - 1)*1.5);
    }
    this->currV = dir; 
}

//產生隨機位置的羊
void generateRandSheep(Sheep *sheeps, int n, int limit, pair <float, float> *list){
    for(int i = 0; i < n; i++){
        int hori = rand()%limit;
        int verti = rand()%limit;
        sheeps[i].setData(hori, verti, 0,i);
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
       if(dis<range){ 
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
                if (dis >= 2){
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
int findNearest(float **point,float **meet ,vector<int> undone, pair<float, float> target, int n, int total_p, int type = 0){
    float minD = 10000000;
    int min = -1;
    if(undone.size()>0){
        for(int i = 0; i < undone.size(); i++){
            if(type == 1){
                //cout << undone[i]<<endl;
            }
            if(undone[i]<n){
                pair<float,float>temp(point[undone[i]][0],point[undone[i]][1]);
                if(type == 1){
                    // cout << point[undone[i]][0]<<" "<<point[undone[i]][1]<<endl;
                    cout<< target.first<<" "<<target.second<<endl;
                    // cout << distanceObj(temp, target)<<endl;
                }
                if(distanceObj(temp, target) <= minD){
                    min = undone[i];
                    minD = distanceObj(temp, target);
                }
            }else{
                pair<float, float> meetpoint(meet[undone[i]-n][0], meet[undone[i]-n][1]);
                if(type == 1){
                    // cout<<meet[undone[i]-n][0]<<" "<<meet[undone[i]-n][1]<<endl;
                    cout<< target.first<<" "<<target.second<<endl;
                    // cout << distanceObj(meetpoint, target)<<endl;
                }
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
    // if(player->currV.first>5 || player->currV.first<-5||player->currV.second>5||player->currV.second<-5){
    //     cout<<"currV: "<<player->currV.first<<" "<<player->currV.second<<endl;
    // }
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
    // cout << "dir 1 :"<< dir.first << " " << dir.second << "\t";
    
    // if(dir.first != 0.0 && dir.second != 0.0){
    //     if(abs(dir.first) > 5){
    //         dir.first /= abs(dir.first)/5;
    //     }
    //     if(abs(dir.second) > 5){
    //         dir.second /= abs(dir.second)/5;
    //     }
    // }else{
    //     if(dir.first == 0.0){
    //         if(abs(dir.second) > 5){
    //             dir.second = 5;
    //         }
    //     }else{
    //         if(abs(dir.first) > 5){
    //             dir.first = 5;
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
    // if(dir.first>5 || dir.first<-5||dir.second>5||dir.second<-5){
    //     cout<<"currV: "<<player->currV.first<<" "<<player->currV.second<<endl;
    // cout << "dir :"<< dir.first << " " << dir.second << endl;
    // }
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

    // if(dir.first != 0.0 && dir.second != 0.0){
    //     if(abs(dir.first) > 5){
    //         dir.first /= abs(dir.first)/5;
    //     }
    //     if(abs(dir.second) > 5){
    //         dir.second /= abs(dir.second)/5;
    //     }
    // }else{
    //     if(dir.first == 0.0){
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
    
    player->currV = pair <float, float>(dir.first, dir.second);
}

int black_box(Player *player, Sheep *sheeps, pair <float, float> target, int n, int limit , Sheep *randsheeps , int n_o, pair <float, float> goal, string filename){
    bool done = false ; 
    int time = 0 ;
    pair <float, float> gcm(0, 0);
    pair <float, float> lcm(0, 0);
    int far_sheep = -1;
    Sheep *allsheeps = new Sheep[n+n_o];
    //cout<<"black box tar:"<<target.first<<","<<target.second;
    int k = 0 ;

    for(int i=0;i<n;i++){
        allsheeps[k] = sheeps[i];
        k++;
    }
    for(int j=0;j<n_o;j++){
        allsheeps[k] = randsheeps[j];
        k++;
    }
    
    checkSheepStatus(sheeps, n, player->pos, target,goal);
    done = checkStatus(sheeps, n);
    writeFile(filename, sheeps, n, randsheeps, n_o, player->pos);
    while(!done && time < 50000){

        gcm = calculateGCM(allsheeps, n);
        far_sheep = findFarest(allsheeps, gcm, n);
        lcm = calculateLCM(allsheeps, far_sheep, n);
        
        checkSheepStatus(sheeps, n, player->pos, target, goal);
        /*
        cout<<endl<<"black box time: "<<time<<endl;
        cout<< player->pos.first << "\t" << player->pos.second << endl ;
        for( int i = 0 ; i < n ; i++){
            pair <float, float> test = sheeps[i].getPos() ;
            cout<<sheeps[i].getSta()<<"\t" <<test.first << "\t" << test.second <<endl ;
        }
        */
        
        method3(player, sheeps, target, n, limit);
        // sheepGather(sheeps, far_sheep, lcm, limit);
        // sheepGather(randsheeps, far_sheep, lcm, limit);
        sheepMover(sheeps, n, limit, goal);
        sheepMover(randsheeps, n_o, limit, goal);
        playerMover(player,limit);
        checkSheepStatus(sheeps, n, player->pos, target, goal);
        writeFile(filename, sheeps, n, randsheeps, n_o, player->pos);
        done = checkStatus(sheeps, n);
        time += 1;
    }
    checkSheepStatus(sheeps, n, player->pos, goal, goal);
    return time ; 
}

void generateSheepGroup(Sheep *sheeps, int n, int limit,  float x,float y,float r, int ind){
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
            sheeps[i].setData(x, y, 0, ind);
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



int main(){
    int max_limit = 1000 ;
    int group_n = 1; //一群有幾隻
    float r = 5; //羊群的半徑
    for(int input_n = 20 ; input_n<=20 ; input_n+=5){
        for(int input_index =0 ; input_index<1; input_index++){
            string filename_move  = "movement/nk_case_" + ::to_string(input_n)+"_"+std::to_string(input_index)+".txt";
            string filename_midP  = "midP/nk_case_" + ::to_string(input_n)+"_"+std::to_string(input_index)+".txt";
            createFile(filename_move);
            createFile(filename_midP);
            string filename = "input/N_results_100/input_point_"+std::to_string(input_n)+"_"+std::to_string(input_index)+".txt";
            
            int randseed = 101;
            int iteration_time = 1;
            //srand(time(NULL));
            
            int max_n  ; //幾群羊
            int time = 0 ;
            //float point [5][2] = { {63,14},{44,1},{9,31},{66,86},{20,75} } ;
            //float meet [4][2] = { {28.44, 65.89},{40.31, 2.57},{9.01, 9.29},{13.39, 2.76} } ;
            //int path[5][6] =  {{0,6,8,-1},{1,6,8,-1},{2,7,8,-1},{3,5,7,8,-1},{4,5,7,8,-1} };
            int m  ; //中繼點數量
            float avg_time = 0 ;
            for(int iteration = 0 ; iteration<iteration_time ; iteration++){
                time = 0;
                srand(randseed);
                
                bool read = true;
                pair <float, float> *nodeList;
                pair <float, float> *meet;
                float max_x=0, max_y=0;
                vector<int> *paths;
                
                readfile(&read, filename, &max_n, &m, nodeList, meet, paths, &max_x, &max_y);
                int total_p = m + max_n ;//中繼點＋羊群數量
                

                for(int limit = max_limit; limit <= max_limit; limit++){
                    for(int n = max_n; n <= max_n; n++){
                        
                        float **point = new float*[n];
                        for(int i=0;i<n;i++){
                            point[i]=new float[2];
                            point[i][0] = nodeList[i].first;
                            point[i][1] = nodeList[i].second;
                        }

                        float **meet_point = new float*[m];
                        for(int i=0;i<m;i++){
                            meet_point[i]=new float[2];
                            meet_point[i][0] = meet[i].first;
                            meet_point[i][1] = meet[i].second;
                        }
                        
                        int **path = new int*[n];
                        for(int i=0;i<n;i++){
                            path[i] = new int[paths[i].size()];
                            for(int j=0;j<paths[i].size();j++){
                                path[i][j] = paths[i][j];
                            }
                        }
                    
                        Player player(max_x, max_y);
                        pair <float, float> target(0, 0);
                        Sheep **total_sheeps = new Sheep*[n];
                        int curr; //記錄無人機的目標點
                        int time_pass = 0;
                        bool gameover = false;

                        vector <vector <int> > leaf;
                        for(int k=0;k<m;k++){
                            vector <int> branch;
                            for(int i=0;i<max_n;i++){
                                for(int j=0;j<paths[i].size();j++){
                                    if(path[i][j]== k+max_n ){
                                        branch.push_back(i);
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

                        
                        for(int i=0;i<n;i++){
                            Sheep *sheeps = new Sheep[group_n];
                            generateSheepGroup(sheeps, group_n, limit, point[i][0],point[i][1],r, i);
                            total_sheeps[i] = sheeps;
                            // printStatus(sheeps,group_n, time, &player, 1);
                        }


                        vector <int> undone;
                        vector <int> done;

                        for(int i = 0 ; i<n ; i++){
                            undone.push_back(i);
                        }
                        
                        //無人機初始目標（找最近的點）
                        curr = findNearest(point, meet_point, undone, player.pos, n, total_p);

                        
                        while(!undone.empty() && !gameover && time < 50000){
                            
                            // cout<<"time:"<<time ;
                            // cout <<" curr:"<<curr<<" ";
                            // cout<<" ("<<player.pos.first<<","<<player.pos.second<<") \n";
                            
                            
                            if(curr == -1){
                                //findnearest error
                                curr = findNearest(point,meet_point,undone, player.pos,n ,total_p, 1);
                                
                                if(curr == -1){
                                    cout<< " error! ";
                                    gameover = true;
                                }
                            }
                            /*else if(done.size()==total_p-1){
                                //最後一個中繼點
                                // cout<< " to:goal";
                                int k = 0 ;
                                int allsheep_n = group_n*n;
                                Sheep *allsheep = new Sheep[allsheep_n];
                                Sheep *randsheep = new Sheep[0];
                                for(int i=0;i<n;i++){
                                    for(int j=0;j<group_n;j++){
                                        allsheep[k] = total_sheeps[i][j];
                                        k++;
                                    }
                                }

                                time_pass = black_box(&player,allsheep,target,allsheep_n,limit,randsheep,0,target);

                                k = 0 ;
                                for(int i=0;i<n;i++){
                                    for(int j=0;j<group_n;j++){
                                        total_sheeps[i][j] = allsheep[k] ;
                                        k++;
                                    }
                                }
                                undone.erase(std::remove(undone.begin(), undone.end(), curr), undone.end());
                                done.push_back(curr);

                            }*/
                            else if(curr<n){ 
                                //把羊群趕到中繼點
                                //cout<< " state: "<<"1 ";
                                pair <float,float> goal;
                                if(path[curr][1] == -1){
                                    goal = target;
                                }else{
                                    goal.first = meet_point[path[curr][1]-n][0];
                                    goal.second =  meet_point[path[curr][1]-n][1];
                                }

                                Sheep *randsheep = new Sheep[group_n*(n-1)];
                                int k = 0 ;
                                for(int i=0;i<n;i++){
                                    if(i!=curr){
                                        for(int j=0;j<group_n;j++){
                                            randsheep[k] = total_sheeps[i][j];
                                            k++ ;
                                        }
                                    }
                                }
                    
                                // cout<<" to:"<<path[curr][1];
                                // pair <float,float> goal1(point[curr][0], point[curr][1]); 
                                // time_pass += black_box(&player,total_sheeps[curr],goal1,group_n,limit,randsheep, k-1, target);

                                time_pass = black_box(&player,total_sheeps[curr],goal,group_n,limit,randsheep,k,target, filename_move);

                                k = 0 ;
                                for(int i=0;i<n;i++){
                                    if(i!=curr){
                                        for(int j=0;j<group_n;j++){
                                            total_sheeps[i][j] = randsheep[k];
                                            k++ ;
                                        }
                                    }else{
                                        for(int j=0;j<group_n;j++){
                                            total_sheeps[i][j].setcurrPos(1);
                                        }
                                    }
                                }

                                undone.erase(std::remove(undone.begin(), undone.end(), curr), undone.end());
                                done.push_back(curr);  
                                writeFileUndone(filename_midP, done, time);
                                delete [] randsheep;
                                curr = findNearest(point, meet_point, undone, player.pos, n, total_p);
                            
                            }else{
                                //把在中繼點的趕到下一個中繼點
                                //cout<< " state: "<<"2 ";
                                bool temp[n][group_n];
                                int rand_n = 0;
                                int move_n = 0;
                                int group_ind = -1 ;
                                
                                for(int i=0;i<n;i++){
                                    for(int j=0;j<group_n;j++){
                                        if(path[i][total_sheeps[i][j].getcurrPos()] == curr){
                                            temp[i][j] = true;
                                            move_n++;
                                            group_ind = i ;
                                        }else{
                                            temp[i][j] = false;
                                            rand_n++;
                                        }
                                    }
                                }
                                
                                Sheep *movesheep = new Sheep[move_n];
                                Sheep *randsheep = new Sheep[rand_n];
                                int count1 = 0;
                                int count2 = 0;
                                
                                for(int i=0;i<n;i++){
                                    for(int j=0;j<group_n;j++){
                                        if(temp[i][j]){
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
                                    goal.first = meet_point[path[group_ind][movesheep[count1-1].getcurrPos()+1]-n][0];
                                    goal.second =  meet_point[path[group_ind][movesheep[count1-1].getcurrPos()+1]-n][1];
                                }

                                // cout<<" to:"<<path[group_ind][movesheep[count1-1].getcurrPos()+1];
                                //pair <float,float> goal(meet_point[path[group_ind][movesheep[count1-1].getcurrPos()+1]-n][0], meet_point[path[group_ind][movesheep[count1-1].getcurrPos()+1]-n][1]);
                                time_pass = black_box(&player,movesheep,goal,move_n,limit,randsheep,rand_n,target, filename_move);

                                count1 = 0;
                                count2= 0;
                                for(int i=0;i<n;i++){
                                    for(int j=0;j<group_n;j++){
                                        if(temp[i][j]){
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
                                writeFileUndone(filename_midP, done, time);
                                curr = findNearest(point, meet_point, undone, player.pos, n, total_p);
                            }

                            //update undone
                            for(int k=0;k<m;k++){
                                for(int l=0;l<leaf[k].size();l++){
                                    for(int r=0;r<done.size();r++){
                                        if(done[r]==leaf[k][l]){
                                            leaf[k].erase(std::remove(leaf[k].begin(), leaf[k].end(), done[r]), leaf[k].end());
                                            //cout<<done[r]<<" "<<k+n<<endl;
                                        }
                                    }
                                }

                                if(find(undone.begin(), undone.end(), k+n) == undone.end()&& find(done.begin(), done.end(), k+n) == done.end()){
                                    if(leaf[k].empty()){
                                        undone.push_back(k+n);
                                        curr = findNearest(point,meet_point,undone, player.pos,n ,total_p);
                                    }
                                }
                            }
                            

                            // cout<<" undone : ";
                            // for (size_t i = 0; i < undone.size(); ++i) {
                            //    cout <<undone[i]<<" ";
                            // }
                            // cout<<" done : ";
                            // for (size_t i = 0; i < done.size(); ++i) {
                            //    cout <<done[i]<<" ";
                            // }
                            time += time_pass ; 
                            
                            // cout<<" pass_time:"<<time_pass<<endl;
                            time_pass = 0;
                            // if(time>50000){
                                // gameover = true;
                                // cout<<"time > 50000";
                            // }
                            // for(int i=0;i<n;i++){
                            //     printStatus(total_sheeps[i],group_n, time, &player, 1);
                            // }
                        }
                        // cout<<"total time : "<< time << endl;
                        avg_time += time;
                    }
                }
                randseed+=1000;
            }
            avg_time = avg_time/iteration_time;
            cout<<input_n<<"\t"<<input_index<<"\t"<<avg_time<<endl ;
        }
    }
}