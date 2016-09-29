#ifndef PARAMETERCOMPUTEBASE_H
#define PARAMETERCOMPUTEBASE_H

#include <QObject>
#include <QtDebug>
#include "Compute/computdialog.h"

//参数计算的基类
//之所以使用派生结构是为了重复使用信号连接，界面类等共同信息
//2013.11.3加入了Model属性，使得计算基于不同模型，并且加入了run()的虚函数，使得所有实例的运行函数统一了接口

class ParameterComputeBase : public QObject
{
    Q_OBJECT
public:
    explicit ParameterComputeBase(QList<Conductor> list, ComputDialog *computeDlg,QObject *parent = 0);

    enum Model{
        equal,          //等电位
        unequal_r,      //不等电位，忽略感抗
        unequal_z_v1,   //不等电位，计算感抗，但忽略互感
        unequal_z_v2    //不等电位，计算感抗，不忽略互感（最精确）
    };

    virtual void run(){}     //对象的执行函数
    
signals:
    void progressValueChanged(int,int);         //进度条数值显示
    void currentName(QString,QString);          //当前进行计算的名称，步数
    void outputTextAdd(QString);                //添加输出的文字信息
    void debugTextAdd(QString);                 //添加debug显示文字信息
    
public slots:

protected:
    ComputDialog *dlg;
    QList<Conductor> cutlist;
    Model model();      //计算模型
    void deb(QString str);  //输出信息

private:
    Model model_;
    
};

#endif // PARAMETERCOMPUTEBASE_H
