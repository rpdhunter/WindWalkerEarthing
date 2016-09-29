#ifndef CURRENTCOMPUTE_H
#define CURRENTCOMPUTE_H

#include "../parametercomputebase.h"

//这是一个管理函数
//设立此函数，目的是主函数的简洁

class CurrentCompute : public ParameterComputeBase
{
    Q_OBJECT
public:
    explicit CurrentCompute(QList<Conductor> list, ComputDialog *computeDlg);

    void run();
    
signals:
    
public slots:
    
};

#endif // CURRENTCOMPUTE_H
