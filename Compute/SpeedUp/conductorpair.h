#ifndef CONDUCTORPAIR_H
#define CONDUCTORPAIR_H

#include "Base/conductor.h"

//本类为SpeedUp类提供了数据类型支持
//为了SpeedUp类可以使用QMap，需要在本类中提供大小比较函数
class ConductorPair
{
public:
    ConductorPair(Conductor c1,Conductor c2);
    ConductorPair(Conductor c1);

    enum Mode{
        Huzu = 0,
        Zizu = 1
    };

    bool operator == (const ConductorPair &other) const;
    bool operator > (const ConductorPair &other) const;
    bool operator < (const ConductorPair &other) const;

    Conductor c1,c2;
    double l1,l2;
    Mode myMode;

};

#endif // CONDUCTORPAIR_H
