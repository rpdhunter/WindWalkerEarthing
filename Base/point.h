#ifndef POINT_H
#define POINT_H

class Point
{
public:
    Point();
    Point(double x,double y,double z);


    double x,y,z;

    static double length(Point p1,Point p2);    //两点之间的距离
    static double _length(Point p1,Point p2,Point A,Point B);   //两点之间的距离（如果和AB方向相反则为负）

    static Point ChaZhi(Point p1, Point p2, int i, int n);   //点的线性插值

    static Point Shadow(Point A,Point E,Point F);   //返回点A在线段EF确定的直线上的投影坐标

//    static double distance(Point A,Point E,Point F);  //返回A点和线段EF的最短距离
    static bool isInline(Point A,Point E,Point F);  //返回A点和线段EF的最短距离

    bool operator == (const Point &other) const;
};

#endif // POINT_H
