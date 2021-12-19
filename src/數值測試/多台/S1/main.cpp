#include <iostream>
#include <fstream>
#include <list> 
#include <stdlib.h>
#include <utility>
#include <cmath>
#include <random> 
#include <climits>
#include <algorithm>
// #include "line.h"
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
void writeFile(string filename, Sheep * sheeps_posi,int n, Player* player, int m){
    fstream f;
    f.open(filename, ios::app);

    for (int it = 0; it < n; it++){
        f << sheeps_posi[it].getIndex() << " " << sheeps_posi[it].getPos().first << " " << sheeps_posi[it].getPos().second << "\t";
    }
    for (int i = 0; i < m; i++){
        f << player[i].pos.first << " " << player[i].pos.second << "\t";
    }
    f << "\n";
    f.close();
}

void writeFile_S(string filename, Sheep * sheeps_posi,int n, Sheep* randsheeps, int n_o, Player *player, int m){
    fstream f;
    f.open(filename, ios::app);

    for (int it = 0; it < n; it++){
        f << sheeps_posi[it].getIndex() << " " << sheeps_posi[it].getPos().first << " " << sheeps_posi[it].getPos().second << "\t";
    }
    for (int it = 0; it < n_o; it++){
        f << randsheeps[it].getIndex() << " " << randsheeps[it].getPos().first << " " << randsheeps[it].getPos().second << "\t";
    }

    for (int i = 0; i < m; i++){
        f << player[i].pos.first << " " << player[i].pos.second << "\t";
    }
    f << "\n";
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

//計算距離
float distanceObj(pair<float, float> obj1, pair<float, float> obj2){
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
            sheeps[i].setData(hori, verti, 0, i);
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
            sheeps[i].setData(hori, verti, 0, i);
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
            sheeps[i].setData(hori, verti, 0, i);
            list[i] = pair<float, float>(hori, verti);
        }
    }
}


void checkSheepStatus(Sheep *sheeps,vector<int> ava, int n, Player *player, int m, pair<float, float> target, pair<float, float> goal){
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
    //    if(sheeps[i].getPos().first ==  target.first && sheeps[i].getPos().second == target.second){
        if(find(ava.begin(), ava.end(), i) != ava.end()){

            if(distanceObj(sheeps[i].getPos(), target) <= range || distanceObj(sheeps[i].getPos(), goal) <= 2){
                    // cout << "set sheep " << i << "'s status to 2\n";  
                    sheeps[i].setStatus(2);
            }else{
                    dis = INT_MAX;
                    // find 
                    sheeps[i].playerReset();

                    // 先判斷無人機在經過的路徑上有沒有影響
                    bool flag = false;
                    for(int j = 0; j < m; j++){
                        // if(player[j].status == -2 || player[j].status == i){

                            dis = distanceObj(sheeps[i].getPos(), player[j].pos);

                            // cout << i << " "  << j << ": " << dis << endl;
                            if(dis <= 2){
                                flag = true;
                                // cout << "because player " << j << ", set sheep " << i << "'s status to 1\n";
                                sheeps[i].setStatus(1);
                                // sheeps[i].calV0(player);  <- 要等到scan完所有無人機才能算(搬到sheepMover)
                                sheeps[i].addPlayer(j);
                            }
                        // }
                    }
                    if(!flag){
                        sheeps[i].setStatus(0);
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


void method3(Player *player, Sheep *sheeps, vector<int> movesheep, pair <float, float> target, int n, int limit, int m, int group_n){
    // 不可選擇的羊隻(更改指定無人機方法)
    vector <int> n_ava;
    for(int i = 0; i < n; i++){
        n_ava.push_back(i);
    }
    for(int i = 0; i < movesheep.size(); i++){
        n_ava.erase(std::remove(n_ava.begin(), n_ava.end(), movesheep[i]), n_ava.end());
    }
    int index[m];
    int curr_index;
    float dir_x, dir_y;
    for(int j = 0; j < m; j++){
        if(player[j].status == group_n){
            index[j] = findFarest(sheeps, target, n, n_ava);
            // cout << index[j] << endl;
            n_ava.push_back(index[j]);
        }

    }
    // 決定指派給哪台無人機
    for(int i = 0; i < m; i++){
        for(int j = i+1; j < m; j++){
            if(player[i].status == group_n && player[j].status == group_n){
                if(index[i] != -1 and index[j] != -1){
                    if(distanceObj(sheeps[index[i]].getPos(), player[j].pos)+distanceObj(sheeps[index[j]].getPos(), player[i].pos) <
                    distanceObj(sheeps[index[i]].getPos(), player[i].pos)+distanceObj(sheeps[index[j]].getPos(), player[j].pos)){
                        // swap
                        curr_index = index[i];
                        index[i] = index[j];
                        index[j] = curr_index;
                    }
                }
            }
        }        
    }
    for(int j = 0; j < m; j++){
        // 決定去哪個位置
        if(player[j].status == group_n){

        
        pair <float, float> tarPos;
        pair <float, float> dir;
        if(index[j] != -1){
        
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
            tarPos.first = sheeps[index[j]].getPos().first + dir_x;
            tarPos.second = sheeps[index[j]].getPos().second + dir_y;
            dir.first = tarPos.first - player[j].pos.first;
            dir.second = tarPos.second - player[j].pos.second;
        }else{
            dir = pair <float, float>(limit,limit);
        }
        // cout << dir_x << " " << dir_y << endl;
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
        
        // cout << dir.first << " " << dir.second << endl;
        player[j].currV = pair <float, float>(dir.first, dir.second);
        // player[j].path.p1 = Point(player[j].pos.first, player[j].pos.second);
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

int black_box(Player *player,int m_of_D, Sheep *sheeps, int max_n,vector <int> * movesheep, int group_n, pair<float, float> *meet,int  m,
pair <float, float> *goal,int limit, pair<float, float> desti,string filename = ""){
    bool done = false ;
    bool anydone; 
    int time = 0 ;
    vector <int> randsheep;
    for(int i = 0; i < max_n; i++){
        randsheep.push_back(i);
    }
    for(int i = 0; i < group_n; i++){
        for(auto it2 = begin(movesheep[i]); it2 != end(movesheep[i]); ++it2){
            if(find(randsheep.begin(), randsheep.end(), *it2) != randsheep.end()){
                randsheep.erase(std::remove(randsheep.begin(), randsheep.end(), *it2), randsheep.end());
            }
        }
    }
    vector<int> totalSheep;
    for(int i = 0; i < max_n; i++){
        totalSheep.push_back(i);
    }
    for(int i = 0; i < group_n; i++){
        checkSheepStatus(sheeps, movesheep[i], max_n, player, m_of_D, goal[i], desti);
        anydone = checkStatus(sheeps, movesheep[i], max_n);
        if(anydone){
            done = true;
            for(int j = 0; j < m_of_D; j++){
                if(player[j].status == i){
                    // cout << "player " << j << " is done.\n";
                    player[j].idle = true;
                }
            }
        }
    }
    // writeFile(filename, sheeps, max_n, player, m_of_D);
    // for(int iter = 0; iter < 10; iter++){
    while(!done){
        for(int i = 0; i < group_n; i++){
            sheepMover(sheeps, player, max_n, limit, movesheep[i], desti);
        }
        sheepMover(sheeps, player, max_n, limit, randsheep, desti);
        for(int i = 0; i < group_n; i++){
            method3(player, sheeps, movesheep[i], goal[i], max_n, limit, m_of_D, i);
        }
        playerMover(player, limit, m_of_D);

        for(int i =0;i< group_n;i++){
            checkSheepStatus(sheeps, movesheep[i], max_n, player, m_of_D, goal[i], desti);
            anydone = checkStatus(sheeps, movesheep[i], max_n);
            if(anydone){
                done = true;
                for(int j = 0; j < m_of_D; j++){
                    if(player[j].status == i){
                        // cout << "player " << j << " is done.\n";
                        player[j].idle = true;
                    }
                }
            }
        }
        time += 1;
        // writeFile(filename, sheeps, max_n, player, m_of_D);
    }
    // printStatus(sheeps, max_n, time, player, m_of_D);
    checkSheepStatus(sheeps, totalSheep, max_n, player, m_of_D, desti, desti);
    return time ; 
}



//找最近的羊
int findNearest(Sheep *point,pair<float, float> *meet ,vector<int> undone, pair<float, float> target, int n, int total_p, vector<int> ava = {}){
    float minD = 10000000;
    int min = -1;
    if(undone.size()>0){
        for(int i = 0; i < undone.size(); i++){
            if(find(ava.begin(), ava.end(), undone[i]) == ava.end()){
                if(undone[i]<n){
                    pair<float,float>temp(point[undone[i]].getPos().first,point[undone[i]].getPos().second);
                    if(distanceObj(temp, target) <= minD){
                        min = undone[i];
                        minD = distanceObj(temp, target);
                    }
                }else{
                    pair<float, float> meetpoint(meet[undone[i]-n].first, meet[undone[i]-n].second);
                    if(distanceObj(meetpoint, target) <= minD ){
                        min = undone[i];
                        minD = distanceObj(meetpoint, target);
                    }
                }
            }
        }
    }
    return min;
}

int find_same(vector<int> curr,  int node){
    int index = -1;
    int count = 0;
    for(auto it = curr.begin(); it != curr.end(); ++it){
        if(*(it) == node){
            index = count;
        }
        count += 1;
    }
    return index;
}

int main(){
    int limit = 1000 ;
    int m_of_D = 2;
    for(int g = 1; g <= 3; g++){
    for(int input_n = 5 ; input_n<=20 ; input_n+=5){
        for(int input_index =0 ; input_index<30; input_index++){
            string filename_move  = "../../movement/S1_case_" + ::to_string(input_n)+"_"+std::to_string(input_index) + "_" + ::to_string(m_of_D)+ "_" + ::to_string(g)+"_Mu.txt";
            // string filename_midP  = "../../midP/S1_case_" + ::to_string(input_n)+"_"+std::to_string(input_index)+ "_" +::to_string(m_of_D)+ "_" + ::to_string(g)+"_Mu.txt";
            // string filename_move  = "../../movement/S1_case_" + ::to_string(input_n)+"_"+std::to_string(input_index) + "_" + ::to_string(m_of_D)+"_MuT.txt";
            // string filename_midP  = "../../midP/S1_case_" + ::to_string(input_n)+"_"+std::to_string(input_index)+ "_" +::to_string(m_of_D)+ ".txt";
            // createFile(filename_move);
            // createFile(filename_midP);
            string filename = "../../input/S1_results_100_group/input_point_"+std::to_string(input_n)+"_"+std::to_string(input_index)+ "_" + ::to_string(g)+".txt";
            
            int randseed = 101;
            int iteration_time = 100;
            
            int max_n  ; //幾群羊
            int time = 0 ;
            int m  ; //中繼點數量
            float avg_time = 0 ;
            float playerPath[m_of_D];
            for(int i = 0 ; i < m_of_D; i++){
                playerPath[i] = 0;
            }
            for(int iteration = 0 ; iteration<iteration_time ; iteration++){
                time = 0;
                srand(randseed);
                
                bool read = true;
                vector <int> n_ava;
                pair <float, float> *nodeList;
                pair <float, float> *meet;
                float max_x=0, max_y=0;
                vector<int> *paths;
                
                pair <float, float> target(0,0);
                readfile(&read, filename, &max_n, &m, nodeList, meet, paths, &max_x, &max_y);
                int total_p = m + max_n ;//中繼點＋羊群數量
                // 建立羊群物件
                Sheep *sheeps = new Sheep[max_n];
                for(int i=0 ; i<max_n;i++){
                    sheeps[i].setData(nodeList[i].first, nodeList[i].second, 0, i);
                }

                // 建立pre_group (中繼點各自匯集的點集合)
                vector<int> pre_group[m+1];
                int keys[total_p+1];
                for(int i=0; i<max_n; i++){
                    for(auto it = paths[i].begin(); it != paths[i].end()-1; ++it){
                        keys[*(it)] = *(it+1);
                        if(*(it+1) != -1){
                            pre_group[*(it+1)-max_n].push_back(*it);
                            for(auto it2 = paths[i].begin(); it2 != it+1; ++it2){
                                if (find(pre_group[*(it+1) - max_n].begin(), pre_group[*(it+1) - max_n].end(), *(it2)) == pre_group[*(it+1) - max_n].end()){
                                    pre_group[*(it+1) - max_n].push_back(*(it2));
                                } 
                            }
                        }else{
                            pre_group[m].push_back(*it);
                            for(auto it2 = paths[i].begin(); it2 != it+1; ++it2){
                                if (find(pre_group[m].begin(), pre_group[m].end(), *(it2)) == pre_group[m].end()){
                                    pre_group[m].push_back(*(it2));
                                } 
                            }
                        }
                    }
                }
                // for(int i = 0; i < m+1; i++){
                //     cout << "PreGroup of mid point " << i+max_n << " : ";
                //     for(auto it = pre_group[i].begin(); it != pre_group[i].end(); ++it){
                //         cout << *it << "\t";
                //     }
                //     cout << endl;
                // }
                // 遊戲用
                vector<int> undone;
                vector<int> done;
                vector<int> processing;
                for(int i = 0 ; i < max_n ; i++){
                    undone.push_back(i);
                }
                Player player[m_of_D];
                for(int i = 0; i < m_of_D; i++){
                    player[i].pos = pair <float, float>(0, 0);
                    player[i].idle = true;
                    player[i].path = 0;
                }
                vector <int> curr;
                pair<float, float> goal[m_of_D];
                vector <int> movesheep[m_of_D];
                bool conti;
                bool gameover = false;
                int pass_time ;
                // group_n 每次共有幾群(可能會有複數台無人機負責同一群
                int curr_node, next_node, group_n;
                // /*
                // for(int j = 0; j < 2; j++){
                // cout << undone.size() << " " << gameover << endl;
                while(undone.size() > 0 && !gameover){
                    // n_ava.clear();
                    curr.clear();
                    processing.clear();
                    group_n = 0;
                    conti = false;
                    for(int i = 0; i < m_of_D; i++){
                        // cout << player[i].idle << endl;
                        if(player[i].idle){
                            curr_node = findNearest(sheeps, meet, undone, player[i].pos, max_n, total_p, n_ava);
                        }else{
                            curr_node = player[i].curr;
                        }
                        if(curr_node != -1){
                            conti = true;
                            player[i].idle = false;
                            player[i].curr = curr_node;
                            if(find_same(curr, curr_node) == -1){
                                player[i].status = group_n;
                                curr.push_back(curr_node);
                                group_n += 1;
                            }else{
                                player[i].status = find_same(curr, curr_node);
                            }
                            if(curr_node < max_n){
                                // 只有趕一隻羊，不需兩台無人機
                                if(find(n_ava.begin(), n_ava.end(),curr_node) == n_ava.end()){
                                    n_ava.push_back(curr_node);
                                }
                            }
                        }
                        // cout << "Drone " << i << " is going to group " << player[i].status << ", curr node: " << player[i].curr << endl;

                    }
                    
                    for(int m = 0; m < group_n; m++){
                        processing.push_back(curr[m]);
                    }
                    // cout << done.size() << endl;
                    if(done.size() != 0){
                        time += 1;
                    }
                    // writeFileUndone(filename_midP, processing, time);
                        // processing.clear();
                    // }
                    // 如果所有無人機都沒有可趕的羊（遊戲結束）
                    if(!conti){
                        gameover = true;
                    }else{
                        // 找出無人機各自負責的羊群，剩下的即為隨機走動羊群
                        for(int i = 0; i < group_n; i++){
                            movesheep[i].clear();
                            if(curr[i] != -1){
                                if(curr[i] < max_n){
                                    movesheep[i].push_back(curr[i]);
                                }else{
                                    for(auto it = pre_group[curr[i]-max_n].begin(); it != pre_group[curr[i]-max_n].end(); ++it){
                                        if(*it < max_n){
                                            movesheep[i].push_back(*it);
                                        }
                                    }
                                }

                                next_node = keys[curr[i]];
                                // player[i].status = next_node;
                                if(next_node != -1){
                                    goal[i] = pair<float, float>(meet[next_node-max_n].first, meet[next_node-max_n].second);
                                }else{
                                    goal[i] = target;
                                }
                            }
                            // cout << "Group " << i << " is move sheep [ "; 
                            // for(auto it = movesheep[i].begin(); it != movesheep[i].end(); ++it){
                            //     cout << *it << "\t";
                            // }
                            // cout << " ], goal is  (" << goal[i].first << ", " << goal[i].second << ")" << endl;

                        }
                        pass_time = black_box(player, m_of_D, sheeps, max_n, movesheep, group_n, meet, m, goal, limit, target, filename_move);

                        time += pass_time;
                        // cout << "time: " << time << endl;
                        // 更新undone
                        for(int i = 0; i < m_of_D; i++){
                            if(player[i].idle){
                                // player i 負責的group 已經跑完了
                                int node = curr[player[i].status];
                                // cout << "node " << node << " is done." << endl;
                                undone.erase(std::remove(undone.begin(), undone.end(), node), undone.end());
                                n_ava.erase(std::remove(undone.begin(), undone.end(), node), undone.end());
                                done.push_back(node); 
                                if(find(processing.begin(), processing.end(), node) != processing.end()){
                                    processing.erase(std::remove(processing.begin(), processing.end(), node), processing.end());
                                }
                            }
                        }
                        if(pass_time == 0){
                            time += 1;
                        }
                        // writeFileUndone(filename_midP, processing, time);
                        for(int i = 0; i < m; i++){
                            if(find(done.begin(), done.end(), i+max_n) == done.end() && find(undone.begin(), undone.end(), i+max_n) ==undone.end() && pre_group[i].size() > 0){
                                bool flag = true;
                                for(auto it = pre_group[i].begin(); it != pre_group[i].end(); ++it){
                                    if(find(done.begin(), done.end(), *it) == done.end()){
                                        flag = false;
                                        break;
                                    }
                                }
                                if(flag){
                                    undone.push_back(i+max_n);
                                }
                            }
                        }

                        // writeFileUndone(filename_midP, done, time);
                    }
                    // for(auto it = undone.begin(); it != undone.end(); ++it){
                    //     cout << *it << "\t";
                    // }
                    // cout << endl;

                    
                }
                // cout << "total time: " << time << endl;
                delete [] sheeps;
                for( int i=0;i<m_of_D; i++){
                    playerPath[i] += player[i].path;
                }
                avg_time += time;
                randseed += 1000;
                    // */
            }
            avg_time = avg_time/iteration_time;
            // cout<<input_n<<"\t"<<input_index<<"\t"<<avg_time<<endl ;
            cout<<input_n<<"\t"<<input_index<<"\t"<<g << "\t"<<avg_time;
            float totalPath = 0;
            for(int i = 0; i < m_of_D; i++){
                // cout << "\t" << playerPath[i]/iteration_time;
                totalPath += playerPath[i];
            }
            cout << "\t" << totalPath/(iteration_time);
            cout<<endl ;
        }
    }
}
}



