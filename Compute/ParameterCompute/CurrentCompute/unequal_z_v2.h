#ifndef UNEQUAL_Z_V2_H
#define UNEQUAL_Z_V2_H

#include "unequal_r.h"


class UnEqual_Z_V2 : public UnEqual_R
{
    Q_OBJECT
public:
    explicit UnEqual_Z_V2(QList<Conductor> list, ComputDialog *computeDlg);

    void run();     //不等电位计算主程序
    
signals:
    
public slots:

private:
    cx_mat matrixGenerate_Z(QList<Conductor> cutlist);  //生成互感矩阵
    
};

#endif // UNEQUAL_Z_V2_H
