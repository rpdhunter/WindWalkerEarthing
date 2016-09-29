#ifndef GRAPHICSSCENE_H
#define GRAPHICSSCENE_H

#include <QGraphicsScene>
#include "Base/conductor.h"
#include <QItemSelection>
#include "graphicslineitem.h"

class GraphicsScene : public QGraphicsScene
{
    Q_OBJECT
public:
    explicit GraphicsScene(QObject *parent = 0);

    QList<GraphicsLineItem *> mainitemlist();           //返回主数据，用于undo和redo操作
    void setMainItemList(QList<GraphicsLineItem *> list);   //设置主数据

    QList<Conductor> datalist();        //返回主数据的外部可见值
    QList<Conductor> selectedList();    //返回选择的导体组

    QList<GraphicsLineItem *> copylist;


    void deleteSelected();              //删除所选择导体
    void selectAll();

    void editConductor(Conductor c);
    void setSelectedConductorFeatures(double r, double rou, double u, QString remarks);   //批量设置选择导体的特性

    void setGroup();
    void cancelGroup();
    void setFixed();
    void cancelFix();

    void cut();
    void copy();
    void paste();

    void translate(double x,double y,double z);
    void translateTo(double x,double y,double z);
    void rotate(double x);
    void scale(double x);


public slots:
    void doSelectionChanged();      //中转槽函数，当自身选择变化时，通知自身发射selectionChanged(QList<bool> )信号；
    void doItemChanged();           //当场景中的项目发生变化时，发射dataChanged(QList<Conductor> )信号;

    void addConductor(QList<Conductor> list);   //添加新导体

    void doTableSelectionChanged(QItemSelection,QItemSelection);

signals:
    void itemMoved(QList<QGraphicsItem *>, QList<QPointF>, QList<QPointF>);     //同步移动信息，使mainwindow知晓移动动作
    void selectionChanged(QList<GraphicsLineItem *> );  //当自身选择出现变化时，发射信号，通知mainwindow改变表格的选择
    void dataChanged(QList<Conductor> );        //当自身主数据出现变化时，发射信号，通知mainwindow改变表格的数据模型和视图

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

private:
//    void deleteGroup(QGraphicsItemGroup *group);    //删除指定组，以及组中的子组（通过递归实现）
//    void updateData(QList<GraphicsLineItem *> list);//同步指定项目的数据和位置

//    GraphicsLineItem *movingItem;
    QList<QGraphicsItem *> movingItemList;
    QList<QPointF> oldPosList;

//    QPointF oldPos;
    QList<GraphicsLineItem *> itemlist;

   // QMap<int,Conductor> datamap;    //主数据数组，K存储变量编号，V存储导体变量
   // QMap<int,GraphicsLineItem *> linemap; //图形变量视图,K和datamap对应，V存储图形变量
  //  QMap<int,bool> flagmap;         //用于显示datamap和lineitemmap的一些对应状态
  //  int ID;     //导体编号（从1开始，主窗口存在周期内，只能越来越大）
};

#endif // GRAPHICSSCENE_H
