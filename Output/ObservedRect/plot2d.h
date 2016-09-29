#ifndef PLOT2D_H
#define PLOT2D_H

#include <qwt_plot.h>
#include <qwt_plot_spectrogram.h>
#include "Base/conductor.h"
#include "plot2ditem/conductoritem.h"
#include "plot2ditem/pointvoltageitem.h"
#include "Base/currentsource.h"

class Plot2D : public QwtPlot
{
    Q_OBJECT

public:
    Plot2D(QString str,QWidget * parent = NULL);

    void setData(QString shape);


public Q_SLOTS:
    void showContour( bool on );    //光谱线
    void showSpectrogram( bool on );    //等高线
    void showBlackWhite( bool on);  //显示灰阶
    void showConductor(bool on);    //显示导体
    void showConductorText(bool on);    //显示导体信息
    void showPointVoltage(bool on);     //显示节点电压
    void setMode(QString str);      //设置复数显示模式

protected:
//    void paintEvent(QPaintEvent *event);

private:
    QwtPlotSpectrogram *d_spectrogram;  //视图画布
    QwtScaleWidget *rightAxisWidget;
    QString path;       //数据文件夹路径

    QList<Conductor> cutlist;
    QList<ConductorItem*> cutConductorlist;

    QList<CurrentSource> pointvoltagelist;
    QList<PointVoltageItem*> pointvoltageitemlist;

    void resetView();
    void addAxisWidget();

    void readCutConductor();    //读取切割导体
    void readPointVoltage();    //读取节点电位
//    void readI();               //读取电流
};

#endif // PLOT2D_H
