#ifndef LINE_H
#define LINE_H
#include <complex>
using Point = std::complex<float>;
struct Line {Point p1, p2;};
Point rotate(Point org, Point center, float angle);
// 線段(用於表示無人機路徑)


// 點積運算
float dot(Point v1, Point v2);

float length(Point p);

// 外積
float cross(Point v1, Point v2);

Point projectP(Point a, Line l);

// 點到線段的距離
float distancePtoL(Point p, Line s);
#endif