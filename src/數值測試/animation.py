# usage:  'python animation.py -n <#sheeps> -i <index> -d <#drones> -m <method> -s <showType>'
# n:羊隻數量
# i:case index
# d:無人機個數
# m:使用方法(m3, S1, S2, S3)
# s:要單純顯示還是輸出mp4(show, mp4)


import numpy as np
import matplotlib.pyplot as plt
import matplotlib.animation as animation
import time
import pandas as pd
import math
import sys, getopt

def distance(x, y):
    return math.sqrt(pow(float(x[0]) - float(y[0]), 2) + pow(float(x[1]) - float(y[1]),2)) 

# 讀取中繼點時間資訊
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
#讀取線段顯示資訊
def readUndone(filename, n):
    mid_t = {}
    with open(filename, 'r',encoding="utf-8") as f:
        line = f.readline()
        while(line):
            text = line.split('\t', n)
            mid_t[int(text[0])] = text[1:len(text)-1]
            line = f.readline()
    return mid_t

# 讀取movement資料
def readfile(filename, n, m):
    with open(filename, 'r',encoding="utf-8") as f:
        line = f.readline()
        curr_time = 0
        path = {}
        sta = {}
        while line:
            if line != '':
                path[curr_time] = {}
                text = line.split('\t', n+m+1)
                for i in range(n):
                    posi = text[i].split(' ', 3)
                    path[curr_time][int(posi[0])] = [posi[1], posi[2]]
                # print(text[len(text)-1])
                for j in range(n, n+m):
                    player = text[j].split(' ', 3)
                    path[curr_time][j] = [player[0], player[1]]
                    sta[curr_time][j] = player[2]
                curr_time += 1
            line = f.readline()
    return path,sta

n = 0
m = 0
f_ind = 0
typ = ''
showTyp = ''
group = 0
try:
    opts, args = getopt.getopt(sys.argv[1:],"hn:i:d:m:s:g:",["sheep=", "index=", "drone=","method=","showType=", "group"])
except getopt.GetoptError:
    print ('animation.py -n <#sheeps> -i <index> -d <#drones> -m <method> -s <showType>')
    sys.exit(2)
for opt, arg in opts:
    if opt == '-h':
        print ('animation.py -n <#sheeps> -i <index> -d <#drones> -m <method> -s <showType>')
        sys.exit()
    elif opt in ("-n", "--sheep"):
        n = arg
    elif opt in ("-i", "--index"):
        f_ind = arg
    elif opt in ("-d", "--drone"):
        m = arg
    elif opt in ("-m", "--method"):
        typ = arg
    elif opt in ("-s", "--showType"):
        showTyp = arg
    elif opt in ("-g", "--group"):
        group = arg

m = int(m)
n = int(n)

g = {}
m_p = {}
label = []
node_path = {}
mid_t = {}
key = {}
if typ != 'm3':
    midFilename = ''
    if typ == 'S1':
        if group != 0:
            midFilename = 'input/newS1_results_100_123/input_point_'+str(n)+'_'+str(f_ind)+'_'+str(group)+'.txt'
        else:
            midFilename = 'input/newS1_results_100_123/input_point_'+str(n)+'_'+str(f_ind)+'.txt'
    else:
        if group != 0:
            midFilename = 'input/S2_results_100/S2_input_point_'+str(n)+'_'+str(f_ind)+'_'+str(group)+'.txt'
        else:
            midFilename = 'input/S2_results_100/S2_input_point_'+str(n)+'_'+str(f_ind)+'.txt'
    undoneFilename = 'midP/'+typ+'_case_'+str(n)+'_'+str(f_ind)+'_'+str(m)
    if group != 0:
        undoneFilename += '_' + str(group)  
    if (m >= 2):
        undoneFilename += '_Mu'
    undoneFilename += '.txt'
    g, m_p, n, label, node_path = readMid(midFilename, typ)
    mid_t = readUndone(undoneFilename, n+len(g)+len(m_p)+1)
    for i in range(len(label)):
        key[i] = int(label[i])
        

moveFilename = "movement/"+typ+"_case_"+str(n)+"_" + str(f_ind) + "_"+str(m)
if group != 0:
    moveFilename += '_'+str(group)
if m >= 2:
    moveFilename += '_Mu'
moveFilename += '_Mu_test'
    
moveFilename += '.txt'
    
optFilename = typ + '_case_' + str(n) + '_' + str(f_ind) + '_' + str(m) + '_' + str(group) + '.mp4'
path, sta = readfile(moveFilename, n, m)
colmap = {}
if typ == 'S2' or typ == 'S3':
    col_size = len(g)+1
else:
    col_size = len(path[0]) 
for i in range(col_size):
    colmap[i] = list(np.random.choice(range(100,256), size=3))
    for j in range(3):
        colmap[i][j] /= 255



fig = plt.figure()
ax = plt.axes(xlim=(0, 30), ylim=(0, 30))
timetext = ax.set_title('')
drone_texts = []
for i in range(m):
    text  = ax.text(0,0,'drone ' + str(i+1))
    drone_texts.append(text)

scats = []
lines = []
node_p = []
c_other = [211/255,211/255,211/255]
c_curr = [230/255,0/255,0/255]
c = list()
# 中繼點
mid = []
# print(m_p)
for i in range(len(m_p) + len(g)):
    mark = '*'
    if typ == 'S2' or typ == 'S3':
        if i >= len(g):
            mark = 'x'
            # c = colmap[int(label[i])]
        # else:
            # c = colmap[i]
    # else:
        # c = colmap[i]
    scat = ax.scatter([],[], color = 'black', marker = mark)
    mid.append(scat)
# 中繼點連線
# pre_group = {}
for index,lay in enumerate(node_path):
    for x in range(len(node_path[index])-1):
        v1 = node_path[index][x]
        v2 = node_path[index][x+1]
        if (v1,v2) not in node_p:
            node_p.append((int(v1),int(v2)))
for i in range(len(node_p)):
    lobj = ax.plot([],[],lw=2,color = c_other,linestyle='-.')[0]
    lines.append(lobj)
# print(node_path)
# print(node_p)
# print(m_p)
# 羊隻、無人機
for i in range(len(path[0])):
    if typ == 'S2' or typ == 'S3':
        if i < len(path[0]) -m:
            c  = colmap[int(label[i])]
        else:
            c = colmap[col_size-1]
    else:
        c = colmap[i]
    if i >= n:
        scat = ax.scatter([],[], color = c, marker = '1')
    else:
        scat = ax.scatter([],[], color = c)
    scats.append(scat)

def init():
    for line in lines:
        line.set_data([],[])
    # for scat in scats:
    #     scat.set_offsets([])
    return tuple(lines) + tuple( scats) + tuple(mid) + tuple(drone_texts) + (timetext,)

# x = {0:[1, 20, 50, 60],1:[3, 25, 60 , 55]}
# y = {0:[5, 30, 20 ,50],1:[10, 25, 40, 60]}
curr_time = [0]
curr_node = []
length = 0
if typ == 'S1':
    length = len(m_p) + n + 1
if typ == 'S2' or typ == 'S3':
    length = len(g) + len(m_p)+1
undone = []
for i in range(length):
    undone.append(i)
total_path = [0]
# print(path)
prev_time = [0]

# timest = mid_t.keys()
# for i in range(1,len(timest)):
    


def animate(i, undone, curr_time, prev_time, curr_node, total_path, n, m):
    timetext.set_text('Time: ' + str(i) + ', drone path:' + str(total_path[0]))
    # x = numpy.array(range(1,npdata.shape[0]+1))
    if i == len(path):
        undone.clear()
    if typ == 'S1' or typ == 'S2' or typ == 'S3':
        if i in mid_t:
            prev_time[0] = curr_time[0]
            curr_time[0] = i
            curr_node.clear()
            for node in mid_t[i]:
                curr_node.append(int(node))
            # if(i > 0):
            #     for node in mid_t[prev_time[0]]:
            #         if node not in mid_t[i] and int(node) in undone:
            #             undone.remove(int(node))
        elif i == len(path):
            curr_node = [-1]
        # print(undone)
        # print(curr_node)
        # print(str(prev_time[0])+ ' '+str(curr_time[0]))
        #中繼點
        for lnum, node in enumerate(mid):
            if lnum in undone:
                if typ == 'S2' or typ == 'S3':
                    if lnum >= len(g):
                        x,y = m_p[lnum]
                    else:
                        x,y = g[lnum]
                else:
                    x,y = m_p[lnum+n]
                node.set_offsets((x,y))
            else:
                c = [0,0,0,0]
                node.set_color(c)
        #中繼點連線 
        for lnum,line in enumerate(lines):
            (v1,v2) = node_p[lnum]
            co = list()
            ls = ''
            x = y = []
            if typ == 'S1':
                # print(str(v1) + ' ' + str(v2))
                if (v1 in undone or v1 in curr_node)  and v1 >= n and (v2 >= n or v2 == -1):
                                    # 畫圖
                    if v2 == -1:
                        x, y = zip(m_p[v1], [0,0])
                    else:
                        x, y = zip(m_p[v1], m_p[v2])
                    if v1 in curr_node:
                        co = c_curr
                        ls = '-'
                    else:
                        co = c_other
                        ls = '-.'
                else:
                    co = [0,0,0,0]
                line.set_data(x,y)
                line.set_color(co)
                line.set_linestyle(ls)
            elif typ == 'S2' or typ == 'S3':
                if (v2 in undone or v2 in curr_node or v2 == -1) :
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
                    if v2 in curr_node:
                        co = c_curr
                        ls = '-'
                    else:
                        co = c_other
                        ls = '-.'
                else:
                    co = [0,0,0,0]
                line.set_data(x,y)
                line.set_color(co)
                line.set_linestyle(ls)
    x = i
    if i == len(path):
        x = i-1
    if i > 0:
        for k in range(n, len(path[x])):
            if not math.isnan(float(path[x][k][0])) and not math.isnan(float(path[x][k][1])) and not math.isnan(float(path[x-1][k][0])) and not math.isnan(float(path[x-1][k][1])):
                total_path[0] += math.floor(distance(path[x][k],path[x-1][k])*100)/100
    for lnum,scat in enumerate(scats):
        scat.set_offsets((path[x][lnum],path[x][lnum]))
    for lnum, texts in enumerate(drone_texts):
    #     # print(path[x][lnum+n])
        texts.set_position((float(path[x][lnum+n][0]), float(path[x][lnum+n][1])))
    return tuple(lines)+ tuple(scats) + tuple(mid)+ tuple(drone_texts)+  (timetext,) 

anim = animation.FuncAnimation(fig, animate, init_func=init,
                               frames=len(path)+1,  fargs=(undone, curr_time, prev_time, curr_node, total_path, n,m),interval=1, blit=True)
if showTyp == 'show':
    plt.show()
else:
    writergif = animation.FFMpegWriter(fps=60) 
    anim.save(optFilename, writer=writergif)


