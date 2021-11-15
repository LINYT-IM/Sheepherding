//矩形場域
//可跑多次算平均時間
//可讀檔
//無人機起始位置為最大的x、y座標
#include <iostream>
#include <cmath>
#include <vector>
#include <algorithm>
#include <random> 
#include <fstream>
using namespace std;
int a = -1;
#define PI 3.1415926

void createFile(string filename){
    fstream f;
    f.open(filename, ios::out | ios::trunc);
    f.close();
}




void readfile(bool *read, string filename, int *N, pair <float, float> *&posi, float* max_x, float* max_y){
    fstream input;
    input.open(filename);
    // No file
    if(!input){
        cout << "Cannot Find The File.";
        *read = false;
        return;
    }
    char c;
    //cout<<c<<" ";
    string problemType, line;
    int index;
    float x,y;
    float arr[2];
    bool flag = true;
    while(flag){
        input >> c;
        switch(c){
            // ignore commemts
            case 'n':
                input >> *N;
                    // cout << *N << endl;
                posi = new pair<float, float>[*N];
                    // dis = new float[*N];
                getline(input, line);
                break;
            case 'k':
                    // input >> x >> y;
                    // *target = pair<float, float>(x,y);
                    // cout << target->first << " " << target->second << endl;
                getline(input, line);
                break;
            case 'm':
                    // input >> *M;
                    // cout << *M << endl;
                    // paths = new vector<int>[*M];
                getline(input, line);
                break;
            case 'd':
            {
                input >> index >> x >> y;
                posi[index] = pair<float, float>(x,y);
                if(x>*max_x){
                    *max_x = x;
                }
                if(y>*max_y){
                    *max_y = y;
                }
                if(index == *N - 1){
                    flag = false;
                }
                getline(input, line);
                break;
            } 
            case 'c':
                getline(input, line);
                break;
            case 'h':
                getline(input, line);
                break;
            case 'g':
                    getline(input, line);
                break;
            case 'p':
                    // allocate the nodeList
                    // input >> index;
                    // paths[index].push_back(index);
                    // getline(input,line,'\n');
                    // pos = 0;
                    // while ((pos = line.find(delimiter)) != string::npos) {
                        // token =  line.substr(0, pos);
                        // if(token != ""){
                            // paths[index].push_back(std::stoi(token));
                        // } 
                        // line.erase(0, pos + delimiter.length());
                    // }
                // paths[index].push_back(std::stoi(line));
                getline(input, line);
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
        
        void setData(int x, int y, int status, int index);
        void setStatus(int stat){
            this->status = stat;
        };
        void setcurrPos(int pos){
            this->currPos = pos;
        }
        int getIndex(){
            return this->index;
        }
        void randMove(int limit);
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

void Sheep::setData(int x, int y, int status, int index){
    this->x = x; 
    this->y = y;
    this->status = status;
    this->currPos = 0;
    this->index = index;
}

//羊隨機走動 status = 0
void Sheep::randMove(int limit){
    int randv = 1 ;
    float hori ;
    float verti ;
    hori = ((rand() % (randv*2  + 1 )) - randv)/2.0; 
    verti = ((rand() % (randv*2  + 1 )) - randv)/2.0; 
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

void writeFile(string filename, Sheep * sheeps_posi,int n, pair<float, float> player){
    fstream f;
    f.open(filename, ios::app);

    for (int it = 0; it < n; it++){
        f << sheeps_posi[it].getIndex() << " " << sheeps_posi[it].getPos().first << " " << sheeps_posi[it].getPos().second << "\t";
    }
    f << player.first << " " << player.second << "\n";
    f.close();
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

//產生隨機位置的羊
void generateRandSheep(Sheep *sheeps, int n, int limit, pair <float, float> *list){
    for(int i = 0; i < n; i++){
        int hori = rand()%limit;
        int verti = rand()%limit;
        sheeps[i].setData(hori, verti, 0, i);
        list[i] = pair<float, float>(hori, verti);
    }
}

//設定羊的狀態
void checkSheepStatus(Sheep *sheeps, int n, pair <float, float> player, pair <float, float> target){
    float dis;
    for(int i = 0; i < n; i++){
        /*
            status = 0: 隨機走動mode
                   = 1: 驚嚇Mode
                   = 2: 抵達終點
        */
       dis = distanceObj(sheeps[i].getPos(), target);
       if(dis<2){ 
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

void printStatus(Sheep *sheeps,int n, int time, Player *player, int m){
    cout << "\tTime: " << time << endl;
    for(int i = 0; i < n; i++){
        cout << "Sheep " << i << " : (" <<  sheeps[i].getPos().first << ", " << sheeps[i].getPos().second << "), status = " << sheeps[i].getSta() << endl;  
    }
    for(int i = 0; i < m; i++){
        cout << "Player "<< i << " : (" << player[i].pos.first << ", " << player[i].pos.second << ")" << endl;
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
                    sheeps[i].randMove(limit);
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
int findNearest(float point[][2],float meet[][2] ,vector<int> undone, pair<float, float> target, int n, int total_p){
    float minD = 10000000;
    int min = -1;
    if(undone.size()>0){
        for(int i = 0; i < undone.size(); i++){
            if(undone[i]<n){
                pair<float,float>temp(point[undone[i]][0],point[undone[i]][1]);
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
    if(player->pos.first + player->currV.first > limit || player->pos.first + player->currV.first < 0){
        if(player->pos.first + player->currV.first > limit){
            player->pos.first = limit;
        }else{
            player->pos.first = 0;
        }
    }else{
        // move
        player->pos.first += player->currV.first;
    }
    if(player->pos.second + player->currV.second > limit || player->pos.second + player->currV.second < 0){
        if(player->pos.second + player->currV.second > limit){
            player->pos.second = limit;
        }else{
            player->pos.second = 0;
        }
    }else{
        // move
        player->pos.second += player->currV.second;
    }
}

//以離目標最遠的羊開始趕
void method3(Player *player, Sheep *sheeps, pair <float, float> target, int n, int limit){
    int index = findFarest(sheeps, target, n);
    float dir_x;
    float dir_y;

    if(sheeps[index].getPos().first - target.first == 0){
        dir_x = 0;
    }else{
        dir_x = (sheeps[index].getPos().first - target.first)/abs((sheeps[index].getPos().first - target.first));
    }
    if(sheeps[index].getPos().second - target.second == 0){
        dir_y = 0;
    }else{
        dir_y = (sheeps[index].getPos().second - target.second)/abs((sheeps[index].getPos().second - target.second));
    }
    //cout <<"dir_ : "<< dir_x << " " << dir_y << endl;
    pair <float, float> tarPos(sheeps[index].getPos().first + dir_x, sheeps[index].getPos().second + dir_y);
    pair <float, float> dir(tarPos.first - player->pos.first, tarPos.second - player->pos.second);
    //cout << "tar:"<< tarPos.first << " " << tarPos.second << endl;
    //cout << "dir :"<< dir.first << " " << dir.second << endl;
    
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

int black_box(Player *player, Sheep *sheeps, pair <float, float> target, int n, int limit , Sheep *randsheeps , int n_o, string filename){
    bool done = false ; 
    int time = 0 ;
    pair <float, float> gcm(0, 0);
    pair <float, float> lcm(0, 0);
    int far_sheep = -1;
    // cout<<"black box tar:"<<target.first<<","<<target.second;

    checkSheepStatus(sheeps, n, player->pos, target);
    done = checkStatus(sheeps, n);
    // for (int i=0; i<n; i++){
    //     cout << sheeps[i].getPos().first << endl;
    // }
    writeFile(filename, sheeps, n, player->pos);
    while(!done){

        gcm = calculateGCM(sheeps, n);
        far_sheep = findFarest(sheeps, gcm, n);
        lcm = calculateLCM(sheeps, far_sheep, n);
        
        checkSheepStatus(sheeps, n, player->pos, target);
        /*
        cout<<endl<<"black box time: "<<time<<endl;
        cout<< player->pos.first << "\t" << player->pos.second << endl ;
        for( int i = 0 ; i < n ; i++){
            pair <float, float> test = sheeps[i].getPos() ;
            cout<<sheeps[i].getSta()<<"\t" <<test.first << "\t" << test.second <<endl ;
        }
        */
        
        method3(player, sheeps, target, n, limit);
        //sheepGather(sheeps, far_sheep, lcm, limit);
        sheepMover(sheeps, n, limit, target);
        if(n_o > 0){
            sheepMover(randsheeps, n_o, limit, target);
        }
        playerMover(player,limit);
        // printStatus(sheeps, n, time, player, 1);
        writeFile(filename, sheeps, n, player->pos);
        done = checkStatus(sheeps, n);
        time += 1;
    }
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
            if(verti>limit){
                verti = limit;
            }
            if(hori<0){
                hori = 0;
            }
            if(verti<0){
                verti = 0;
            }
            // hori = rand()%(2*cirR) + center.first - cirR;
            // verti = rand()%(2*cirR) + center.second - cirR;
            sheeps[i].setData(hori, verti, 0, i);
        }
}


int main(){
    // ofstream ofs;
    // ofs.open("output.txt");
    
    for(int input_n = 20 ; input_n<=20 ; input_n+=5){
        for(int input_index = 1 ; input_index<2; input_index++){
            string filename_move  = "movement/case_" + ::to_string(input_n)+"_"+std::to_string(input_index)+".txt";
            createFile(filename_move);
            string filename = "input/sheep_" + ::to_string(input_n)+"_"+std::to_string(input_index)+".txt";
            // cout << filename << endl;
            bool read = true;
            float avg_time = 0;
            int randseed = 101;
            int max_n; //幾隻羊
            int max_limit = 1000 ;
            int iteration_time = 1;
            float **total_sheeps = new float*[max_n];
            pair <float, float> *nodeList;
            float max_x=0, max_y=0;

            readfile(&read, filename, &max_n, nodeList, &max_x, &max_y);
            
            
            for(int iteration = 0 ; iteration<iteration_time ; iteration++){
            
                srand(randseed);
                //srand(time(NULL));
                
                // int max_n = 5 ; //幾群羊
                int time = 0 ;
                //float point [5][2] = { {63,14},{44,1},{9,31},{66,86},{20,75} } ;
                // float point [10][2] = {{32,23},{25,20},{28,12},{22,39},{1,30},{32,10},{43,46},{9,8},{31,39},{37,29}};
                //float meet [4][2] = { {14.16, 18.68},{10.02, 20.98},{6.44, 22.66},{3.61, 3.6} } ;
                //int path[5][6] =  {{0,7,8,-1},{1,6,7,8,-1},{2,5,6,7,8,-1},{3,8,-1},{4,5,6,7,8,-1} };
                //int m = 4 ; //中繼點數量
                //int total_p = m + max_n ;//中繼點＋羊群數量
                // int group_n = 5; //一群有幾隻
                // float r = 5; //羊群的半徑
                
                
                for(int limit = max_limit; limit <= max_limit; limit++){
                    for(int n = max_n; n <= max_n; n++){
                        Player player(max_x, max_y);
                        pair <float, float> target(0, 0);
                        // Sheep **total_sheeps = new Sheep*[n];
                        int curr; //記錄無人機的目標點
                        bool gameover = false;

                        // cout<<" ("<<player.pos.first<<","<<player.pos.second<<") ";

                        
                        // Sheep **m3_total_sheeps = new Sheep*[n];

                        // for(int i=0;i<n;i++){
                            // Sheep *sheeps = new Sheep[n];
                            // generateSheepGroup(sheeps, group_n, limit, point[i][0],point[i][1],r);
                            // total_sheeps[i] = sheeps;
                            // m3_total_sheeps[i] = sheeps;
                            // printStatus(sheeps,n, time, &player, 1);
                        // }

                        int k = 0 ;
                        Sheep *m3_total_sheeps = new Sheep[n];
                        for(int i=0;i<n;i++){
                            m3_total_sheeps[i].setData(nodeList[i].first,nodeList[i].second, 0, i);
                            // for(int j=0;j<group_n;j++){
                                // m3_total_sheeps[k] = total_sheeps[i][j];
                                // k++;
                            // }
                        }
                        
                        Sheep *randsheep = new Sheep[1];

                        time += black_box(&player,m3_total_sheeps,target,n,limit,randsheep,0, filename_move);
                        // cout<<"method 3 total time: "<<time<<endl;
                        avg_time += time;
                        
                    }
                }
                randseed += 1000;
            }
            avg_time = avg_time/iteration_time;
            cout<<input_n<<"\t"<<input_index<<"\t"<<avg_time<<endl ;
            
            // if (!ofs.is_open()) {
                // cout << "Failed to open file.\n";
            // } else {
                // ofs <<input_n<<"\t"<<input_index<<"\t"<<avg_time<<"\n";
            // }

            // for (int max = 0; max < max_n; max++){
            //     delete total_sheeps[max];
            // }
            // delete [] total_sheeps;
        }
    }
    // ofs.close();
}