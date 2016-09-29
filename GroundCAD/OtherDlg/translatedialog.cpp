#include "translatedialog.h"
#include "ui_translatedialog.h"

TranslateDialog::TranslateDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TranslateDialog)
{
    ui->setupUi(this);
}

TranslateDialog::~TranslateDialog()
{
    delete ui;
}

double TranslateDialog::getX()
{
    return ui->lineEdit_X->text().toDouble();
}

double TranslateDialog::getY()
{
    return ui->lineEdit_Y->text().toDouble();
}

double TranslateDialog::getZ()
{
    return ui->lineEdit_Z->text().toDouble();
}

int TranslateDialog::flag()
{
    return ui->comboBox->currentIndex();
}

#include "ui_rotatedialog.h"

RotateDialog::RotateDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RotateDialog)
{
    ui->setupUi(this);
}

RotateDialog::~RotateDialog()
{
    delete ui;
}

double RotateDialog::angle()
{
    return ui->lineEdit->text().toDouble();
}

#include "ui_scaledialog.h"

ScaleDialog::ScaleDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ScaleDialog)
{
    ui->setupUi(this);
}

ScaleDialog::~ScaleDialog()
{
    delete ui;
}

double ScaleDialog::scale()
{
    return ui->lineEdit->text().toDouble();
}

//导体编辑对话框
#include "ui_editconductordialog.h"

EditConductorDialog::EditConductorDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EditConductorDialog)
{
    ui->setupUi(this);
}

EditConductorDialog::~EditConductorDialog()
{
    delete ui;
}

void EditConductorDialog::setConductor(Conductor c)
{
    ui->lineEdit_X0->setText(tr("%1").arg(c.x0));
    ui->lineEdit_X1->setText(tr("%1").arg(c.x1));
    ui->lineEdit_Y0->setText(tr("%1").arg(c.y0));
    ui->lineEdit_Y1->setText(tr("%1").arg(c.y1));
    ui->lineEdit_Z0->setText(tr("%1").arg(c.z0));
    ui->lineEdit_Z1->setText(tr("%1").arg(c.z1));

    ui->lineEdit_R->setText(tr("%1").arg(c.r));
    ui->lineEdit_Rou->setText(tr("%1").arg(c.rou));
    ui->lineEdit_U->setText(tr("%1").arg(c.u));
    ui->lineEdit_Remarks->setText(c.remarks);
}

Conductor EditConductorDialog::getConductor()
{
    Conductor c;
    c.x0=ui->lineEdit_X0->text().toDouble();
    c.x1=ui->lineEdit_X1->text().toDouble();
    c.y0=ui->lineEdit_Y0->text().toDouble();
    c.y1=ui->lineEdit_Y1->text().toDouble();
    c.z0=ui->lineEdit_Z0->text().toDouble();
    c.z1=ui->lineEdit_Z1->text().toDouble();

    c.r =ui->lineEdit_R->text().toDouble();
    c.rou =ui->lineEdit_Rou->text().toDouble();
    c.u =ui->lineEdit_U->text().toDouble();
    c.remarks =ui->lineEdit_Remarks->text();

    return c;
}

#include "ui_oblineeditdialog.h"

ObLineEditDialog::ObLineEditDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ObLineEditDialog)
{
    ui->setupUi(this);
}

ObLineEditDialog::~ObLineEditDialog()
{
    delete ui;
}

void ObLineEditDialog::setObLine(ObservedLine line)
{
    ui->ObLine_X1->setText(tr("%1").arg(line.P1().x));
    ui->ObLine_Y1->setText(tr("%1").arg(line.P1().y));
    ui->ObLine_Z1->setText(tr("%1").arg(line.P1().z));
    ui->ObLine_X2->setText(tr("%1").arg(line.P2().x));
    ui->ObLine_Y2->setText(tr("%1").arg(line.P2().y));
    ui->ObLine_Z2->setText(tr("%1").arg(line.P2().z));
    ui->ObLine_Name->setText(line.name());
    ui->ObLine_Subsection->setValue(line.subsection());
    ui->ObLine_Gap->setText(tr("%1").arg(line.gap()));
}

ObservedLine ObLineEditDialog::getObLine()
{
    ObservedLine line;
    Point P1(ui->ObLine_X1->text().toDouble(),ui->ObLine_Y1->text().toDouble(),ui->ObLine_Z1->text().toDouble());
    Point P2(ui->ObLine_X2->text().toDouble(),ui->ObLine_Y2->text().toDouble(),ui->ObLine_Z2->text().toDouble());
    line.setPoints(P1,P2);
    line.setName(ui->ObLine_Name->text());
    line.setSubsection(ui->ObLine_Subsection->value());
    return line;
}
