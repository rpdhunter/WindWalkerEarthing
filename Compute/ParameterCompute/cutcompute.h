#ifndef CUTCOMPUTE_H
#define CUTCOMPUTE_H

#include "parametercomputebase.h"


class CutCompute : public ParameterComputeBase
{
public:
    CutCompute(QList<Conductor> list, ComputDialog *computeDlg);

    QList<Conductor> cut();

private:

    void powerProintCut();      //电源接入点切割（发生在交点切割之前）

    void saveCutlistToFile(QList<Conductor> cutlist);
};

#endif // CUTCOMPUTE_H
