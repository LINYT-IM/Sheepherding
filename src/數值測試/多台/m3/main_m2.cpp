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
環境設定:1000x1000
羊隻大小:1x1
隨機移動速率:0.5/s
逃跑速率:v0 = 1.5/s, a = -1/s
R = 2
Player V = 5/s

*/

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
                getline(input, line);
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
        f << player[i].pos.first << " " << player[i].pos.second << "\t" << player[i].sta << "\t";
    }
    f << "\n";
    f.close();
}

void writeFile_S(string filename, Sheep * sheeps_posi,int n, Sheep* randsheeps, int n_o, pair<float, float> player){
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
            // hori = rand()%limit;
            // verti = rand()%limit;
            sheeps[i].setData(hori, verti, 0, i);
            list[i] = pair<float, float>(hori, verti);
        }
    }
}


void checkSheepStatus(Sheep *sheeps, int n, Player *player, int m, pair<float, float> target){
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

            bool flag = false;
            // 先判斷無人機在經過的路徑上有沒有影響
            for(int j = 0; j < m; j++){
                dis = distanceObj(sheeps[i].getPos(), player[j].pos);

                // cout << i << " "  << j << ": " << dis << endl;
                if(dis <= 2){
                    flag = true;
                    // cout << "because player " << j << ", set sheep " << i << "'s status to 1\n";
                    sheeps[i].setStatus(1);
                    // sheeps[i].calV0(player);  <- 要等到scan完所有無人機才能算(搬到sheepMover)
                    sheeps[i].addPlayer(j);
                }
            }

            if(!flag){
                sheeps[i].setStatus(0);
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
                // Point cen(lcm.first, lcm.second);
                // Point d(lcm.first + (lcm.first - target.first)*c2*R*n,lcm.second + (lcm.second - target.second)*c2*R*n);
                // Point pos1 = rotate(d, cen, 30);
                // Point pos2 = rotate(d, cen, -30);
                // dis1 = distanceObj(player[j].pos, pair<float, float> (pos1.real(), pos1.imag()));
                // dis2 = distanceObj(player[j].pos, pair<float, float> (pos2.real(), pos2.imag()));
                // if(dis1 < dis2){
                //     dir = pair <float, float>(pos1.real() - player[j].pos.first, pos1.imag() - player[j].pos.second);
                // }else{
                //     dir = pair <float, float>(pos1.real() - player[j].pos.first, pos1.imag() - player[j].pos.second);
                // }
                if (lcm.first == target.first || lcm.second == target.second){
                    dir = pair <float, float>(lcm.first + (lcm.first - target.first), (lcm.second - target.second) + lcm.second );

                }else{
                    dir = pair <float, float>(lcm.first + (lcm.first - target.first)/abs((lcm.first - target.first)), (lcm.second - target.second)/abs((lcm.second - target.second)) + lcm.second );
                }
                player[j].sta = 0;
            }else{
                // pc
                // cout << "pc" << endl;
                dir = pair <float, float>(lcm.first + (sheeps[findFarest(sheeps,gcm,n, n_ava)].getPos().first - lcm.first)*c3*R, lcm.second + (sheeps[findFarest(sheeps,gcm,n, n_ava)].getPos().second - lcm.second)*c3*R);
                n_ava.push_back(findFarest(sheeps, gcm, n, n_ava));    
                player[j].sta = 1;
            }
            // calculate player velocity
            float v_x = dir.first - player[j].pos.first;
            float v_y = dir.second - player[j].pos.second;
            if(v_x != 0 && v_y != 0){
                if(abs(v_x) > 5){
                    v_x /= abs(v_x)/5;
                }
                if(abs(v_y) > 5){
                    v_y /= abs(v_y)/5;
                }
                // player[j].currV = pair <float, float>(v_x, v_y);
            }else{
                if(v_x == 0 && v_y == 0){
                    // player[j].currV = pair <float, float>(0, 0);
                }else if(v_x == 0){
                    if(abs(v_y) > 5){
                        v_y /= abs(v_y) / 5;
                    }
                    v_x = 0;
                    // player[j].currV = pair <float, float>(0, v_y);
                }else{
                    if(abs(v_x) >5){
                        v_x /= abs(v_x) / 5;
                    }
                    v_y = 0;
                    // player[j].currV = pair <float, float>(v_x, 0);
                }

            }
            // 圓弧形跑法（增加垂直向量）
            // Point orig(v_x, v_y);
            // Point verti = rotate(orig, Point(0, 0), 90);
            // cout << orig.real() << " " <<  orig.imag() << endl;
            // cout << verti.real() << " " <<  verti.imag() << endl;
            // verti.real(verti.real() / 10);
            // verti.imag(verti.imag() / 10);
            // Point res = orig + verti;
            player[j].currV = pair <float, float>(v_x, v_y);
            
            // player[j].currV = pair <float, float>(res.real(), res.imag());
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

// pair<float, float> calculateGCM(Sheep *sheeps, int n){
//     pair<float, float> gcm(0, 0);
//     int count = 0;
//     for(int i = 0; i < n; i++){
//         if(sheeps[i].getSta() != 2){
//             gcm.first +=  sheeps[i].getPos().first;
//             gcm.second +=  sheeps[i].getPos().second;
//             count += 1;
//         }
//     }
//     // cout << "count: " << count << endl;
//     gcm.first /= count;
//     gcm.second /= count;
//     return gcm;
// }

// pair<float, float> calculateLCM(Sheep *sheeps, int index, int n){
//     pair<float, float> lcm(0, 0);
//     int count = 0;
//     for(int i = 0; i < n; i++){
//         if(sheeps[i].getSta() != 2 && i != index){
//             lcm.first +=  sheeps[i].getPos().first;
//             lcm.second +=  sheeps[i].getPos().second;
//             count += 1;
//         }
//     }
//     if(count == 0){
//         // 只剩一隻
//         lcm = sheeps[index].getPos();
//     }else{
//         lcm.first /= count;
//         lcm.second /= count;
//     }
//     return lcm;   
// }

int black_box(Player *player, Sheep *sheeps, pair <float, float> target, int n, int limit , Sheep *randsheeps , int n_o, int m, string filename = ""){
    bool done = false ; 
    int time = 0 ;
    pair <float, float> gcm(0, 0);
    pair <float, float> lcm(0, 0);
    int far_sheep = -1;
    // cout<<"black box tar:"<<target.first<<","<<target.second;

    checkSheepStatus(sheeps, n, player,m, target);
    done = checkStatus(sheeps, n);
    // for (int i=0; i<n; i++){
    //     cout << sheeps[i].getPos().first << " " << sheeps[i].getPos().second << endl;
    // }
    // writeFile(filename, sheeps, n, player,m);
    while(!done){

        gcm = calculateGCM(sheeps, n);
        far_sheep = findFarest(sheeps, gcm, n);
        lcm = calculateLCM(sheeps, far_sheep, n);
        
        checkSheepStatus(sheeps, n, player, m, target);
        // for(int i=0; i<n; i++){
        //     cout << "sheep " << i << " : " << sheeps[i].getSta() << endl;
        // }
        sheepMover(sheeps, player, n, limit);
        if(n_o > 0){
            sheepMover(randsheeps, player, n_o, limit);
        }
        method2(sheeps, player, n, m, gcm, lcm, target, limit, 1.33, 0.03, 0.9);
        // method3(player, sheeps, target, n, limit, m);
        //sheepGather(sheeps, far_sheep, lcm, limit);
        playerMover(player,limit, m);
        // printStatus(sheeps, n, time, player, 1);
        writeFile(filename, sheeps, n, player, m);
        done = checkStatus(sheeps, n);
        time += 1;
    }
    return time ; 
}

int main(){
    int m = 1;
    // for(int g = 1; g <= 3; g++){
    for(int input_n = 10 ; input_n<=10 ; input_n+=5){
        for(int input_index = 0 ; input_index<1; input_index++){
            string filename_move  = "../../movement/m3_case_" + ::to_string(input_n)+"_"+std::to_string(input_index)+ "_" + ::to_string(m) + "_Mu_test.txt";
            createFile(filename_move);
            string filename = "../../input/sheep_" + ::to_string(input_n)+"_"+std::to_string(input_index)+"test.txt";
            // cout << filename << endl;
            bool read = true;
            float avg_time = 0;
            int randseed = 101;
            int max_n; //幾隻羊
            int max_limit = 1000 ;
            int iteration_time = 1;
            // 無人機個數
            pair <float, float> *nodeList;
            float max_x=0, max_y=0;

            readfile(&read, filename, &max_n, nodeList, &max_x, &max_y);
            float **total_sheeps = new float*[max_n];
            float playerPath[m];
            for(int i = 0; i < m; i++){
                playerPath[i] = 0;
            }
            for(int iteration = 0 ; iteration<iteration_time ; iteration++){
            
                srand(randseed);
                //srand(time(NULL));
                
                // int max_n = 5 ; //幾群羊
                int time = 0 ;
                
                
                for(int limit = max_limit; limit <= max_limit; limit++){
                    for(int n = max_n; n <= max_n; n++){
                        Player *player;
                        player = new Player[m];
                        for(int i = 0; i < m; i++){
                            player[i].pos = pair<float, float>(0, 0);
                            player[i].path = 0;
                            player[i].sta = 1;
                        }
                        pair <float, float> target(0, 0);
                        // Sheep **total_sheeps = new Sheep*[n];
                        int curr; //記錄無人機的目標點
                        bool gameover = false;

                        int k = 0 ;
                        Sheep *m3_total_sheeps = new Sheep[n];
                        for(int i=0;i<n;i++){
                            m3_total_sheeps[i].setData(nodeList[i].first,nodeList[i].second, 0, i);
                            // cout << m3_total_sheeps[i].getPos().first << " " << m3_total_sheeps[i].getPos().second << endl;
                        }
                        
                        Sheep *randsheep = new Sheep[1];

                        time += black_box(player,m3_total_sheeps,target,n,limit,randsheep,0, m, filename_move);
                        // cout<<"method 3 total time: "<<time<<endl;
                        avg_time += time;
                        for(int i=0;i<m;i++){
                            playerPath[i] += player[i].path;
                        }    
                    }
                }
                randseed += 1000;
            }
            avg_time = avg_time/iteration_time;
            // cout<<input_n<<"\t"<<input_index<<"\t"<<avg_time<<endl ;
            cout<<input_n<<"\t"<<input_index<<"\t" <<avg_time;
            float totalPath = 0;
            for (int i=0; i < m; i++){
                // cout << "\t" << playerPath[i]/iteration_time;
                totalPath += playerPath[i];
            }
            cout << "\t" << totalPath/(iteration_time);
            cout <<endl ;
            
        }
    }
// }
}



