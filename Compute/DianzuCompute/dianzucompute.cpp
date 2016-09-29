#include "dianzucompute.h"
#include "Base/compute.h"
#include "Base/conductor.h"
#include "Base/point.h"
#include "math.h"
#include "qmath.h"

#include "dawalibi.h"
#include "heppe.h"
#include "jifenfa.h"
#include "kouteynikoff.h"
#include "secoundsubdivision.h"

#include <QtDebug>

#include "armadillo"

using namespace arma;

const double pi = 3.1415926;        //圆周率
const double MINVALUE = 0.00000001;   //最小值（用于比较实数和零，如果小于这个数明则认为相等）


DianZuCompute::DianZuCompute()
{
}

///////////////////////////////////////////////////
////几何法
///////////////////////////////////////////////////

//几何法求解均匀介质中两导体的互阻系数
//其中rou为土壤电阻率
double DianZuCompute::HuZu_JiHe_Totle(Conductor c1, Conductor c2 ,double rou)
{
    //对一部分导体做必要的预处理
    Conductor_YuChuLi(c1,c2);

    Heppe a(c1);
    return rou*a.huzu(c2);
}

//生成互阻系数，使用几何法
double DianZuCompute::HuZu_JiHe_Half(Conductor c1, Conductor c2, double rou)
{
    double R = HuZu_JiHe_Totle(c1,c2,rou);
    double _R = HuZu_JiHe_Totle(c1,c2.image(),rou);

    return R + _R;
}

double DianZuCompute::ZiZu_JiHe_Totle(Conductor c1, double rou)
{
    Heppe a(c1);
    return rou*a.zizu();
}

double DianZuCompute::ZiZu_JiHe_Half(Conductor c1, double rou)
{
    double R = ZiZu_JiHe_Totle(c1,rou);
    double _R = HuZu_JiHe_Totle(c1,c1.image(),rou);

    return R + _R;
}

///////////////////////////////////////////////////
////积分法
///////////////////////////////////////////////////

//最原始的积分算法，效率低下，但是具有无与伦比的稳定性
//虽然稳定，但是仍旧要求两段导体除端点外不能相交，且不能重合
double DianZuCompute::HuZu_JiFen_Totle(Conductor c1, Conductor c2, double rou ,int n)
{
    //对一部分导体做必要的预处理
    Conductor_YuChuLi(c1,c2);

    JifenFa a(c1);
    return rou*a.huzu(c2,n);
}

//最原始的积分算法，效率低下，但是具有无与伦比的稳定性
//虽然稳定，但是仍旧要求两段导体除端点外不能相交，且不能重合
double DianZuCompute::HuZu_JiFen_Half(Conductor c1, Conductor c2, double rou,int n)
{
    double R = HuZu_JiFen_Totle(c1,c2,rou,n);
    double _R = HuZu_JiFen_Totle(c1,c2.image(),rou,n);

    return R + _R;
}

double DianZuCompute::ZiZu_JiFen_Totle(Conductor c1, double rou,int n)
{
    JifenFa a(c1);
    return rou*a.zizu(n);
}

double DianZuCompute::ZiZu_JiFen_Half(Conductor c1, double rou,int n)
{
    double R = ZiZu_JiFen_Totle(c1,rou,n);
    double _R = HuZu_JiFen_Totle(c1,c1.image(),rou,n);

    return R + _R;
}

///////////////////////////////////////////////////
////Dawalibi方法
///////////////////////////////////////////////////

double DianZuCompute::HuZu_Dawalibi_Totle(Conductor c1, Conductor c2, double rou, int n)
{
    //对一部分导体做必要的预处理
    Conductor_YuChuLi(c1,c2);

    Dawalibi a(c1);
    return rou*a.huzu(c2,n);
}

double DianZuCompute::HuZu_Dawalibi_Half(Conductor c1, Conductor c2, double rou, int n)
{
    double R = HuZu_Dawalibi_Totle(c1,c2,rou,n);
    double _R = HuZu_Dawalibi_Totle(c1,c2.image(),rou,n);

    return R + _R;
}

double DianZuCompute::ZiZu_Dawalibi_Totle(Conductor c1, double rou, int n)
{
    Dawalibi a(c1);
    return rou*a.zizu(n);
}

double DianZuCompute::ZiZu_Dawalibi_Half(Conductor c1, double rou, int n)
{
    double R = ZiZu_Dawalibi_Totle(c1,rou,n);
    double _R = HuZu_Dawalibi_Totle(c1,c1.image(),rou,n);

    return R + _R;
}

///////////////////////////////////////////////////
////Kouteynikoff法
///////////////////////////////////////////////////

double DianZuCompute::HuZu_Kouteynikoff_Totle(Conductor c1, Conductor c2, double rou, int n)
{
    Kouteynikoff a(c1);
    return rou*a.huzu(c2,n);
}

double DianZuCompute::HuZu_Kouteynikoff_Half(Conductor c1, Conductor c2, double rou, int n)
{
    //这里先不做预处理，以后还需讨论
    double R = HuZu_Kouteynikoff_Totle(c1,c2,rou,n);
    double _R = HuZu_Kouteynikoff_Totle(c1,c2.image(),rou,n);

    return R + _R;
}

double DianZuCompute::ZiZu_Kouteynikoff_Totle(Conductor c1, double rou, int n)
{
    Kouteynikoff a(c1);
    return rou*a.zizu(n);
}

double DianZuCompute::ZiZu_Kouteynikoff_Half(Conductor c1, double rou, int n)
{
    double R = ZiZu_Kouteynikoff_Totle(c1,rou,n);
    double _R = HuZu_Kouteynikoff_Totle(c1,c1.image(),rou,n);

    return R + _R;
}

///////////////////////////////////////////////////
////二次分域法
///////////////////////////////////////////////////
//二次分域法目前只能计算自阻（互阻或许原生不支持）
//double DianZuCompute::HuZu_SecSubdivision_Totle(Conductor c1, Conductor c2, double rou, int n)
//{

//}

//double DianZuCompute::HuZu_SecSubdivision_Half(Conductor c1, Conductor c2, double rou, int n)
//{
//}

//double DianZuCompute::ZiZu_SecSubdivision_Totle(Conductor c1, double rou, int n)
//{
//}

double DianZuCompute::ZiZu_SecSubdivision_Half(Conductor c1, double rou, int n)
{
    SecoundSubdivision a(c1,n);
    return rou*a.zizu();
}

///////////////////////////////////////////////////
////其他方法
///////////////////////////////////////////////////

//生成两段导体间的互阻系数，使用积分法
//采用了何金良《电力系统接地技术》P202中所述的方法
//如果导体距离远，则看成点计算，近则选择目标导体（c2）上三个点，分别计算出系数再平均
double DianZuCompute::HuZu_JiFen_Half_V3(Conductor c1, Conductor c2, double rou)
{
    Conductor _c1 = c1.image();   //镜像
    Point O(c1.centrePoint());
    Point _O(_c1.centrePoint());
    Point P(c2.centrePoint());
    double r = Point::length(O,P);
    double _r = Point::length(_O,P);


    double l = Compute::space(c1,c2);
    if(l>10*c1.length() && l>c2.length()){
        double G = rou * (1/r + 1/_r) / 4 / pi;
        return G;
    }
    else{
        Point P1(c2.x0,c2.y0,c2.z0);
        Point P2(c2.x1,c2.y1,c2.z1);
        double r1 = Point::length(O,P1);
        double _r1 = Point::length(_O,P1);
        double r2 = Point::length(O,P2);
        double _r2 = Point::length(_O,P2);

        double G,G1,G2;
        if(r<=0.01 || _r<=0.01){
            G = 0;
        }else{
            G = rou * (1/r + 1/_r) / 4 / pi;
        }
        if(r1<=0.01 || _r1<=0.01){
            G1 = 0;
        }else{
            G1 = rou * (1/r1 + 1/_r1) / 4 / pi;
        }
        if(r2<=0.01 || _r2<=0.01){
            G2 = 0;
        }else{
            G2 = rou * (1/r2 + 1/_r2) / 4 / pi;
        }
        return (G+G1+G2)/3;
    }
}








//求互阻的积分法（使用近距离算法）
//n为积分点数
double DianZuCompute::HuZu_JiFen_Totle_v2(Conductor c1, Conductor c2, double rou, int n)
{
    Point A(c1.x0,c1.y0,c1.z0);
    Point B(c1.x1,c1.y1,c1.z1);

    Point E(c2.x0,c2.y0,c2.z0);
    Point F(c2.x1,c2.y1,c2.z1);

    double L1 = Point::length(A,B); //导体L1长度
    double L2 = Point::length(E,F); //导体L1长度

    double sum = 0;

    for(int i=0;i<n;i++){
        Point S = Point::ChaZhi(E,F,i,n);
        sum += Segment_Point_Potential_v2(c1,S,rou);
    }

    double R = sum / n ;

    return R;
}

double DianZuCompute::Segment_Point_Potential_v2(Conductor c1, Point P, double rou)
{
    double a = c1.r;    //导体半径
    double b = c1.length(); //导体长度
    Point _P = Point::Shadow(P,Point(c1.x0,c1.y0,c1.z0),Point(c1.x1,c1.y1,c1.z1));
    Point O((c1.x0+c1.x1)/2,(c1.y0+c1.y1)/2,(c1.z0+c1.z1)/2);
    double r = Point::length(_P,P);
    double z = Point::length(_P,O);

    double sum = 0;
    double d = 0;
    double di = 0.1;    //设定积分步长
//    for(int i=1;;i++){
//        d = second_modified_Bessel(0,di*i*r) * cos(z*di*i) * sin(b*di*i/2) * di
//                / pow(di*i,2) /  second_modified_Bessel(1,di*i*a);
//        //        qDebug()<<"d="<<d;
//        sum += d;
//        if(abs(d)<MINVALUE){
//            break;
//        }
//    }

//    qDebug()<<a<<b<<r<<z;
//    sum = asr(0,1,0.001,a,b,r,z);

//    qDebug()<<"resule is:"<<sum;

    sum = sum * rou / (pi*pi*a*b);
    return sum;

}

//预处理导体，用于计算近距离导体互阻之前
//算法：
//1.缩短较长导体（因为两导体长度接近时，计算精度高）
//2.长导体靠近短导体的一端缩减到空出半径的距离
void DianZuCompute::Conductor_YuChuLi(Conductor &c1, Conductor &c2)
{
//    if(Compute::space(c1,c2) < qMax(c1.r,c2.r)){
//        double l1 = c1.length();
//        double l2 = c2.length();

//        if(l1 > l2){
//            c1 = Compute::Suoduan(c1,c2.r);
//        }
//        else{
//            c2 = Compute::Suoduan(c2,c1.r);
//        }
//    }
    if(Compute::relationship(c1,c2) == Compute::collinear || Compute::space(c1,c2)<=MINVALUE){

        if(Compute::space(c1,c2)<=MINVALUE){
            double a=0;
            double b=0;
        }

        c2 = Compute::Pingyi(c1,c2,c2.r);
    }

}








