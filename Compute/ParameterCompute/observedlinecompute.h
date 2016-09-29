#ifndef OBSERVEDLINECOMPUTE_H
#define OBSERVEDLINECOMPUTE_H

#include "stepcompute.h"
#include "touchcompute.h"

///////////////////////////////////////////////////////////
/// \计算观测线的类，由于使用了多重继承，使用了“虚基类”的高级技术
///////////////////////////////////////////////////////////
class ObservedLineCompute : public StepCompute, public TouchCompute
{
public:
    ObservedLineCompute(QList<Conductor> list, ComputDialog *computeDlg);

    void run();

private:
    void observedLine();        //观测线电位计算
    void observedLine_R();        //观测线电位计算(模式3)
    void observedLine_Z();        //观测线电位计算(模式4)
};

#endif // OBSERVEDLINECOMPUTE_H
