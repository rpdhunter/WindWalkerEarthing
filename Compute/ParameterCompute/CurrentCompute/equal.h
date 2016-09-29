#ifndef EQUAL_H
#define EQUAL_H

//计算电流分布的类
//需要输入导体（切割好），土壤，电源情况
//得到电流分布，接地电阻
//可以使用等电位和不等电位两种模型

#include "../parametercomputebase.h"
#include "Compute/computdialog.h"
#include "base/currentsource.h"
#include "armadillo"

using namespace arma;
class Equal : public ParameterComputeBase
{
    Q_OBJECT
public:
    explicit Equal(QList<Conductor> list, ComputDialog *computeDlg);

    void run();             //等电位模型计算

    mat matrixGenerate(QList<Conductor> cutlist);      //生成电阻矩阵
    
signals:
    
public slots:

private:

    double matrixSolve(mat R);       //系数矩阵求解,返回接地电阻
    
};

#endif // EQUAL_H
