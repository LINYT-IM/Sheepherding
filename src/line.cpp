#include <iostream>
#include "line.h"
using namespace std;

Point rotate(Point org, Point center, float angle){
    Point r(cos(angle/180*M_PI), sin(angle/180*M_PI));
    Point rusult = (org - center)*r + center;
    return rusult;
}

// 點積運算
float dot(Point v1, Point v2)
{
    return v1.real() * v2.real() + v1.imag() * v2.imag();
}

float length(Point p)
{
    return sqrt(p.real() * p.real() + p.imag() * p.imag());
}

// 外積
float cross(Point v1, Point v2)
{
    return v1.real() * v2.imag() - v1.imag() * v2.real();
}

Point projectP(Point a, Line l){
    // l.p1位移至原點
    Point pa = a - l.p1;
    Point pq = l.p2 - l.p1;
    // a投影至直線(p,q)，得到b。
    Point pb = pq * dot(pa, pq) / norm(pq);
    Point b = l.p1 + pb;
    return b;
}

// 點到線段的距離
float distancePtoL(Point p, Line s)
{
    // 線段兩點不能為同一點
    if(s.p1 == s.p2){
        return -1;
    }
    // 建立向量求點積，以判斷點在哪一區。
    Point v (s.p2 - s.p1);
    Point v1 (p - s.p1);
    Point v2 (p - s.p2);
 
    // 端點距離區：點到點距離
    // 等於=可以省略
    if (dot(v, v1) <= 0) return length(v1);
    if (dot(v, v2) >= 0) return length(v2);
    // 垂直距離區：點到線距離
    return abs(cross(v, v1)) / length(v);
}