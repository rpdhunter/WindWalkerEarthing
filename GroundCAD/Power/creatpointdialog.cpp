#include "creatpointdialog.h"
#include "ui_creatpointdialog.h"

#include <QMessageBox>
#include "../GroundingCADWindow.h"
#include "Base/compute.h"
#include "Base/conductor.h"
#include <QList>

CreatPointDialog::CreatPointDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CreatPointDialog)
{
    ui->setupUi(this);

    groundingCADWindow = qobject_cast<GroundingCADWindow*>(parent->parent()->parent());     //这尼玛，我咱编出这么奇葩的程序的？

    iniUi();
}

CreatPointDialog::~CreatPointDialog()
{
    delete ui;
}

//对外唯一接口，返回交点
Point CreatPointDialog::getCrossPoint()
{
    int n1 = ui->spinBox1->value()-1;
    int n2 = ui->spinBox2->value()-1;
    Conductor c1 = groundingCADWindow->datalist().value(n1);
    Conductor c2 = groundingCADWindow->datalist().value(n2);
    Conductor c = Compute::getShortestDistance(c1,c2);
    return c.P0();      //返回公垂线的一端即可
}

void CreatPointDialog::on_buttonBox_accepted()
{
    int n1 = ui->spinBox1->value()-1;
    int n2 = ui->spinBox2->value()-1;
    if(groundingCADWindow->datalist().count()<2){
        QMessageBox::information(this,tr("条件不足"),tr("请返回主界面，输入2根以上导体之后，才能使用此功能！"));
        this->reject();
    }
    else if(n1==n2){
        QMessageBox::information(this,tr("输入导体不正确"),tr("请输入不同导体！"));
    }
    else{
        Conductor c1 = groundingCADWindow->datalist().value(n1);
        Conductor c2 = groundingCADWindow->datalist().value(n2);
        if(Compute::space(c1,c2)<0.00001){   //相交
            this->accept();
        }
        else{                           //不相交
            QMessageBox::information(this,tr("失败"),tr("输入导体并无交点！"));
            this->reject();
        }
    }

}

void CreatPointDialog::iniUi()
{
    int n = groundingCADWindow->datalist().count();
    if(n==0){
        ui->spinBox1->setMaximum(0);
        ui->spinBox2->setMaximum(0);
        ui->spinBox1->setMinimum(0);
        ui->spinBox2->setMinimum(0);
    }
    else{
        ui->spinBox1->setMaximum(n);
        ui->spinBox2->setMaximum(n);
        ui->spinBox1->setMinimum(1);
        ui->spinBox2->setMinimum(1);
    }

}
