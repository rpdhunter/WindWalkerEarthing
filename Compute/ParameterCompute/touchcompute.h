#ifndef TOUCHCOMPUTE_H
#define TOUCHCOMPUTE_H

#include "parametercomputebase.h"
#include "Base/currentsource.h"
#include "armadillo"

using namespace arma;

class TouchCompute : virtual public ParameterComputeBase
{
public:
    TouchCompute(QList<Conductor> list, ComputDialog *computeDlg);

    void run();

protected:
    double getNearestGPR(Point P);
    cx_double getNearestGPR_cx(Point P);
    void readVoltage(); //读取电位值

private:    
    //接触电压相关函数
    void touchIn(); //围墙内接触电压

    void touch_Equal();  //等电位接触电压
    void touch_UnEqual_R();//不等电位接触电压
    void touch_UnEqual_Z();//不等电位接触电压

    QList<CurrentSource> voltageList;
};

#endif // TOUCHCOMPUTE_H
