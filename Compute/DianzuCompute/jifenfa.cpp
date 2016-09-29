#include "jifenfa.h"
#include "Base/conductor.h"
#include "Base/point.h"
#include "Base/compute.h"
#include "math.h"

#include <QtDebug>

const double pi = 3.1415926;


JifenFa::JifenFa(Conductor c)
    :JiFenBase(c)
{
}

//求解导体在P点的电位
//n为积分点数
double JifenFa::Vp(Point P,int n)
{
    trans(P);   //坐标变换
    return rect(static_cast<pFun>(&JifenFa::F1),0,l,n);    //调用距形公式计算积分
//    return ladder(static_cast<pFun>(&JifenFa::F1),0,l,n);    //调用梯形公式计算积分
//    return asr(static_cast<pFun>(&JifenFa::F1),0,l,0.000001);    //调用辛普生公式计算积分

//    Point A(c1.x0,c1.y0,c1.z0);
//    Point B(c1.x1,c1.y1,c1.z1);

//    double sum = 0;

//    for(int i=0;i<n;i++){
//        Point S = Point::ChaZhi(A,B,i,n);
//        sum += 1/Point::length(S,P);
//    }
//    return sum/n/4/pi;
}

//坐标转换后，P点的电位（为了提高计算效率，实际除以了l*4*pi）
double JifenFa::Vp(double x, double y, int n)
{
    px = x;
    py = y;
    return ladder(static_cast<pFun>(&JifenFa::F1),0,l,n)/4/pi;    //调用梯形公式计算积分
}

double JifenFa::huzu(Conductor c2,int n)
{
    Point E(c2.x0,c2.y0,c2.z0);
    Point F(c2.x1,c2.y1,c2.z1);

    double sum = 0;

    for(int i=0;i<n;i++){
        Point S = Point::ChaZhi(E,F,i,n);
        sum += Vp(S,n);
    }
    return sum/n/4/pi;
}

double JifenFa::zizu(int n)
{
    return huzu(Compute::Pingyi(c1,c1.r),n);
}

double JifenFa::F1(double x)
{
    return 1/sqrt((pow((px-x),2)+py*py));
}


