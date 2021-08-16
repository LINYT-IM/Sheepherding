#include <iostream>
#include <cmath>
#include <vector>
#include <algorithm>
#include <utility>
#include <random>
using namespace std;

struct opt{
    float score;
    vector<int> list;
};

void printout_list(vector <int> list){
    cout << "\n[ ";
    for(int i=0; i<list.size(); i++){
        cout << list[i] << " ";
    }
    cout << "]\n";
}

int farest(float *dis, int n){
    int max = -1;
    int ind = -1;
    for(int i=0; i<n; i++){
        // cout << dis[i] << endl;
        if(dis[i] > max && dis[i] > 0){
            max = dis[i];
            ind = i;
        }
    }
    return ind;
}

// 每次都挑最遠的
void method1(float *dis, int n, float V, opt *res){
    // copy dis array
    float dis_edit[n];
    int curr;
    vector <int> list;
    vector <int> undone;
    for(int i=0; i<n; i++){
        dis_edit[i] = dis[i];
        undone.push_back(i);
    }
    
    while(!undone.empty()){
        curr = farest(dis_edit, n);
        list.push_back(curr + 1);
        // 沒有加速度ver.
        // dis_edit[curr] -= V;
        // if(dis_edit[curr] <= 0){
        //     undone.erase(std::remove(undone.begin(), undone.end(), curr), undone.end());
        // }
        // 有加速度ver.
        for(int j = list.size()-1 ; j >= std::max({0, int(list.size()-V)}); j--){
            dis_edit[list[j]-1] -= V - (list.size()-1-j);
            if(dis_edit[list[j]-1] <= 0){
                undone.erase(std::remove(undone.begin(), undone.end(), list[j]-1), undone.end());
            }
        }
    }
    // printout_list(list);
    // cout << "method1: " << list.size() << " times\n";
    res->list.assign(list.begin(), list.end());
    res->score = list.size();
} 
// 輪流
void method2(float *dis, int n, float V, opt *res){
    float dis_edit[n];
    int curr_index = 0;
    int curr = 0;
    vector <int> list;
    vector <int> undone;
    for(int i=0; i<n; i++){
        dis_edit[i] = dis[i];
        undone.push_back(i);
    }
    int count = 0;
    while(!undone.empty()){
        // next index
        if(undone[curr_index] != curr || count == 0){
            // curr = undone[curr_index];
        }else{
            if(curr_index + 1 < undone.size()){
                curr_index += 1;
            }else{
                curr_index = 0;
            }
        }
        curr = undone[curr_index];
        list.push_back(curr + 1);
        // 沒有加速度ver.
        // dis_edit[curr] -= V;
        // if(dis_edit[curr] <= 0){
        //     undone.erase(std::remove(undone.begin(), undone.end(), curr), undone.end());
        // }
        // 有加速度ver.
        for(int j = list.size()-1 ; j >= std::max({0, int(list.size()-V)}); j--){
            dis_edit[list[j]-1] -= V - (list.size()-1-j);
            if(dis_edit[list[j]-1] <= 0){
                undone.erase(std::remove(undone.begin(), undone.end(), list[j]-1), undone.end());
            }
        }
        count +=1;
    }

    // printout_list(list);
    // cout << "method2: " << list.size() << " times\n";
    res->list.assign(list.begin(), list.end());
    res->score = list.size();
} 

float totalV(float v){
    float total = 0;
    for(int i = 1; i <= v; i++){
        total += i;
    }
    return total;
}

// 一次送一個到終點
void method3(float *dis, int n, float V, opt *res){
    int time = 0;
    float curr_dis = 0;
    vector<int> list;
    for(int i = 0; i < n; i++){
        // 沒有加速度ver.
        // time += ceil(dis[i] / V);
        // for(int j = 0; j < ceil(dis[i] / V); j++){
        //     list.push_back(i+1);
        // }
        // 有加速度ver.
        curr_dis = dis[i];
        while(curr_dis > 0){
            list.push_back(i + 1);
            curr_dis -= V;
            time += 1;
            if(curr_dis <= totalV(V-1)){
                for(int j = V-1; j > 0; j--){
                    if(curr_dis > 0){
                        curr_dis -= j;
                        if(i == n-1){
                            list.push_back(0);
                            time += 1;
                        }
                    }else{
                        break;
                    }
                }
            }
        }
    }

    // printout_list(list);
    // cout << "method3: " << time << " times\n";
    res->list.assign(list.begin(), list.end());
    res->score = time;
} 

void calculateDis(float *dis, int n, pair <int, int> *node, pair <int, int> target){
    for(int i = 0; i < n; i++){
        dis[i] = sqrt(pow(target.first - node[i].first,2) + pow(target.second - node[i].second,2));
    }
}

float generator(int limit, int min,int typ = 0){
    random_device rd;
    mt19937 generator(rd());
    if(typ == 0){
        uniform_int_distribution<int> unif(min, limit);
        return unif(generator);
    }else{
        uniform_real_distribution<float> unif(min, limit);
        return unif(generator);
    }
}

int main(int argc, char *argv[]){
    int samples_n = atoi(argv[1]);
    struct opt *results = new opt[3*samples_n];
    int n = 3;
    float V = 3;
    float dis[n];
    pair <int,int> target(0,0);
    // dis[0] = 4;
    // dis[1] = 6;
    // dis[2] = 8;
    pair <int,int> nodeList[n];
    for (int j = 0; j < samples_n; j++){
        for(int i = 0; i < n; i++){
            nodeList[i].first = generator(100,0);
            nodeList[i].second = generator(100,0);
            while(nodeList[i].first ==  target.first && nodeList[i].second == target.second){
                nodeList[i].first = generator(100,0);
                nodeList[i].second = generator(100,0);
            }
        }
        calculateDis(dis, n, nodeList, target);
        method1(dis, n, V, &results[j]);
        method2(dis, n, V, &results[j+samples_n]);
        method3(dis, n, V, &results[j+2*samples_n]);
    }
    // #methods
    float total[3];
    for(int i=0; i < samples_n; i++) {
        // 算平均
        total[0] += results[i].score;
        total[1] += results[i + samples_n].score;
        total[2] += results[i + 2*samples_n].score;
    }
    cout << "Method1: " << total[0] / samples_n << endl;
    cout << "Method2: " << total[1] / samples_n << endl;
    cout << "Method3: " << total[2] / samples_n << endl;
}