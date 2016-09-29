#ifndef CUTTHREAD_H
#define CUTTHREAD_H

#include <QThread>
#include "Base/conductor.h"
#include "Base/point.h"

#include "Base/compute.h"
#include "computdialog.h"

#include "armadillo"

using namespace arma;

//class mat;

class ComputDialog;

//切割计算线程
class CutThread : public QThread
{
    Q_OBJECT
public:
    explicit CutThread(QList<Conductor> list,ComputDialog *computeDlg, QObject *parent = 0);

signals:
    void progressValueChanged(int,int);     //进度条数值显示
    void computeCompleted();            //计算完毕信号
    void currentName(QString,QString);          //当前进行计算的名称，步数

public slots:

protected:
    void run();

private:
    QList<Conductor> datalist;
    ComputDialog *dlg;

    QString outputText;     //输出文档

};


#endif // CUTTHREAD_H
