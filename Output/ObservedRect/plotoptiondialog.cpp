#include "plotoptiondialog.h"
#include "ui_plotoptiondialog.h"

#include <QColorDialog>
using namespace Qwt3D;

PlotOptionDialog::PlotOptionDialog(Qwt3D::SurfacePlot *widget, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PlotOptionDialog)
{
    ui->setupUi(this);

    ui->listWidget->setCurrentRow(0);
    plotWidget=widget;
    connect(ui->slider,SIGNAL(valueChanged(int)),plotWidget,SLOT(setResolution(int)));

    p = new QPixmap(QSize(15,15));

    colorInit();
    fontInit();
    linghtInit();
    GradInit();
    otherInit();

}

PlotOptionDialog::~PlotOptionDialog()
{
    delete ui;
}

void PlotOptionDialog::colorInit()
{
    colorMap["backgroundColor"]=Qwt3D::GL2Qt(plotWidget->backgroundRGBAColor())  ;
    colorMap["axisColor"]=Qwt3D::GL2Qt(plotWidget->coordinates()->getAxesColor());
    colorMap["meshColor"]=Qwt3D::GL2Qt(plotWidget->meshColor());
    colorMap["numberColor"]=Qwt3D::GL2Qt(plotWidget->coordinates()->getNumberColor());
    colorMap["titleColor"]=Qwt3D::GL2Qt(plotWidget->titleColor());
    colorMap["labelColor"]=Qwt3D::GL2Qt(plotWidget->coordinates()->getLabelColor());
    colorMap["dataColor"]=Qt::red;  //空白

    setIconColor();
}

void PlotOptionDialog::fontInit()
{
}

void PlotOptionDialog::linghtInit()
{
}

void PlotOptionDialog::GradInit()
{
}

void PlotOptionDialog::otherInit()
{
    ui->lineEdit_Title->setText(plotWidget->title() );
    ui->lineEdit_X->setText(plotWidget->coordinates()->axes[X1].labelString());
    ui->lineEdit_Y->setText(plotWidget->coordinates()->axes[Y1].labelString());
    ui->lineEdit_Z->setText(plotWidget->coordinates()->axes[Z1].labelString());

    ui->checkBox_Mouse->setChecked(plotWidget->isMouseEnabled());
    ui->checkBox_Keyboard->setChecked(plotWidget->isKeyboardEnabled());
}

void PlotOptionDialog::setIconColor()
{
    p->fill(colorMap["backgroundColor"]);
    ui->btn_BackroundColor->setIcon(QIcon(*p));
    p->fill(colorMap["axisColor"]);
    ui->btn_AxisColor->setIcon(QIcon(*p));
    p->fill(colorMap["meshColor"]);
    ui->btn_MeshColor->setIcon(QIcon(*p));
    p->fill(colorMap["numberColor"]);
    ui->btn_NumberColor->setIcon(QIcon(*p));
    p->fill(colorMap["titleColor"]);
    ui->btn_TitleColor->setIcon(QIcon(*p));
    p->fill(colorMap["labelColor"]);
    ui->btn_LabelColor->setIcon(QIcon(*p));
    p->fill(colorMap["dataColor"]);
    ui->btn_DataColor->setIcon(QIcon(*p));
}

void PlotOptionDialog::on_btn_BackroundColor_clicked()
{
    QColor c = QColorDialog::getColor( Qt::white, this );
    if ( !c.isValid() )
        return;
    colorMap["backgroundColor"]=c;
    setIconColor();
}

void PlotOptionDialog::on_btn_MeshColor_clicked()
{
    QColor c = QColorDialog::getColor( Qt::white, this );
    if ( !c.isValid() )
        return;
    colorMap["meshColor"]=c;
    setIconColor();
}

void PlotOptionDialog::on_btn_AxisColor_clicked()
{
    QColor c = QColorDialog::getColor( Qt::white, this );
    if ( !c.isValid() )
        return;
    colorMap["axisColor"]=c;
    setIconColor();
}

void PlotOptionDialog::on_btn_NumberColor_clicked()
{
    QColor c = QColorDialog::getColor( Qt::white, this );
    if ( !c.isValid() )
        return;
    colorMap["numberColor"]=c;
    setIconColor();
}

void PlotOptionDialog::on_btn_LabelColor_clicked()
{
    QColor c = QColorDialog::getColor( Qt::white, this );
    if ( !c.isValid() )
        return;
    colorMap["labelColor"]=c;
    setIconColor();
}

void PlotOptionDialog::on_btn_TitleColor_clicked()
{
    QColor c = QColorDialog::getColor( Qt::white, this );
    if ( !c.isValid() )
        return;
    colorMap["titleColor"]=c;
    setIconColor();
}

void PlotOptionDialog::on_btn_DataColor_clicked()
{
    QColor c = QColorDialog::getColor( Qt::white, this );
    if ( !c.isValid() )
        return;
    colorMap["dataColor"]=c;
    setIconColor();
}

void PlotOptionDialog::on_btn_ResetColor_clicked()
{
    colorMap["backgroundColor"]=Qt::white;
    colorMap["axisColor"]=Qt::black;
    colorMap["meshColor"]=Qt::black;
    colorMap["numberColor"]=Qt::black;
    colorMap["titleColor"]=Qt::black;
    colorMap["labelColor"]=Qt::black;
    colorMap["dataColor"]=Qt::blue;

    setIconColor();
}



void PlotOptionDialog::on_buttonBox_accepted()
{
    plotWidget->setBackgroundColor(Qwt3D::Qt2GL(colorMap["backgroundColor"]));
    plotWidget->coordinates()->setAxesColor(Qwt3D::Qt2GL(colorMap["axisColor"]));
    plotWidget->setMeshColor(Qwt3D::Qt2GL(colorMap["meshColor"]));
    plotWidget->coordinates()->setNumberColor(Qwt3D::Qt2GL(colorMap["numberColor"]));
    plotWidget->setTitleColor(Qwt3D::Qt2GL(colorMap["titleColor"]));
    plotWidget->coordinates()->setLabelColor(Qwt3D::Qt2GL(colorMap["labelColor"]));

    plotWidget->setTitle(ui->lineEdit_Title->text());
    plotWidget->coordinates()->axes[X1].setLabelString(ui->lineEdit_X->text());
    plotWidget->coordinates()->axes[X2].setLabelString(ui->lineEdit_X->text());
    plotWidget->coordinates()->axes[X3].setLabelString(ui->lineEdit_X->text());
    plotWidget->coordinates()->axes[X4].setLabelString(ui->lineEdit_X->text());
    plotWidget->coordinates()->axes[Y1].setLabelString(ui->lineEdit_Y->text());
    plotWidget->coordinates()->axes[Y2].setLabelString(ui->lineEdit_Y->text());
    plotWidget->coordinates()->axes[Y3].setLabelString(ui->lineEdit_Y->text());
    plotWidget->coordinates()->axes[Y4].setLabelString(ui->lineEdit_Y->text());
    plotWidget->coordinates()->axes[Z1].setLabelString(ui->lineEdit_Z->text());
    plotWidget->coordinates()->axes[Z2].setLabelString(ui->lineEdit_Z->text());
    plotWidget->coordinates()->axes[Z3].setLabelString(ui->lineEdit_Z->text());
    plotWidget->coordinates()->axes[Z4].setLabelString(ui->lineEdit_Z->text());

    plotWidget->enableMouse(ui->checkBox_Mouse->isChecked());
    plotWidget->enableKeyboard(ui->checkBox_Keyboard->isChecked());
}
