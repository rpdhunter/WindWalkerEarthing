//导体输入的人机交互界面
//完成1.导体输入 2.接地网生成 3.输入拼写检查
#ifndef NEWGRID_H
#define NEWGRID_H

#include <QDialog>
#include <QtCore>
#include <QLineEdit>
#include <QAbstractButton>
#include "Base/conductor.h"
#include "../Graphics/graphicsscene.h"
#include "../GroundingCADWindow.h"

namespace Ui {
    class NewGrid;
}

const double pi=3.14159265358979;

class NewGrid : public QDialog
{
    Q_OBJECT

public:
    explicit NewGrid(GraphicsScene *scene,QWidget *parent = 0);
    ~NewGrid();



signals:
    void dataAdded(QList<Conductor> );  //导体添加信号
    //观测面添加信号
    void obAdded(QRectF ob_rect,int ob_mesh_x,int ob_mesh_y,double ob_z);

    void observedLineAdded(ObservedLine);   //

public slots:

private:
    Ui::NewGrid *ui;
    GraphicsScene *myscene; //本地的scene，用于预览
    GraphicsScene *scene;   //父窗口的scene

    GroundingCADWindow *groundingCADWindow; //父窗口指针

    QList<Conductor> datalist;      //父窗口传来的导体列表

    QList<Conductor> uiDatalist;    //ui设置的导体列表


    void readData1();
    void readData2();
    void readData3();
    void readData4();
    void readData5();
    void readData6();

    QList<Conductor> createGrid(const double k);
    QList<Conductor> createGrid_CaoXiaobin();   //曹晓斌
    QList<Conductor> createGrid_GB();       //国标
    double GB_dingzhi(int k, int n);

    void readData();    //读取ui数据(导体数据以返回值形式读取，观测面和围墙数据是直接操作父窗口数据（公有变量）)
    void iniData();     //初始化ui数据

private slots:

    void on_pushButton_update_clicked();
    void on_tBn_Add_clicked();
    void on_tBn_Del_clicked();
    void on_tBn_Up_clicked();
    void on_tBn_Down_clicked();
    void doClearPreview();      //重置预览图
    void doDataAdded(QAbstractButton* button);


    void doFeatureDlg();
};

#endif // NEWGRID_H
