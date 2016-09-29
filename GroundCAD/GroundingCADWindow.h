#ifndef GroundingCADWindow_H
#define GroundingCADWindow_H

#include <QMainWindow>
#include <QTableView>
#include <QStandardItemModel>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QLabel>
#include <QMouseEvent>
#include <QProgressBar>
#include <QActionGroup>
#include <QUndoStack>

#include "Base/conductor.h"
#include "Base/observedline.h"
#include "Base/currentsource.h"
#include "Graphics/datamodel.h"
#include "Graphics/graphicsscene.h"
#include "Graphics/graphicsview.h"
#include "windwalkerearthingmaindlg.h"



namespace Ui {
class GroundingCADWindow;
}

class ObLineListModle;
class PowerListModel;


class GroundingCADWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit GroundingCADWindow(QWidget *parent = 0);
    ~GroundingCADWindow();

    QRectF ob_rect;     //观测面矩形
    double ob_z;        //网格深度
    int ob_xmesh;    //x方向网格数
    int ob_ymesh;    //y方向网格数

    QPolygonF weiqiang(); //围墙轮廓多边形
    void setWeiqiang(QPolygonF p);   //设置围墙多边形
    QPolygonF weiqiangPolygon;

    QList<ObservedLine> observedLineList;

    double defultR;         //默认半径
    double defultRou;         //默认导体电阻率
    double defultU;         //默认导体相对磁导率
    QString defultRemarks;         //默认导体标识

    double current;                 //故障入地电流（等电位）
    QList<CurrentSource> currentSourceList;     //故障入地电流源列表（不等电位）

    QList<Conductor> datalist();        //返回导体个数（目前只用一次）

signals:
    //    void datalistChanged(QList<Conductor> );
    //    void cutlistChanged(QList<Conductor> );

    void datalistChangeSaved();

    
private slots:
    void doSelectionChanged(QList<GraphicsLineItem *> list);
    void doMenuStateChanged();      //根据选择变化，对菜单状态等作相应处理
    void doDatalistChanged();   //导体数据相关处理函数
    void doCutlistChanged();   //导体数据相关处理函数

    void doCustomContextMenu(const QPoint pos); //右键菜单
    void doObCustomContextMenu(const QPoint pos); //观测线右键菜单
    void doPowerCustomContextMenu(const QPoint pos);    //电源右键

    void setObRect(QRectF ob_r,int mesh_x,int mesh_y,double z);
    void addObservedLine(ObservedLine line);

    void itemMoved(QList<QGraphicsItem*> itemList,QList<QPointF> newPosList, QList<QPointF> oldPosList);

    void on_atn_AddObject_triggered();

    void on_atn_zoomin_triggered();

    void on_atn_zoomout_triggered();

    void on_acn_Zoom1_triggered();

    void on_atn_fit_triggered();

    void on_atn_check_triggered();

    void on_atn_dataCut_triggered();

    void on_atn_Point_triggered();

    void on_atn_Pen_toggled(bool arg1);

    void on_atn_Hand_triggered();

    void on_atn_Group_triggered();

    void on_atn_CancelGroup_triggered();

    void on_atn_fixed_triggered();

    void on_atn_CancelFix_triggered();

    void on_atn_All_triggered();

    void on_atn_Del_triggered();

    void on_atn_Cut_triggered();

    void on_atn_Copy_triggered();

    void on_atn_Paste_triggered();

    void on_atn_translate_triggered();

    void on_atn_scale_triggered();

    void on_atn_rotate_triggered();

    void on_atn_EditConductor_triggered();

    void on_atn_Save_triggered();

    void on_atn_Open_triggered();

    void on_atn_New_triggered();

    void on_atn_SaveAs_triggered();

    void on_atn_OBShow_toggled(bool arg1);      //显示围墙和观测面


    void on_atn_EditObLine_triggered();

    void on_atn_DelObLine_triggered();

    void on_atn_features_triggered();

    void on_atn_Power_triggered();

protected:

    void closeEvent(QCloseEvent *event);

private:
    Ui::GroundingCADWindow *ui;
    WindWalkerEarthingMainDlg *windWalkerDlg;


    QActionGroup *pointerGroup;

    QAction *undoAction;        //undo和redo的动作，没有通过界面编辑做是想动态改变显示文字
    QAction *redoAction;
    QUndoStack *undoStack;      //设定重做队列

    QLabel *pointPos;
    QProgressBar *progressBar;

    DataModel *datamodel;
    QItemSelectionModel *selectionModel;

    QList<Conductor> cutlist;
    DataModel *cutmodel;

    ObLineListModle *observedLineModel;     //观测线模型
    PowerListModel *powerListModel;         //电源数据模型

    GraphicsView *view;
    GraphicsScene *scene;

    qreal scale;
    int ID;         //导体编号（从1开始，主窗口存在周期内，只能越来越大）

    enum{uncheck,passed,fail} isChecked;  //接地网检测的标志，成功后才可分割计算


//    void showdata(const QList<Conductor > list);
//    void drawdata(const QList<Conductor > list, QPen pen);


    void menuSetup();
    void modelViewSetup();
    void statusBarSetup();

    void readFile();    //读取标准位置文件
    void read(QString fileName);        //【读取】核心功能
    void saveFile();    //保存标准位置文件
    void save(QString fileName);        //【保存】核心功能

    void doObservedLineChanged();       //进行观测线修改操作
    void doPowerListChanged();          //进行电源修改



  //  QList<Conductor> cutConductor(QList<Conductor> datalist);

};

#endif // GroundingCADWindow_H
