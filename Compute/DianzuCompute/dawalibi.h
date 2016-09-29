#ifndef DAWALIBI_H
#define DAWALIBI_H

#include "jifenbase.h"

class Dawalibi : public JiFenBase
{
public:
    Dawalibi(Conductor c);

    double Vp(Point P);     //求解导体在P点的电位

    double huzu(Conductor c2, int n);      //求解互阻(全空间)
    double zizu(int n);                         //求解自阻(全空间)

private:
    double Vp(double x,double y);   //求解导体在相对坐标下的电位
};

#endif // DAWALIBI_H
