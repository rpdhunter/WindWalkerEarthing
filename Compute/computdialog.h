#ifndef COMPUTDIALOG_H
#define COMPUTDIALOG_H

#include <QDialog>
#include "Base/conductor.h"
#include "Base/observedline.h"
#include "windwalkerearthingmaindlg.h"

class CutThread;
class CurrentSource;

namespace Ui {
class ComputDialog;
}

class ComputDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit ComputDialog(QWidget *parent = 0);
    ~ComputDialog();

    Ui::ComputDialog *ui;

    QString currentProjectDir;  //当前工程文件夹(=dlg->currentProjectDir)
    QString currentDir;     //计算文件夹（/当前工程/计算文件）

    QList<double> earth_thickness;      //土壤分层厚度
    QList<double> earth_resistivity;    //土壤分层电阻率

    QRectF ob_rect;      //观测面
    double ob_z;        //网格深度
    double ob_xmesh;    //x方向网格数
    double ob_ymesh;    //y方向网格数
    QPolygonF weiqiang; //围墙外边框多边形

    double I_in;        //最大故障入地电流
    QList<CurrentSource> currentSourceList; //电源列表

    QList<ObservedLine> observedLineList;   //观测线清单

//    WindWalkerEarthingMainDlg *mainDlg; //父窗口的指针，用于传递一些ui参数（目前只用到当前文件夹）


    QList<Conductor> dataList;          //导体数据

signals:
    void breakCompute();    //终止计算

public slots:
    void setValue(int i,int n);
    void timeUpdate();

    void setTitle(QString title,QString buzhou);

    void totleProgress(int n);  //设定计算总进度

    void computBegin();      //计算开始时，进行一些处理
    void computFinished();      //计算完成后的处理

    int thirdCut();             //返回ui的三次切割数

    void doOutputTextAdd(QString str);  //添加输出文字
    void doDebugTextAdd(QString str);   //添加debug文字输出（给主界面“详细信息提供输出”）

protected:
    void closeEvent(QCloseEvent * event);
    
private slots:

    void on_btn_OK_clicked();

    void on_pushBtn_More_toggled(bool checked);

    void on_btn_Terminate_clicked();

    void on_pushButton_clicked();

    void on_pbn_OptimizeDesign_clicked();   //调出优化设计面板

private:
    void readDataList();
    void readEarth();


    CutThread *cutThread;               //计算线程
    QTimer *timer;
    QTime *startTime;

    QString outputText;         //输出文字
    QString debugText;

};

#endif // COMPUTDIALOG_H
