#include <iostream>
#include <cmath>
#include <vector>
#include <algorithm>
#include <utility>
#include <random>
#include <fstream>
using namespace std;


void readfile(bool *read, string filename, int *N, int *M,　int *C,  pair <float, float> *target, pair <float, float> *&posi, vector<int> *&paths, float *&dis){
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
        switch(c){
            // ignore commemts
            case 'c':
                getline(input, line);
                break;
            case 'd':
            {
                input >> index >> x >> y >> dist;
                posi[index] = pair<float, float>(x,y);
                dis[index] = dist;
                getline(input, line);
                break;
            }
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
            case 's':
                input >> x >> y;
				*target = pair<float, float>(x,y);
                // cout << target->first << " " << target->second << endl;
                getline(input, line);
                break;
            case 'n':
                input >> *N;
                // cout << *N << endl;
				posi = new pair<float, float>[*N];
                dis = new float[*N];
                getline(input, line);
                break;
            case 'm':
                input >> *M;
                // cout << *M << endl;
				paths = new vector<int>[*M];
                getline(input, line);
                break;
            case 'c':
                input >> *C;
                getline(input, line);
                break;
            case 'e':
                flag = false;
                break;
        }
    }
}


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

int farest(float *dis, int n ,vector <int> undone = vector <int>()){
    int max = -1;
    int ind = -1;
    for(int i=0; i<n; i++){
        // cout << dis[i] << endl;
        if(undone.empty()){
            if(dis[i] > max && dis[i] > 0){
                max = dis[i];
                ind = i;
            }
        }else{
            // bool test = find(undone.begin(), undone.end(), i) != undone.end();
            // cout << i  << " " << dis[i] << endl;
            if(dis[i] > max && dis[i] > 0 && find(undone.begin(), undone.end(), i) != undone.end()){
                max = dis[i];
                ind = i;
            }

        }
    }
    return ind;
}

// 每次都挑最遠的
void method1(float *dis, int n, float V, opt *&res, int C){
    // copy dis array
    float dis_edit[n];
    int curr;
    vector <int> list[C];
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

int minimum(int * time, int C){
    float min = INT_MAX;
    int ind = -1;
    for(int i = 0; i < C; i++){
        if(min > time[i]){
            min = time[i];
            ind = i;
        }
    }
    return ind;
}
// Branch ver
void method1_B(float *dis, int n, int m,  float V, opt *&res,vector <int> *data, int C){
    // copy dis array
    float dis_edit[n];
    int curr;
    vector <int> list[C];
    vector <int> undone;
    vector <int> done;
    for(int i=0; i<n; i++){
        dis_edit[i] = dis[i];
        if(i < m){
            undone.push_back(i);
        }
    }
    int player;
    int player_t[C];
    bool flag;
    while(!undone.empty()){
        for (int i = 0; i < C; i++){
            curr = farest(dis_edit, n, undone);
            player = minimum(player_t, C, dis_edit);
            list[player].push_back(curr + 1);
        }
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
                done.push_back(list[j]-1);
            }
        }
        // update middle point
        
        for(int i = 0; i < n-m; i++) {
            if(find(undone.begin(), undone.end(), i+m) == undone.end() && find(done.begin(), done.end(), i+m) == done.end()){
                flag = false;
                for(auto it = begin(data[i]); it != end(data[i]); ++it){
                    if(find(undone.begin(), undone.end(), *it) != undone.end()){
                        // undone 裡還有
                        flag = true;
                    }
                }
                if(!flag){
                    undone.push_back(i+m);
                }
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

void calculateDis(float *dis, int n, pair <float, float> *node, pair <float, float> target){
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
    string filename = "input_point.txt";
    // filename = argv[1];
    // read file
    bool read = true;
    string problemType, line, title;
    int N, M, C;
    pair <float, float> target;
    pair <float, float> *nodeList;
	vector<int> *paths;
    float *dis;
    readfile(&read, filename, &N,&M, &C, &target, nodeList, paths, dis);


    int samples_n = 1;
    struct opt *results = new opt[C];
    int n = N;
    float V = 3;
    vector <int> data[N-M];
    for(int i = 0; i < M; i++){
        for(auto it = begin(paths[i])+1; it+1 != end(paths[i]); ++it){
            if(*it > M - 1 && find(data[*it-M].begin(),data[*it-M].end(), *(it-1)) == data[*it-M].end()){
                data[*it-M].push_back(*(it-1));
            }
        }
    }

    // original method
    float org_dis[n];
    // for (int j = 0; j < samples_n; j++){
    calculateDis(org_dis, M ,nodeList, target);
    method1(org_dis, M, V, &results, C);
    // }

    // branch ver.
    // method1_B(dis, N, M, V, &results[1], data);

    // cout << "Original version: " << results[0].score << " times" << endl;
    // cout << "Branch version: " << results[1].score << " times" << endl;
}