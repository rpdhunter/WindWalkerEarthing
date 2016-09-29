#include "dawalibi.h"
#include "Base/conductor.h"
#include "Base/point.h"
#include "qmath.h"

#include <QtDebug>

const double pi = 3.1415926;

Dawalibi::Dawalibi(Conductor c)
    :JiFenBase(c)
{
}

double Dawalibi::Vp(Point P)
{
    trans(P);   //坐标变换
    double v;
    if(qAbs(r1-px)<0.0000000001){  //共线的节奏
        double a = r1>r2?r1:r2;
        v = qLn(a/(a-l));
    }
    else{
        v = qLn((l-px+r2)/(r1-px));
    }
    return v/4/pi/l;
}

double Dawalibi::Vp(double x, double y)
{
    px = x;
    py = y;
    r1 = sqrt(px*px+py*py);
    r2 = sqrt((l-px)*(l-px)+py*py);

    double v;
    if(qAbs(r1-px)<0.0000000001){  //共线的节奏
        double a = r1>r2?r1:r2;
        v = qLn(a/(a-l));
    }
    else{
        v = qLn((l-px+r2)/(r1-px));
    }
//    double v = qLn((l-px+r2)/(r1-px))/4/pi/l;
    if(v>1000)
        qDebug()<<"dawalibi error"<<v;
    return v/4/pi/l;
}

//计算互阻需要知道积分点数
double Dawalibi::huzu(Conductor c2, int n)
{
    Point E(c2.x0,c2.y0,c2.z0);
    Point F(c2.x1,c2.y1,c2.z1);

    double sum = 0;

    for(int i=0;i<n;i++){
        Point S = Point::ChaZhi(E,F,i,n);
        sum += Vp(S);
    }
    return sum/n;
}

double Dawalibi::zizu(int n)
{
    double sum = 0;
    double h = l/n;

    for(int i=0;i<n;i++){
        sum += Vp((i+0.5)*h,c1.r);
    }
    return sum/n;
}
