#ifndef HEPPE_H
#define HEPPE_H

#include "jifenbase.h"

class Heppe : public JiFenBase
{
public:
    Heppe(Conductor c);
    double huzu(Conductor c2);      //求解互阻(全空间)
    double zizu();                  //求解自电阻(全空间)

    double hugan(Conductor c2);      //求解互感(全空间)
    double waizigan();                  //求解外自电感(全空间)

private:


};

#endif // HEPPE_H
