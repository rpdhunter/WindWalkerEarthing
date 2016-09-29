#ifndef POTENTIALCOMPUTE_H
#define POTENTIALCOMPUTE_H

#include "parametercomputebase.h"
#include "armadillo"

using namespace arma;

//计算地表电位的类
class PotentialCompute : virtual public ParameterComputeBase
{
public:
    PotentialCompute(QList<Conductor> list, ComputDialog *computeDlg);

    void run();


protected:
    double conductorsPotential(QList<Conductor> cutlist,Point P,vec I);    //计算导体组在P点产生的电位
    cx_double conductorsPotential(QList<Conductor> cutlist,Point P,cx_vec I);    //计算导体组在P点产生的电位(电流为复数的版本)

private:
    void potentialGenerate(); //电位分布计算
    void potentialGenerate_cx(); //电位分布计算(复数)
    //地表电势相关函数
    double conductorPotential(Conductor c1,Point P,double I);       //计算单根导体在P点产生的电位
    cx_double conductorPotential(Conductor c1,Point P,cx_double I);       //计算单根导体在P点产生的电位(电流为复数的版本)

};

#endif // POTENTIALCOMPUTE_H
