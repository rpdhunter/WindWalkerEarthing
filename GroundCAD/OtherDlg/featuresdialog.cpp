#include "featuresdialog.h"
#include "ui_featuresdialog.h"
#include "../GroundingCADWindow.h"

const double roubase = 1.7241e-8;       //电阻率参考基值
const double ubase = 12.5664e-7;       //电阻率参考基值


FeaturesDialog::FeaturesDialog(GroundingCADWindow *dlg,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FeaturesDialog)
{
    ui->setupUi(this);
    groundingCADWindow = dlg;

    //控件初始化
    ui->lineEdit_R->setText(QString("%1").arg(dlg->defultR));
    ui->lineEdit_Rou->setText(QString("%1").arg(dlg->defultRou));
    ui->lineEdit_U->setText(QString("%1").arg(dlg->defultU));
    ui->lineEdit_Remarks->setText(QString("%1").arg(dlg->defultRemarks));
}

FeaturesDialog::~FeaturesDialog()
{
    delete ui;
}

//读取默认值，并且修改父窗口的默认参数
void FeaturesDialog::getDefult()
{
    double R = ui->lineEdit_R->text().toDouble();
    if(R>0){
        groundingCADWindow->defultR = R;
    }

    double rou = ui->lineEdit_Rou->text().toDouble();
    if(rou>0){
        groundingCADWindow->defultRou = rou;
    }

    double u = ui->lineEdit_U->text().toDouble();
    if(u>0){
        groundingCADWindow->defultU = u;
    }

    groundingCADWindow->defultRemarks = ui->lineEdit_Remarks->text();

}

/////////////////////////////////////////////
/// \以下代码段用于协调电阻率、磁导率的相对值和绝对值
/////////////////////////////////////////////
void FeaturesDialog::on_lineEdit_Rou_xiangdui_editingFinished()
{
    double rou = ui->lineEdit_Rou_xiangdui->text().toDouble();
    ui->lineEdit_Rou->setText(QString("%1").arg(rou*roubase));
}

void FeaturesDialog::on_lineEdit_Rou_editingFinished()
{
    double rou = ui->lineEdit_Rou->text().toDouble();
    ui->lineEdit_Rou_xiangdui->setText(QString("%1").arg(rou/roubase));
}

void FeaturesDialog::on_lineEdit_U_editingFinished()
{
    double u = ui->lineEdit_U->text().toDouble();
    ui->lineEdit_U_juedui->setText(QString("%1").arg(u*ubase));
}

void FeaturesDialog::on_lineEdit_U_juedui_editingFinished()
{
    double u = ui->lineEdit_U_juedui->text().toDouble();
    ui->lineEdit_U->setText(QString("%1").arg(u/ubase));
}
