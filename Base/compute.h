#ifndef COMPUTE_H
#define COMPUTE_H

#include <QObject>
#include "Base/conductor.h"

//此类只要完成计算功能
class Compute : public QObject
{
    Q_OBJECT
public:

    enum Relation{
        skew,       //异面
        parallel,   //平行
        crossing,   //相交（直接相交）
        collinear,  //共线（不重合）
        error
    };

    explicit Compute(QObject *parent = 0);

    static Compute::Relation relationship(Conductor c1,Conductor c2); //返回两线段所在直线的位置关系
    static double jiaoDu(Conductor c1,Conductor c2);        //返回两导体的交角（弧度，范围0-pi/2）
    static double space(Conductor c1,Conductor c2);      //返回两导体的最短距离
    static bool isCoincide(Conductor c1,Conductor c2);  //检查导体是否重叠
    static Conductor getShortestDistance(Conductor c1,Conductor c2);  //返回两导体段的交点（线段）
    static Conductor getCrossPoint(Conductor c1,Conductor c2);  //返回两导体段的公垂线段

    static Conductor Pingyi(Conductor c, double a);    //平移导体(随机方向)
    static Conductor Pingyi(Conductor c1,Conductor c2, double a);    //平移导体c2（延远离c1的方向）
    static Conductor Suoduan(Conductor c1, double a);   //缩短导体

    QList<Conductor> cutConductor(QList<Conductor> datalist);   //导体组切割函数
    static double totalLength(QList<Conductor> list);            //求总长度
    static double averageLength(QList<Conductor> list);            //求平均长度
    static double minLength(QList<Conductor> list);                //求最短长度
    bool cutLongConductor(QList<Conductor> &list);        //切割过长导体
    QList<Conductor> secendCut(QList<Conductor> list);               //二次切割
    QList<Conductor> thirdCut(QList<Conductor> list, int k);               //三次切割(仅作研究之用)
    QList<Conductor> averageCut(Conductor c, int k);        //平均切割

    static bool insertPoint(QVector<double> &P, QList<QVector<double> > &points);  //插入点

    
signals:
    void progressValueChanged_cut(int,int);

public slots:
    
};

#endif // COMPUTE_H
