#ifndef JIFENFA_H
#define JIFENFA_H

#include "jifenbase.h"

class JifenFa : public JiFenBase
{
public:
    JifenFa(Conductor c);

    double Vp(Point P, int n);     //求解导体在P点的电位    
    double huzu(Conductor c2, int n);      //求解互阻(全空间)
    double zizu(int n);

private:
    double F1(double x); //定义被积函数1

    double Vp(double x,double y, int n);     //求解导体在P点的电位

};

#endif // JIFENFA_H
