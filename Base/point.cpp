#include "point.h"
#include <QtCore/qmath.h>
#include "armadillo"
#include <QtDebug>

using namespace arma;
const double MINVALUE = 1e-7;

Point::Point()
{
    this->x = 0;
    this->y = 0;
    this->z = 0;
}

Point::Point(double x, double y, double z)
{
    this->x = x;
    this->y = y;
    this->z = z;
}

double Point::length(Point p1, Point p2)
{
    return qSqrt(qPow(p1.x-p2.x,2) + qPow(p1.y-p2.y,2) + qPow(p1.z-p2.z,2));
}

double Point::_length(Point p1, Point p2, Point A, Point B)
{
    double l1 = p2.x - p1.x;
    double m1 = p2.y - p1.y;
    double n1 = p2.z - p1.z;
    double l2 = B.x - A.x;
    double m2 = B.y - A.y;
    double n2 = B.z - A.z;

    if(l1*l2 + m1*m2 + n1*n2 > 0)
        return length(p1,p2);
    else
        return -length(p1,p2);

}

//插值函数，根据给定的i和n值，返回p1，p2两点之间的一个点
//此点距离p1点为i/n倍两点间的距离
//n=1,2,3...
//i=0,1,2...n-1
Point Point::ChaZhi(Point p1, Point p2, int i, int n)
{
    if(i<0 || n<1 || i>=n)
        qDebug()<<"ChaZhi() funtion error";

    double dx = p2.x - p1.x;
    double dy = p2.y - p1.y;
    double dz = p2.z - p1.z;

    Point p(p1.x + dx*(i+0.5)/n,p1.y + dy*(i+0.5)/n,p1.z + dz*(i+0.5)/n);

    return p;

}

Point Point::Shadow(Point A, Point E, Point F)
{
    double l = F.x - E.x;
    double m = F.y - E.y;
    double n = F.z - E.z;
    double d5 = l*l + m*m +n*n;
    double d6 = l*(A.x - E.x) + m*(A.y - E.y) +n*(A.z - E.z);
    return Point(E.x+l*d6/d5,E.y+m*d6/d5,E.z+n*d6/d5);
}

bool Point::isInline(Point A, Point E, Point F)
{
    double l1 = length(A,E);
    double l2 = length(A,F);
    double l = length(E,F);

    if(qAbs(l1+l2-l)<0.00001 ){
        if(qAbs(l1-l)<0.00001 || qAbs(l2-l)<0.00001){
            return false;
        }
        else{
            return true;
        }
    }
    return false;
}

bool Point::operator ==(const Point &other) const
{
    if(qAbs(other.x - this->x)<MINVALUE && qAbs(other.y - this->y)<MINVALUE && qAbs(other.z - this->z)<MINVALUE )
        return true;
    else
        return false;

}

//double Point::distance(Point A, Point E, Point F)
//{

//}
