#ifndef UNEQUAL_Z_V1_H
#define UNEQUAL_Z_V1_H

#include "unequal_r.h"

class UnEqual_Z_V1 : public UnEqual_R
{
    Q_OBJECT
public:
    explicit UnEqual_Z_V1(QList<Conductor> list, ComputDialog *computeDlg);

    void run();
    
signals:
    
public slots:

private:
    cx_mat matrixGenerate_GTT_CX();       //端节点间的导纳矩阵
    cx_mat matrixGenerate_GMM_CX();                                       //中节点间的导纳矩阵
    cx_mat matrixGenerate_GTM_CX();       //端节点和中节点间的导纳矩阵
    
};

#endif // UNEQUAL_Z_V1_H
