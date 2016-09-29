#include "parametercomputebase.h"
#include "ui_computdialog.h"

ParameterComputeBase::ParameterComputeBase(QList<Conductor> list, ComputDialog *computeDlg,QObject *parent) :
    QObject(parent)
{
    this->cutlist = list;
    this->dlg  = computeDlg;

    //只能在构造函数中初始化
    if(dlg->ui->radioButton_E->isChecked()){
        this->model_ = equal;
    }
    else if(dlg->ui->radioButton_UE->isChecked()){
        this->model_ = unequal_r;
    }
    else if(dlg->ui->radioButton_UE_Z_V1->isChecked()){
        this->model_ = unequal_z_v1;
    }
    else if(dlg->ui->radioButton_UE_Z_V2->isChecked()){
        this->model_ = unequal_z_v2;
    }

    connect(this,SIGNAL(progressValueChanged(int,int)),dlg,SLOT(setValue(int,int)));
    connect(this,SIGNAL(currentName(QString,QString)),dlg,SLOT(setTitle(QString,QString)));
    connect(this,SIGNAL(outputTextAdd(QString)),dlg,SLOT(doOutputTextAdd(QString)));
    connect(this,SIGNAL(debugTextAdd(QString)),dlg,SLOT(doDebugTextAdd(QString)));
}

ParameterComputeBase::Model ParameterComputeBase::model()
{
    return this->model_;
}

//输出文本至
//1.debug控制台
//2.程序界面输出
void ParameterComputeBase::deb(QString str)
{
    qDebug()<<str;
    emit debugTextAdd(str);
}

