#!/usr/bin/env python
# coding: utf-8

# In[2]:


import matplotlib.pyplot as plt
import numpy as np
import time
import pandas as pd
import matplotlib.animation
import math


# In[26]:

def distance(x, y):
    return math.sqrt(pow(float(x[0]) - float(y[0]), 2) + pow(float(x[1]) - float(y[1]),2)) 

def readMid(filename, typ):
    g = {}
    m_p = {}
    n = 0
    label = []
    node_path = {}
    with open(filename, 'r',encoding="utf-8") as f:
        line = f.readline()
        while(line):
            if line != '':
                if line[0] == 'n':
                    n = int(line.split('\t', 2)[1])
                elif line[0] == 'd':
                    text = line.split('\t',4)
                elif line[0] == 'c':
                    if typ == 'S2' or typ == 'S3':
                        text = line.split('\t',4)
                        g[int(text[1])] = [float(text[2]), float(text[3])]
                elif line[0] == 'g':
                    label = line.split('\t', n + 1)[1:n+1]
                elif line[0] == 'h':
                    if typ == 'S1':
                        text = line.split('\t',4)
                        m_p[int(text[1])] = [float(text[2]), float(text[3])]
                    if typ == 'S2' or typ == 'S3':
                        text = line.split('\t',4)
                        m_p[int(text[1])] = [float(text[2]), float(text[3])]
                elif line[0] == 'p':
                    text = line.split('\t',n+len(m_p)+len(g))
                    node_path[int(text[1])] = text[1:len(text)-1]
                    node_path[int(text[1])].append(-1)
            line = f.readline()
    return g, m_p, n, label, node_path

def readUndone(filename, n):
    mid_t = {}
    with open(filename, 'r',encoding="utf-8") as f:
        line = f.readline()
        while(line):
            text = line.split('\t', n)
            mid_t[int(text[0])] = text[1:len(text)-1]
            line = f.readline()
    return mid_t

# print(g)

# print(n)
# print(label)



def readfile(filename, n, m):
    with open(filename, 'r',encoding="utf-8") as f:
        line = f.readline()
        curr_time = 0
        path = {}
        while line:
            if line != '':
                path[curr_time] = {}
                text = line.split('\t', n+m)
                for i in range(len(text)-m-1):
                    posi = text[i].split(' ', 3)
                    # print(posi)
                    path[curr_time][posi[0]] = [posi[1], posi[2]]
                # print(text[len(text)-1])
                for j in range(n, len(text)-1):
                    player = text[j].split(' ', 2)
                    path[curr_time][j] = [player[0], player[1]]
                curr_time += 1
            line = f.readline()
    return path


# In[27]:

def path_show(node_path, typ, undone, curr_node, length_path, colmap, m_p = {}, g = {}):
    c_other = [211/255,211/255,211/255]
    c_curr = [230/255,0/255,0/255]
    for n_p in node_path:
        for m in range(len(node_path[n_p])-1):
            v1 = int(node_path[n_p][m])    
            v2 = int(node_path[n_p][m+1])
            if typ == 'S1':
                if (v1 in undone or v1 == curr_node)  and v1 > length_path-1 and (v2 > length_path-1 or v2 == -1):
                                # 畫圖
                    if v2 == -1:
                        x, y = zip(m_p[v1], [0,0])
                    else:
                        x, y = zip(m_p[v1], m_p[v2])
                    if v1 == curr_node:
                        plt.plot(x,y, color = c_curr,linestyle='-')
                    else:
                        plt.plot(x,y, color = c_other,linestyle='-.')
            elif typ == 'S2' or typ == 'S3':
                if (v1 in undone or v1 == curr_node) or v2 == -1:
                    o = []
                    d = []
                    if v1 > len(g)-1:
                        o = m_p[v1]
                    else:
                        o = g[v1]
                    if v2 > len(g) -1:
                        d  = m_p[v2]
                    elif v2 == -1:
                        d = [0,0]
                    else:
                        d = g[v2]
                    x, y = zip(o, d)
                    if v1 == curr_node:
                        plt.plot(x,y, color = c_curr,linestyle='-')
                    else:
                        plt.plot(x,y, color = c_other,linestyle='-.')
    if typ == 'S2' or typ == 'S3':
        for g_c in g:
            if g_c in undone:
                plt.scatter(g[g_c][0], g[g_c][1], color = colmap[g_c], marker = '*')
                        
    if typ == 'S1' or typ == 'S2' or typ == 'S3':
        for m in m_p:
            if m in undone:
                plt.scatter(m_p[m][0], m_p[m][1], color = 'black', marker = 'x')


def node_show(path, i, colmap, typ, n):
    plt.xlim([-1, 100])
    plt.ylim([-1, 100])
    for j in path[i]:
        if int(j) >= n:
            plt.scatter(float(path[i][j][0]), float(path[i][j][1]), color = colmap[len(colmap)-1], marker='1', s = 100)
        else:
            if typ == 'S2' or typ == 'S3':
                c = colmap[key[int(j)]]
            else:
                c = colmap[int(j)]
            plt.scatter(float(path[i][j][0]), float(path[i][j][1]), color = c)
    
    for k in range(n, len(path[i])):
        plt.text(float(path[i][k][0]), float(path[i][k][1]),'drone(' + str(k) + ')',  size = 8, weight = 'light', ha= 'left',va='bottom', wrap = False)
# print(path[0])

# In[32]:
def anim(path, typ, n, m_p = {}, g = {}, key= [], mid_t = {}, node_path = {}):
    
    colmap = {}
    if typ == 'S2' or typ == 'S3':
        col_size = len(g)+1
    else:
        col_size = len(path[0]) 
    for i in range(col_size):
        colmap[i] = list(np.random.choice(range(100,256), size=3))
        for j in range(3):
            colmap[i][j] /= 255
    plt.ion()
    plt.figure(figsize=(100,100))
    total_path = 0
    length = 0
    if typ == 'S1':
        length = len(m_p) + len(path[0])
    if typ == 'S2' or typ == 'S3':
        length = len(g) + len(m_p)+1
    undone = []
    for i in range(length):
        undone.append(i)
    curr_line = 0
    curr_node = -1
    for i in range(0,len(path)):
        if typ == 'S1' or typ == 'S2' or typ == 'S3':
            if i in mid_t:
                curr_line = i
                for node in mid_t[i]:
                    if int(node) in undone:
                        curr_node = int(node)
                        undone.remove(int(node))
        if i % 10 == 0:
            plt.clf()
            # 顯示中繼點跟線段
            path_show(node_path, typ, undone, curr_node, len(path[0]), colmap, m_p,g)

            # 顯示個羊隻當前位置
            node_show(path, i, colmap, typ, n)
            plt.title('time: ' + str(i)+ ', drone path: ' + str(total_path))
            plt.pause(0.001)
        if i > 0:
            for k in range(n, len(path[i])):
                total_path += math.floor(distance(path[i][k],path[i-1][k])*100)/100
    
    plt.clf()
    node_show(path, i-1, colmap, typ, n)
    plt.title('time: ' + str(i-1)+ ', drone path: ' + str(total_path))
    plt.waitforbuttonpress()

m = 2
n = 5
g = {}
m_p = {}
label = []
node_path = {}
# g, m_p, n, label, node_path = readMid('input/N_results_100/input_point_5_0.txt', 'S1')
# print(n)
mid_t = {}
# mid_t = readUndone('midP/nk_case_5_0.txt', n+len(g)+len(m_p)+1)
key = {}
for i in range(len(label)):
    key[i] = int(label[i])
# print(g)
# print(m_p)
# print(mid_t)

path = readfile("movement/case_5_0_Mu.txt", n, m)
# print(path)
anim(path, 'm3', n, m_p, g, key, mid_t, node_path)