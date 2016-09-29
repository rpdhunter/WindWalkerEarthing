#ifndef SPEEDUPCX_H
#define SPEEDUPCX_H

#include "speedup.h"

//基于复数的加速类

class SpeedUpCx : public SpeedUp
{
public:
    SpeedUpCx();

    Complex lookup_Z(Conductor c1,Conductor c2);
    Complex lookup_Z(Conductor c1);

    void addZ(Conductor c1,Conductor c2,Complex Z);
    void addZ(Conductor c1,Complex Z);

    void show_CX(int n);

private:
    QMap<ConductorPair,Complex> zizukanglist;    //自阻数据
    QMap<ConductorPair,Complex> huzukanglist;    //互阻数据
};

#endif // SPEEDUPCX_H
