#include "stepcompute.h"
#include <QtDebug>
#include <qmath.h>
#include <qwt_spline.h>

const double pi = 3.1415926;        //圆周率

StepCompute::StepCompute(QList<Conductor> list, ComputDialog *computeDlg)
    :ParameterComputeBase(list,computeDlg),PotentialCompute(list,computeDlg)
{
    STEPSIZE = 0.8;     //跨步电压的步长
}

//跨步电压
//算法：
//1.计算目标点P周围四个方向的跨步电压dx1,dx2,dy1,dy2
//2.根据坐标方向的跨步电压，计算每象限的最大跨步电压（调用step_tidu(double dx,double dy)函数）
//3.比较四个轴向跨步电压和四个象限最大跨步电压，得到最大跨步电压
//4.改进：为了充分利用地表电势数据，将跨步范围内的地表电势采样点也加入进来，参与最大跨步电压的比较，
//  不过度提高计算时间的同时，提高了计算准确性
//5.缺陷——这种做法是基于地表电势函数在STEPSIZE=0.8m的维度上连续这个基本假设上的，
//  如果变化过快（地网面积太小），精度会下降较大。完美的算法还需要探讨
void StepCompute::run()
{
    double x_space = dlg->ob_rect.width() / dlg->ob_xmesh;
    double y_space = dlg->ob_rect.height() / dlg->ob_ymesh;

    mat Step;

    if(x_space>STEPSIZE || y_space>STEPSIZE){
        Step = step1();
    }
    else{
        Step = step3();
    }

    //    mat Step = step2();
    Step.save("跨步电压.mat",raw_ascii);

    double m = max(max(Step));
    deb(tr("跨步电压计算完成！\t最大跨步电压：%1V ").arg(m));
    emit outputTextAdd(tr("最大跨步电压：%1V \n").arg(m));
}

//产生跨步电压的第一种方法
//算法简介：
//1.计算目标点P周围四个方向的跨步电压dx1,dx2,dy1,dy2
//2.根据坐标方向的跨步电压，计算每象限的最大跨步电压（调用step_tidu(double dx,double dy)函数）
//3.比较四个轴向跨步电压和四个象限最大跨步电压，得到最大跨步电压
//4.缺陷 —— 这种做法是基于地表电势函数在STEPSIZE=0.8m的维度上连续这个基本假设上的，
//  如果变化过快（地网面积太小），精度会下降较大。
mat StepCompute::step1()
{
    mat Potential;
    Potential.load("观测面电位.mat");

    mat Step = Potential;
    Step.zeros();

    double x_space = dlg->ob_rect.width() / dlg->ob_xmesh;
    double y_space = dlg->ob_rect.height() / dlg->ob_ymesh;

    int col = dlg->ob_xmesh+1;  //观测点密度，横向
    int row = dlg->ob_ymesh+1;  //观测点密度，纵向



    if(model() == equal || model() == unequal_r){
        vec I;
        I.load("电流分布.mat");
        for(int i=0;i<row;i++){
            for(int j=0;j<col;j++){
                double P_x = dlg->ob_rect.x() + j*x_space;
                double P_y = dlg->ob_rect.y() + i*y_space;
                double P_z = 0;
                Point P(P_x,P_y,P_z);    //计算点的坐标

                double fp = Potential(i,j);

                Step(i,j) = step_singlepoint_m1(P,cutlist,I,fp);
            }
            emit progressValueChanged(i+1,row-1);
        }
    }
    else if(model() == unequal_z_v1 || model() == unequal_z_v2){
        cx_vec I;
        I.load("电流分布.mat");
        for(int i=0;i<row;i++){
            for(int j=0;j<col;j++){
                double P_x = dlg->ob_rect.x() + j*x_space;
                double P_y = dlg->ob_rect.y() + i*y_space;
                double P_z = 0;
                Point P(P_x,P_y,P_z);    //计算点的坐标

                double fp = Potential(i,j);

                Step(i,j) = step_singlepoint_m1_cx(P,cutlist,I);
            }
            emit progressValueChanged(i+1,row-1);
        }
    }
    return Step;
}

//产生跨步电压的第二种方法
//算法简介：
//1.为了充分利用地表电势数据，将跨步范围内的地表电势采样点进行比较，可找出范围内的最大值和最小值，由此计算跨步电压
//2.缺陷 —— 这种做法是基于地表电势采样点充分密集的基础上的，如果采样点系数，将产生很大的误差，甚至不能得出结果
mat StepCompute::step2()
{
    mat Potential;
    Potential.load("观测面电位.mat");

    mat Step = Potential;
    Step.zeros();

    vec I;
    I.load("电流分布.mat");

    double x_space = dlg->ob_rect.width() / dlg->ob_xmesh;
    double y_space = dlg->ob_rect.height() / dlg->ob_ymesh;

    if(x_space>STEPSIZE || y_space>STEPSIZE){
        qDebug()<<tr("警告，由于观测面密度设置过低，跨步电压可能不精确");
    }

    int col = dlg->ob_xmesh+1;  //观测点密度，横向
    int row = dlg->ob_ymesh+1;  //观测点密度，纵向

    for(int i=0;i<row;i++){
        for(int j=0;j<col;j++){
            double P_x = dlg->ob_rect.x() + j*x_space;
            double P_y = dlg->ob_rect.y() + i*y_space;
            double P_z = 0;
            Point P(P_x,P_y,P_z);    //计算点的坐标

            double fp = Potential(i,j);
            Step(i,j) = step_singlepoint_m2(i,j,x_space,y_space,fp,Potential);
        }
        emit progressValueChanged(i+1,row-1);
    }
    return Step;
}

//产生跨步电压的第三种方法
//算法简介：
//1.为了充分利用地表电势数据，将跨步范围内的地表电势采样点进行比较，可找出范围内的最大值和最小值，由此计算跨步电压
//2.缺陷 —— 这种做法是基于地表电势采样点充分密集的基础上的，如果采样点系数，将产生很大的误差，甚至不能得出结果
mat StepCompute::step3()
{
    mat Potential;
    Potential.load("观测面电位.mat");

    mat Step = Potential;
    Step.zeros();

    splineMatrix = splineMatrixGenerate();

    double x_space = dlg->ob_rect.width() / dlg->ob_xmesh;
    double y_space = dlg->ob_rect.height() / dlg->ob_ymesh;
    double x_space_spline = dlg->ob_rect.width() / (splineMatrix.n_cols-1);
    double y_space_spline = dlg->ob_rect.height() / (splineMatrix.n_rows-1);

    int col = dlg->ob_xmesh+1;  //观测点密度，横向
    int row = dlg->ob_ymesh+1;  //观测点密度，纵向

    for(int i=0;i<row;i++){
        for(int j=0;j<col;j++){
            double x0 = dlg->ob_rect.x() + x_space*j;
            double y0 = dlg->ob_rect.y() + y_space*i;
            Point P0(x0,y0,0);  //生成所求点的空间坐标

            int n_spline_xmin = int ((x_space*j  - STEPSIZE) / x_space_spline) -1;  //跨步搜索矩形横坐标最小
            if(n_spline_xmin<0)
                n_spline_xmin = 0;
            int n_spline_ymin = int ((y_space*i - STEPSIZE) / y_space_spline) -1;  //跨步搜索矩形纵坐标最小
            if(n_spline_ymin<0)
                n_spline_ymin = 0;

            int n_spline_xmax = int ((x_space*j + STEPSIZE) / x_space_spline) +2;  //跨步搜索矩形横坐标最大
            if(n_spline_xmax > splineMatrix.n_cols)
                n_spline_xmax = splineMatrix.n_cols;
            int n_spline_ymax = int ((y_space*i + STEPSIZE) / y_space_spline) +2;  //跨步搜索矩形纵坐标最小
            if(n_spline_ymax > splineMatrix.n_rows)
                n_spline_ymax = splineMatrix.n_rows;

            Step(i,j) = step_singlepoint_m3(Potential(i,j),n_spline_xmin,n_spline_xmax,n_spline_ymin,n_spline_ymax,
                                            x_space_spline,y_space_spline,P0);

        }
        emit progressValueChanged(i+1,row);
    }
    return Step;
}


double StepCompute::step_singlepoint_m1(Point P, QList<Conductor> cutlist, vec I, double fp)
{
    Point P_fx2(P.x+STEPSIZE,P.y,P.z);
    Point P_fx1(P.x-STEPSIZE,P.y,P.z);
    Point P_fy2(P.x,P.y+STEPSIZE,P.z);
    Point P_fy1(P.x,P.y-STEPSIZE,P.z);


    double fx2 = conductorsPotential(cutlist,P_fx2,I);
    double fx1 = conductorsPotential(cutlist,P_fx1,I);
    double fy2 = conductorsPotential(cutlist,P_fy2,I);
    double fy1 = conductorsPotential(cutlist,P_fy1,I);

    double dx2 = fx2-fp;
    double dx1 = fx1-fp;
    double dy2 = fy2-fp;
    double dy1 = fy1-fp;

    //四个轴向跨步电压和四个象限最大跨步电压（根据梯度估算）
    vec s;
    s << step_tidu(dx2,dy2) << step_tidu(dy2,dx1) << step_tidu(dx1,dy1) << step_tidu(dy1,dx2)
      << dx2 << dx1 << dy2 << dy1;

    s = abs(s);
    return s.max();
}

double StepCompute::step_singlepoint_m1_cx(Point P, QList<Conductor> cutlist, cx_vec I)
{
    Point P_fx2(P.x+STEPSIZE,P.y,P.z);
    Point P_fx1(P.x-STEPSIZE,P.y,P.z);
    Point P_fy2(P.x,P.y+STEPSIZE,P.z);
    Point P_fy1(P.x,P.y-STEPSIZE,P.z);

    cx_double fx2 = conductorsPotential(cutlist,P_fx2,I);
    cx_double fx1 = conductorsPotential(cutlist,P_fx1,I);
    cx_double fy2 = conductorsPotential(cutlist,P_fy2,I);
    cx_double fy1 = conductorsPotential(cutlist,P_fy1,I);
    cx_double fp  = conductorsPotential(cutlist,P,I);

    cx_double dx2 = fx2-fp;
    cx_double dx1 = fx1-fp;
    cx_double dy2 = fy2-fp;
    cx_double dy1 = fy1-fp;

    //四个轴向跨步电压和四个象限最大跨步电压（根据梯度估算）
    cx_vec s;
    s << step_tidu_cx(dx2,dy2) << step_tidu_cx(dy2,dx1) << step_tidu_cx(dx1,dy1) << step_tidu_cx(dy1,dx2)
      << dx2 << dx1 << dy2 << dy1;

    vec sr = sqrt(pow(real(s),2) + pow(imag(s),2));
    return sr.max();
}

double StepCompute::step_singlepoint_m2(int i, int j, double x_space, double y_space, double fp, mat Potential)
{
    int col = dlg->ob_xmesh+1;  //观测点密度，横向
    int row = dlg->ob_ymesh+1;  //观测点密度，纵向
    //加入范围内的采样点数据，共同比较
    int n_x = STEPSIZE / x_space;   //x方向上，在跨步范围内的网格数(恒为正)
    int n_y = STEPSIZE / y_space;   //x方向上，在跨步范围内的网格数(恒为正)
    double result = 0;
    if(n_x > 0 && n_y > 0){     //这时在跨步范围内有数据点
        for(int k=-n_x;k<n_x;k++){
            if(j+k>=0 && j+k<col){
                int a = qSqrt(qPow(STEPSIZE,2) - qPow(x_space*k,2)) / y_space;  //(恒为正)
                for(int l=-a;l<a;l++){
                    if(i+l>=0 && i+l<row){
                        double temp = abs(Potential(i+l,j+k) - fp);
                        if(temp>result)
                            result = temp;
                    }
                }
            }
        }
    }
    return result;
}

double StepCompute::step_singlepoint_m3(double fp, int n_spline_xmin, int n_spline_xmax, int n_spline_ymin, int n_spline_ymax, double x_space_spline, double y_space_spline, Point P0)
{
    vec tempVec(1);
    tempVec(0)=fp;

    for(int i_spline=n_spline_ymin;i_spline<n_spline_ymax;i_spline++){
        for(int j_spline=n_spline_xmin;j_spline<n_spline_xmax;j_spline++){
            double x =  dlg->ob_rect.x() + x_space_spline*j_spline;
            double y =  dlg->ob_rect.y() + y_space_spline*i_spline;
            Point P(x,y,0); //生成插值点的空间坐标
            if(Point::length(P0,P) <= STEPSIZE){
                rowvec r(1);
                r(0)=splineMatrix(i_spline,j_spline);
                tempVec.insert_rows(tempVec.n_rows-1,r);
            }
        }
    }
    double maxV = max(tempVec);     //跨步内的最大值
    double minV = min(tempVec);     //跨步内的最小值

    double s = maxV-fp > fp-minV ? maxV-fp : fp-minV ;
    return s;
}

double StepCompute::step_tidu(double dx2, double dy2)
{
    if(dx2*dy2 >=0 ){
        double ank;
        if(dx2 == 0){
            ank = pi/2;
        }
        else{
            ank = qAtan(dy2/dx2);      //梯度的角度
        }
        return dx2*qCos(ank)/2 + dy2*qSin(ank)/2;
    }
    else{
        return 0;
    }
}

cx_double StepCompute::step_tidu_cx(cx_double dx2, cx_double dy2)
{
    double a = step_tidu(dx2.real(),dy2.real());
    double b = step_tidu(dx2.imag(),dy2.imag());

    return cx_double(a,b);
}

//三次样条插值函数
vec StepCompute::interpolate(const vec x, const vec y, const int numValues)
{
    QwtSpline spline;
    QPolygonF points;
    for(uint i=0;i<x.n_rows;i++){
        points.append(QPointF(x.at(i),y.at(i)));
    }

    vec interpolatedVec(numValues);
    if ( !spline.setPoints(points) ){
        interpolatedVec.zeros();
        qDebug()<<tr("插值失败");
        return interpolatedVec;
    }

    const double delta = (points.last().x() - points.first().x()) / (numValues - 1);    //点和点之间的距离

    for(int i = 0; i < numValues; i++)  // interpolate
    {
        const double x = points[0].x() + i * delta;
        interpolatedVec[i] = spline.value(x);
    }
    return interpolatedVec;
}

//插值矩阵生成函数
mat StepCompute::splineMatrixGenerate()
{
    double STEPSIZE = 0.8;  //设置步长为0.8米

    double x_space = dlg->ob_rect.width() / dlg->ob_xmesh;
    double y_space = dlg->ob_rect.height() / dlg->ob_ymesh;



    int numValues_x;
    int numValues_y;

    if( x_space/STEPSIZE > 0.1 ){
        numValues_x = int(dlg->ob_rect.width() / (STEPSIZE / 10)) + 1;
    }
    else{
        numValues_x = dlg->ob_xmesh + 1;
    }

    if( y_space/STEPSIZE > 0.1 ){
        numValues_y = int(dlg->ob_rect.height() / (STEPSIZE / 10)) + 1;
    }
    else{
        numValues_y = dlg->ob_ymesh + 1;
    }


    mat Potential;
    Potential.load("观测面电位.mat");

    //生成第一次样条插值的横坐标（采用先横插后纵插的方式）
    vec X_vec(dlg->ob_xmesh+1);
    for(int i=0;i<dlg->ob_xmesh+1;i++){
        X_vec(i) = dlg->ob_rect.x()+i*x_space;
    }
    vec Y_vec(dlg->ob_ymesh+1);
    for(int i=0;i<dlg->ob_ymesh+1;i++){
        Y_vec(i) = dlg->ob_rect.y()+i*y_space;
    }

    mat splineMatrix1;  //插值矩阵
    //一次插值（行插值）
    for(int i=0;i<dlg->ob_ymesh + 1;i++){
        vec interpolateVec = interpolate(X_vec,Potential.row(i).t(),numValues_x);
        splineMatrix1.insert_rows(i,interpolateVec.t());
        emit progressValueChanged(i+1,dlg->ob_ymesh + 1);
    }

    //    splineMatrix1.save("splineMatrix1",raw_ascii);

    mat splineMatrix2;  //插值矩阵
    //二次差值（列插值）
    for(uint i=0;i<splineMatrix1.n_cols;i++){
        vec interpolateVec = interpolate(Y_vec,splineMatrix1.col(i),numValues_y);
        splineMatrix2.insert_cols(i,interpolateVec);
        emit progressValueChanged(i+1,splineMatrix1.n_cols);
    }

    //    splineMatrix2.save("splineMatrix2",raw_ascii);

    return splineMatrix2;
}
