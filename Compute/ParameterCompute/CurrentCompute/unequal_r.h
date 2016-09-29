#ifndef UNEQUAL_R_H
#define UNEQUAL_R_H

#include "equal.h"

struct CrossPoint;

class UnEqual_R : public Equal
{
    Q_OBJECT
public:
    explicit UnEqual_R(QList<Conductor> list, ComputDialog *computeDlg);

    void run();         //不等电位计算主程序    
    
signals:
    
public slots:

protected:
    QList<CrossPoint> crossPointList;       //交点列表
    QList<CrossPoint> powerlist;            //电源列表
    int n;     //中点数
    int m;     //交点数

    void savePointVoltageToFile(vec v,QList<CrossPoint> crossPointList);      //将所有点电压写入文件
    void savePointVoltageToFile(cx_vec v,QList<CrossPoint> crossPointList);      //将所有点电压写入文件

    cx_vec currentGenerate_cx();          //生成电流列表(复数)
    cx_vec currentGenerate_single_cx(int i);          //生成电流列表

private:
    QList<CrossPoint> crossPoint();      //得到所有的交点
    void powerlistGenerate();           //生成(初始化)电源列表
    void addConductor(int i,QList<CrossPoint> &crossPointList);     //添加一段导体入交点列表
    void addPoint(Point P,int i,QList<CrossPoint> &crossPointList);         //添加一点入交点列表


    mat matrixGenerate_GTT();       //端节点间的导纳矩阵
    mat matrixGenerate_GMM();       //中节点间的导纳矩阵
    mat matrixGenerate_GTM();       //端节点和中节点间的导纳矩阵
    vec currentGenerate();          //生成电流列表
    vec currentGenerate_single(int i);          //生成电流列表
    
};

struct CrossPoint
{
    CrossPoint(){isPower=false; }
    Point P;            //交点坐标
    QList<int> c_n;       //与交点直接相连的导体编号

    bool isPower;       //是否是电流源
    CurrentSource cs;   //接入的电流源
    int n;              //在交点列表中的编号

};

#endif // UNEQUAL_R_H
