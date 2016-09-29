#ifndef SPEEDUP_H
#define SPEEDUP_H

#include "Base/conductor.h"
#include "conductorpair.h"
#include <QMap>
class QPointF;

//计算电阻矩阵的优化加速类
//基本思想是将导体信息经过坐标变换后，以精简和唯一的形式保存到类中
//如果下次遇到相同的导体组（坐标变换后），则不经计算直接使用存储值
//函数配备了两个查询接口和两个数据添加接口
//类的数据存储形式为QMap，核心运作方式是跳跃表（skip-list）
//示例：当计算c1和c2互阻时，先调用lookup(c1,c2)，如果返回值不为-1，则返回值就是互阻
//     如果返回值为-1，则使用正常的计算函数计算出c1,c2的互阻，然后调用addR(c1,c2,R)
//     添加这对数据到类数据中
class SpeedUp
{
public:
    SpeedUp();

    double lookup(Conductor c1,Conductor c2);
    double lookup(Conductor c1);

    void addR(Conductor c1,Conductor c2,double R);
    void addR(Conductor c1,double R);

    void show(int n);

    void trans(Conductor &c1,Conductor &c2);  //坐标变换
    void trans(Conductor &c1);  //坐标变换


private:
    QMap<ConductorPair,double> zizulist;    //自阻数据
    QMap<ConductorPair,double> huzulist;    //互阻数据    

    QPointF pointTrans(QPointF P0, double dx, double dy, double sin, double cos);   //点的坐标变换
    void c2Trans(Conductor &c2);    //c2的轴对称变换
    void c2Trans_x(Conductor &c2);
    void c2Trans_y(Conductor &c2);
};

#endif // SPEEDUP_H
