#include "cutthread.h"
#include <QtDebug>

#include "DianzuCompute/dawalibi.h"
#include "ParameterCompute/CurrentCompute/currentcompute.h"
#include "ParameterCompute/cutcompute.h"
#include "ParameterCompute/observedlinecompute.h"
#include "ParameterCompute/potentialcompute.h"
#include "ParameterCompute/stepcompute.h"
#include "ParameterCompute/touchcompute.h"

#include "ui_computdialog.h"


CutThread::CutThread(QList<Conductor> list,ComputDialog *computeDlg,QObject *parent) :
    QThread(parent)
{
    this->datalist = list;
    dlg  = computeDlg;
}


//线程主计算函数
void CutThread::run()
{
    CutCompute *c = new CutCompute(datalist,dlg);
    QList<Conductor> cutlist = c->cut();
    delete c;

    CurrentCompute *currentCompute = new CurrentCompute(cutlist,dlg);
    currentCompute->run();
    delete currentCompute;

    emit(currentName(tr("正在计算地表电位"),tr("6/9")));
    PotentialCompute *potentialCompute = new PotentialCompute(cutlist,dlg);
    potentialCompute->run();
    delete potentialCompute;

    emit(currentName(tr("正在计算观测线电位"),tr("6/9")));
    ObservedLineCompute *observedLineCompute = new ObservedLineCompute(cutlist,dlg);
    observedLineCompute->run();
    delete observedLineCompute;

    emit(currentName(tr("正在计算跨步电压"),tr("7/9")));
    StepCompute *stepCompute = new StepCompute(cutlist,dlg);
    stepCompute->run();
    delete stepCompute;

    emit(currentName(tr("正在计算接触电压"),tr("8/9")));
    TouchCompute *touchCompute = new TouchCompute(cutlist,dlg);
    touchCompute->run();
    delete touchCompute;

    emit(currentName(tr("计算完成！")," "));
    emit computeCompleted();         //发送结束信号
}
