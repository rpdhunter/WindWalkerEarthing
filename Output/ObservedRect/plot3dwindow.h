#ifndef Plot3DWindow_H
#define Plot3DWindow_H

#include <QMainWindow>
#include <QActionGroup>

#include "qwt3d_surfaceplot.h"
#include "windwalkerearthingmaindlg.h"

namespace Ui {
class Plot3DWindow;
}

class QButtonGroup;
class QSplitter;
class Plot2D;
class QLabel;
class QComboBox;

class Plot3DWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit Plot3DWindow(QWidget *parent = 0);
    ~Plot3DWindow();

public slots:
    void createFunction(QAction *action);
    void pickCoordSystem(QAction *action);
    void pickPlotStyle(QAction *action);
    void pickFloorStyle(QAction *action);
    void pickGrid();
    void showGrad(bool val);

    void rotate();

    void doToolbarChange(int );

    void updateColorLegend(int majors, int minors);

//    void replot();  //重绘主视图
//    void creat3DPlot(QString name);  //绘制三维图形
//    void creat2DPlot(QString name);  //绘制二维图形
    
private slots:
    void on_atn_Reset_triggered();

    void on_atn_Animation_toggled(bool arg1);

    void on_atn_Option_triggered();

    void on_atn_Ortho_toggled(bool arg1);

    void on_atn_Light_toggled(bool arg1);

    void on_atn_Legend_toggled(bool arg1);

    void on_atn_AutoScale_toggled(bool arg1);

    void on_atn_Save_triggered();

    void on_atn_Open_triggered();

    void on_atn_Guangpu_toggled(bool arg1);

    void on_atn_Denggaoxian_toggled(bool arg1);

    void on_atn_Huijie_toggled(bool arg1);

    void on_atn_Text_triggered();

    void on_atn_ShowConductor_toggled(bool arg1);

    void on_atn_ConductorText_toggled(bool arg1);

    void on_atn_Fit_triggered();

    void showRotate(double x, double y, double z);
    void showShift(double x, double y);
    void showScale(double x, double y, double z);
    void showZoom(double z);
    void doComboBoxChanged(QString str);    //相应修改显示数据的格式


    void on_atn_PointVotage_toggled(bool arg1);

private:
    Ui::Plot3DWindow *ui;

    WindWalkerEarthingMainDlg *mainDlg;

    QSplitter *splitter;

    void initActions();     //初始化Actions
    void iniStatus();       //初始化状态栏


    Qwt3D::SurfacePlot* dataWidget_3D;
    Plot2D *plot2d;     //2维绘图

    QTimer *timer;

    QButtonGroup *group_tuxing;  //图形选择组
    QButtonGroup *group_weidu;   //维度选择组


    QActionGroup* maindataGroup;    //主数据视图
    QActionGroup* coordGroup;       //坐标显示方式
    QActionGroup* plotstyleGroup;   //数据显示风格
    QActionGroup* gridsGroup;       //网格显示
    QActionGroup* floorstyleGroup;  //投影风格

    //一些用于状态显栏示的Label
    QLabel *filenameWidget, *dimWidget,*rotateLabel, *shiftLabel, *scaleLabel, *zoomLabel;

    QComboBox *comboBox;        //用于控制复数的显示

};

#endif // Plot3DWindow_H
