#include "speedupcx.h"
#include <QtDebug>

SpeedUpCx::SpeedUpCx()
{
}

Complex SpeedUpCx::lookup_Z(Conductor c1, Conductor c2)
{
    trans(c1,c2);
    ConductorPair pair(c1,c2);
    return huzukanglist.value(pair,Complex(-1,0));     //如果没有，返回-1
}

Complex SpeedUpCx::lookup_Z(Conductor c1)
{
    trans(c1);
    ConductorPair pair(c1);
    return zizukanglist.value(pair,Complex(-1,0));     //如果没有，返回-1
}

void SpeedUpCx::addZ(Conductor c1, Conductor c2, Complex Z)
{
    trans(c1,c2);
    ConductorPair pair(c1,c2);
    huzukanglist[pair] = Z;
}

void SpeedUpCx::addZ(Conductor c1, Complex Z)
{
    trans(c1);
    ConductorPair pair(c1);
    zizukanglist[pair] = Z;
}

void SpeedUpCx::show_CX(int n)
{
    double a = this->zizukanglist.count();
    double b = this->huzukanglist.count();
    qDebug()<<QObject::tr("使用加速算法，只计算自阻抗%1段，提速%2%     只计算互阻抗%3段，提速%4%")
              .arg(a).arg(100*(n-a)/n).arg(b).arg(100*(n*(n-1)-b)/n/(n-1));
}
