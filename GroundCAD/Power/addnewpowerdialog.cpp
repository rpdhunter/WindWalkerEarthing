#include "addnewpowerdialog.h"
#include "ui_addnewpowerdialog.h"
#include "Base/currentsource.h"
#include "creatpointdialog.h"


AddNewPowerDialog::AddNewPowerDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddNewPowerDialog)
{
    ui->setupUi(this);
}

AddNewPowerDialog::~AddNewPowerDialog()
{
    delete ui;
}

//初始化ui
void AddNewPowerDialog::iniUi(CurrentSource cs)
{
    ui->lineEdit_X->setText(QString("%1").arg(cs.P.x));
    ui->lineEdit_Y->setText(QString("%1").arg(cs.P.y));
    ui->lineEdit_Z->setText(QString("%1").arg(cs.P.z));

    ui->lineEdit_Real->setText(QString("%1").arg(cs.real()));
    ui->lineEdit_Imaginary->setText(QString("%1").arg(cs.imag()));
    ui->lineEdit_Mo->setText(QString("%1").arg(cs.norm()));
    ui->lineEdit_Fujiao->setText(QString("%1").arg(cs.arg()));
    ui->lineEdit_F->setText(QString("%1").arg(cs.f));
    ui->lineEdit_Remarks->setText(cs.remarks);
}

//读取ui数据
CurrentSource AddNewPowerDialog::readUi()
{
    CurrentSource cs;
    cs.P = Point(ui->lineEdit_X->text().toDouble(),ui->lineEdit_Y->text().toDouble(),ui->lineEdit_Z->text().toDouble());
    if(ui->radioButton->isChecked()){   //代数形式输入
        cs.setCurrent(ui->lineEdit_Real->text().toDouble(),ui->lineEdit_Imaginary->text().toDouble());
    }
    else{
        cs.setCurrent_arg(ui->lineEdit_Mo->text().toDouble(),ui->lineEdit_Fujiao->text().toDouble());
    }
    cs.f = ui->lineEdit_F->text().toDouble();
    cs.remarks = ui->lineEdit_Remarks->text();

    return cs;
}

//根据输入导体交点求得电流注入点坐标
void AddNewPowerDialog::on_pushButton_clicked()
{
    CreatPointDialog dlg(this);
    if(dlg.exec()==1){
        Point P = dlg.getCrossPoint();
        ui->lineEdit_X->setText(QString("%1").arg(P.x));
        ui->lineEdit_Y->setText(QString("%1").arg(P.y));
        ui->lineEdit_Z->setText(QString("%1").arg(P.z));
    }

}

//以下四个函数使得实部虚部和模幅角表述相一致
void AddNewPowerDialog::on_lineEdit_Real_editingFinished()
{
    CurrentSource c;
    c.setCurrent(ui->lineEdit_Real->text().toDouble(),ui->lineEdit_Imaginary->text().toDouble());
    ui->lineEdit_Mo->setText(QString("%1").arg(c.norm()));
    ui->lineEdit_Fujiao->setText(QString("%1").arg(c.arg()));
}

void AddNewPowerDialog::on_lineEdit_Imaginary_editingFinished()
{
    on_lineEdit_Real_editingFinished();
}

void AddNewPowerDialog::on_lineEdit_Mo_editingFinished()
{
    CurrentSource c;
    c.setCurrent_arg(ui->lineEdit_Mo->text().toDouble(),ui->lineEdit_Fujiao->text().toDouble());
    ui->lineEdit_Real->setText(QString("%1").arg(c.real()));
    ui->lineEdit_Imaginary->setText(QString("%1").arg(c.imag()));
}

void AddNewPowerDialog::on_lineEdit_Fujiao_editingFinished()
{
    on_lineEdit_Mo_editingFinished();
}

