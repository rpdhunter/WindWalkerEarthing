#ifndef JIFENBASE_H
#define JIFENBASE_H

#include "Base/conductor.h"

class JiFenBase
{
public:
    JiFenBase(Conductor c);    

    void trans(Point P);    //坐标变换

    //以下为积分公式
    typedef double (JiFenBase::*pFun)(double);     //声明一个函数指针

    double rect(pFun F, double a, double b, int n);        //矩形公式
    double ladder(pFun F, double a, double b, int n);        //梯形公式

    double asr(pFun F, double a, double b, double eps);        //自适应simpson公式主过程
    double asr(pFun F, double a, double b, double eps, double A);   //自适应simpson公式递归过程
    double simpson(double width,double fa,double fb,double fc);  //三点辛普森公式

protected:
    Conductor c1;
    double l;
    double r1;
    double r2;
    double px;      //变换后P点横坐标
    double py;      //变换后P点纵坐标

};

#endif // JIFENBASE_H
