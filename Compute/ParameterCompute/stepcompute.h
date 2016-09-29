#ifndef STEPCOMPUTE_H
#define STEPCOMPUTE_H

#include "potentialcompute.h"
#include "armadillo"

using namespace arma;

class StepCompute : public PotentialCompute
{
public:
    StepCompute(QList<Conductor> list, ComputDialog *computeDlg);

    void run();     //计算跨步电压

protected:
    double step_singlepoint_m1(Point P,QList<Conductor> cutlist, vec I, double fp);
    double step_singlepoint_m1_cx(Point P, QList<Conductor> cutlist, cx_vec I);

    double step_singlepoint_m2(int i,int j,double x_space,double y_space,double fp,mat Potential);
    double step_singlepoint_m3(double fp, int n_spline_xmin, int n_spline_xmax, int n_spline_ymin, int n_spline_ymax,
                               double x_space_spline, double y_space_spline, Point P0);

private:
    //跨步电压相关函数

    mat step1();        //产生跨步电压的第一种方法
    mat step2();        //产生跨步电压的第二种方法
    mat step3();        //产生跨步电压的第三种方法

    mat splineMatrix;
    double STEPSIZE;    //计算跨步电压的步长（一般为0.8m）
    int thirdCut;       //三次切割数目(从ui来)



    double step_tidu(double dx2,double dy2);
    cx_double step_tidu_cx(cx_double dx2,cx_double dy2);
    vec interpolate(const vec x, const vec y, const int numValues);      //一维插值函数
    mat splineMatrixGenerate();     //插值矩阵生成函数
};

#endif // STEPCOMPUTE_H
