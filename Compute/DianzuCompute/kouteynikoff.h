#ifndef KOUTEYNIKOFF_H
#define KOUTEYNIKOFF_H

#include "jifenbase.h"

class Kouteynikoff : public JiFenBase
{
public:
    Kouteynikoff(Conductor c);

    double Vp(Point P);     //求解导体在P点的电位
    double huzu(Conductor c2, int n);      //求解互阻(全空间)
    double zizu(int n);      //求解互阻(全空间)

    void test();    //测试函数

private:
    double a,b,r,z;

    double F(double x); //定义被积函数1
    double Vp(double R, double Z);

    double first_modified_Bessel(int n,double x);    //第一类修正贝塞尔曲线
    double second_modified_Bessel(int n,double x);   //第二类修正贝塞尔曲线
};

#endif // KOUTEYNIKOFF_H
