#ifndef PLOTOPTIONDIALOG_H
#define PLOTOPTIONDIALOG_H

#include <QDialog>
#include <QColor>

#include "qwt3d_surfaceplot.h"

namespace Ui {
class PlotOptionDialog;
}

class PlotOptionDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit PlotOptionDialog(Qwt3D::SurfacePlot* widget,QWidget *parent = 0);
    ~PlotOptionDialog();




signals:
//    void valueChanged(int);
    
private slots:
    void on_btn_BackroundColor_clicked();

    void on_btn_MeshColor_clicked();

    void on_btn_AxisColor_clicked();

    void on_btn_NumberColor_clicked();

    void on_btn_LabelColor_clicked();

    void on_btn_TitleColor_clicked();

    void on_btn_DataColor_clicked();

    void on_btn_ResetColor_clicked();

    void on_buttonBox_accepted();

private:
    Ui::PlotOptionDialog *ui;

    void colorInit();
    void fontInit();
    void linghtInit();
    void GradInit();
    void otherInit();


    void setIconColor();

    QMap<QString,QColor> colorMap;
    QPixmap *p;     //用于生成颜色工具条的颜色图形
    Qwt3D::SurfacePlot *plotWidget;

};

#endif // PLOTOPTIONDIALOG_H
