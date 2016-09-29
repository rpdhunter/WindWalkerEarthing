#include "speedup.h"
#include "Base/point.h"
#include <QtDebug>
#include <QPointF>
#include "math.h"

SpeedUp::SpeedUp()
{
}

double SpeedUp::lookup(Conductor c1, Conductor c2)
{
    trans(c1,c2);
    ConductorPair pair(c1,c2);
    return huzulist.value(pair,-1);     //如果没有，返回-1
}

double SpeedUp::lookup(Conductor c1)
{
    trans(c1);
    ConductorPair pair(c1);
    return zizulist.value(pair,-1);     //如果没有，返回-1
}

void SpeedUp::addR(Conductor c1, Conductor c2, double R)
{
    trans(c1,c2);
    ConductorPair pair(c1,c2);
    huzulist[pair] = R;
}

void SpeedUp::addR(Conductor c1, double R)
{
    trans(c1);
    ConductorPair pair(c1);
    zizulist[pair] = R;
}


//用于显示加速程序效果的函数
//参数n是系数矩阵的阶数
void SpeedUp::show(int n)
{
    double a = this->zizulist.count();
    double b = this->huzulist.count();
    qDebug()<<QObject::tr("使用加速算法，只计算自电阻%1段，提速%2%     只计算互电阻%3段，提速%4%")
              .arg(a).arg(100*(n-a)/n).arg(b).arg(100*(n*(n-1)-b)/n/(n-1));

    //    qDebug()<<"zizu";
    //    foreach(ConductorPair a,zizulist.keys()){
    //        qDebug()<<a.c1.x0<<a.c1.y0<<a.c1.z0<<" "<<a.c1.x1<<a.c1.y1<<a.c1.z1<<" "
    //               <<a.c1.r<<"    "<<zizulist.value(a);
    //    }

    //    qDebug()<<"huzu";
    //    foreach(ConductorPair a,huzulist.keys()){
    //        qDebug()<<a.c1.x0<<a.c1.y0<<a.c1.z0<<" "<<a.c1.x1<<a.c1.y1<<a.c1.z1<<" "
    //               <<a.c2.x0<<a.c2.y0<<a.c2.z0<<" "<<a.c2.x1<<a.c2.y1<<a.c2.z1<<"    "<<huzulist.value(a);
    //    }
}


//变换规则：
//1.以c1为标准，z轴不动，c1的中点为坐标原点，y轴坐标为0
//2.变换二步达成，第一步平移，第二步旋转
//3.之后再对c2做对称变换，目的是使c2的一个端点位于第一象限（目的是最大限度减少计算次数）
void SpeedUp::trans(Conductor &c1, Conductor &c2)
{
    if(c1<c2){
        Conductor temp = c2;
        c2 = c1;
        c1 = temp;
    }

    double dx = (c1.x0 +c1.x1)/2;
    double dy = (c1.y0 +c1.y1)/2;
    //    double l = c1.length(); //l不能为0

    double sin_a,cos_a;
    double l = sqrt(pow(c1.x0-c1.x1,2) + pow(c1.y0-c1.y1,2));

    if(l==0){   //说明c1是垂直地表的
        Point point = c2.centrePoint();
        double l1 = sqrt(pow(point.x-c1.x1,2) + pow(point.y-c1.y1,2));
        if(l1==0){
            sin_a = 0;
            cos_a = 1;     //此种情况说明C1,c2中点在同一直线，且垂直地面，此时不需要旋转变换
        }
        else{
            sin_a = (point.y - c1.y0)/l1;
            cos_a = (point.x - c1.x0)/l1;
        }
    }
    else{
        sin_a = (c1.y1 - c1.y0)/l;
        cos_a = (c1.x1 - c1.x0)/l;
    }

    //作平移和旋转变换
    QPointF P1 = pointTrans(QPointF(c1.x0,c1.y0),dx,dy,sin_a,cos_a);
    QPointF Q1 = pointTrans(QPointF(c1.x1,c1.y1),dx,dy,sin_a,cos_a);
    QPointF P2 = pointTrans(QPointF(c2.x0,c2.y0),dx,dy,sin_a,cos_a);
    QPointF Q2 = pointTrans(QPointF(c2.x1,c2.y1),dx,dy,sin_a,cos_a);

    c1.x0 = P1.x();
    c1.y0 = P1.y();
    c1.x1 = Q1.x();
    c1.y1 = Q1.y();
    c2.x0 = P2.x();
    c2.y0 = P2.y();
    c2.x1 = Q2.x();
    c2.y1 = Q2.y();

    //c2做对称变换
    c2Trans(c2);
}

void SpeedUp::trans(Conductor &c1)
{
    double dx = (c1.x0 +c1.x1)/2;
    double dy = (c1.y0 +c1.y1)/2;

    double sin_a,cos_a;
    double l = sqrt(pow(c1.x0-c1.x1,2) + pow(c1.y0-c1.y1,2));

    if(l==0){
        sin_a = 0;
        cos_a = 1;
    }
    else{
        sin_a = (c1.y1 - c1.y0)/l;
        cos_a = (c1.x1 - c1.x0)/l;
    }


    QPointF P1 = pointTrans(QPointF(c1.x0,c1.y0),dx,dy,sin_a,cos_a);
    QPointF Q1 = pointTrans(QPointF(c1.x1,c1.y1),dx,dy,sin_a,cos_a);

    c1.x0 = P1.x();
    c1.y0 = P1.y();
    c1.x1 = Q1.x();
    c1.y1 = Q1.y();
}

QPointF SpeedUp::pointTrans(QPointF P0, double dx, double dy, double sin, double cos)
{
    double x = (P0.x() - dx)*cos + (P0.y() - dy)*sin ;
    double y = -(P0.x() - dx)*sin + (P0.y() - dy)*cos ;
    return QPointF(x,y);
}


//对称变换，使得c2离原点近的端点在第一象限
void SpeedUp::c2Trans(Conductor &c2)
{
    double l1 = c2.x0*c2.x0 + c2.y0*c2.y0;
    double l2 = c2.x1*c2.x1 + c2.y1*c2.y1;

    //交换线段端点，使得离原点近的端点在前
    if(l1>l2){
        double x=c2.x0;
        double y=c2.y0;
        double z=c2.z0;

        c2.x0 = c2.x1;
        c2.y0 = c2.y1;
        c2.z0 = c2.z1;
        c2.x1 = x;
        c2.y1 = y;
        c2.z1 = z;
    }

    //镜像变换，使得前一点在第一象限
    if(c2.x0<0){
        c2Trans_y(c2);
    }

    if(c2.y0<0){
        c2Trans_x(c2);
    }
    else if(c2.y0==0){
        if(c2.y1<0){
            c2Trans_x(c2);
        }
    }
}

void SpeedUp::c2Trans_x(Conductor &c2)
{
    c2.y0 = -c2.y0;
    c2.y1 = -c2.y1;
}

void SpeedUp::c2Trans_y(Conductor &c2)
{
    c2.x0 = -c2.x0;
    c2.x1 = -c2.x1;
}


