#ifndef SECOUNDSUBDIVISION_H
#define SECOUNDSUBDIVISION_H

#include <Base/conductor.h>
#include <QPointF>
#include <QList>
#include "armadillo"



//二次分域法求解自电阻（因为单独涉及到一套算法，所以单独列成类）
class SecoundSubdivision
{
public:
    SecoundSubdivision(Conductor c,int n);

    double zizu();      //返回二次分域法求得的自电阻
    double huzu(Conductor c2);  //返回二次分域法求得的互电阻
    double Vp(Point P);     //返回导体在P点的电位值
    
    void test();


private:
    Conductor c;
    QPointF P0,P1,_P0,_P1;  //需要求自电阻的导体坐标
    double l;   //导体长度
    double r;   //导体半径
    int n;      //导体切割段数
    QList<QPointF> cutlist_2d;     //切割后每段导体的中心点坐标(2d)
    QList<QPointF> _cutlist_2d;    //切割后每段导体的中心点坐标的镜像

    QList<Conductor> cutlist;       //切割后的导体

    arma::vec X;      //电流分布计算结果和自电阻（矩阵求解后才有意义）
    void matrixCompute();   //求解二次分域矩阵的主函数（最耗时的计算）

    double subZizu(QPointF P); //求解系数矩阵自电阻
    double subHuzu(QPointF P, QPointF Q); //求解系数矩阵互电阻
    double Green_ChenXianLu(double R, double Z);   //陈先璐给出的格林函数（带贝塞尔函数的积分公式）

    double first_modified_Bessel(int n,double x);    //第一类修正贝塞尔曲线
    double second_modified_Bessel(int n,double x);   //第二类修正贝塞尔曲线
    double F(double x,double a,double b,double r,double z);    //微段积分函数
    double asr(double aa,double bb,double eps,double a,double b,double r,double z);        //自适应simpson公式主过程
    double asr(double aa,double bb,double eps,double A,double a,double b,double r,double z);   //自适应simpson公式递归过程
    double simpson(double width,double fa,double fb,double fc);  //三点辛普森公式

};

#endif // SECOUNDSUBDIVISION_H
