#include "potentialcompute.h"
#include "../DianzuCompute/dawalibi.h"

PotentialCompute::PotentialCompute(QList<Conductor> list, ComputDialog *computeDlg)
    :ParameterComputeBase(list,computeDlg)
{
}

void PotentialCompute::run()
{
    if(model()==equal || model()==unequal_r){
        potentialGenerate();
    }
    else if(model()==unequal_z_v1 || model()==unequal_z_v2){
        potentialGenerate_cx();
    }
}

//计算某段导体在某一点的电位
//注释掉的是同一种方法，由于后期编写了专门的Dawalibi类，故直接采用
double PotentialCompute::conductorPotential(Conductor c1, Point P, double I)
{
    Conductor c2;
    c2 = c1.image();

    Dawalibi dawa(c1),_dawa(c2);
    double Vp = dawa.Vp(P)+_dawa.Vp(P);

//    Point A(c1.x0,c1.y0,c1.z0);
//    Point B(c1.x1,c1.y1,c1.z1);
//    Point _A(c2.x0,c2.y0,c2.z0);
//    Point _B(c2.x1,c2.y1,c2.z1);

//    double AB = Point::length(A,B);
//    double AP = Point::length(A,P);
//    double BP = Point::length(B,P);
//    double _AP = Point::length(_A,P);
//    double _BP = Point::length(_B,P);

//    double cos11 = (AP*AP + AB*AB - BP*BP) / (2*AP*AB);
//    double cos12 = (_AP*_AP + AB*AB - _BP*_BP) / (2*_AP*AB);
//    double cos21 = (BP*BP + AB*AB - AP*AP) / (2*BP*AB);
//    double cos22 = (_BP*_BP + AB*AB - _AP*_AP) / (2*_BP*AB);

//    double Vp = log(BP*_BP*(1+cos21)*(1+cos22)) - log(AP*_AP*(1-cos11)*(1-cos12)) ;

//    Vp = dlg->earth_resistivity.at(0) * I * Vp / (4 * AB);

//    Vp = Vp / pi;

    return dlg->earth_resistivity.at(0) * I * Vp;
}

cx_double PotentialCompute::conductorPotential(Conductor c1, Point P, cx_double I)
{
    double re = conductorPotential(c1,P,I.real());
    double im = conductorPotential(c1,P,I.imag());
    return Complex(re,im);
}

//计算导体组在某一点产生的电位(因为代码重用率高而设定的简化函数)
double PotentialCompute::conductorsPotential(QList<Conductor> cutlist, Point P, vec I)
{
    double result = 0;
    for(int k=0;k<cutlist.count();k++){
        result += conductorPotential(cutlist.at(k),P,I.at(k));
    }
    return result;
}

cx_double PotentialCompute::conductorsPotential(QList<Conductor> cutlist, Point P, cx_vec I)
{
    cx_double result(0,0);
    for(int k=0;k<cutlist.count();k++){
        result += conductorPotential(cutlist.at(k),P,I.at(k));
    }
    return result;
}

//电位分布计算
void PotentialCompute::potentialGenerate()
{
    vec I;

    I.load("电流分布.mat");

    int col = dlg->ob_xmesh+1;  //观测点密度，横向
    int row = dlg->ob_ymesh+1;  //观测点密度，纵向

    double x_space = dlg->ob_rect.width() / dlg->ob_xmesh;
    double y_space = dlg->ob_rect.height() / dlg->ob_ymesh;

    mat Potential(row,col);     //结果矩阵

    for(int i=0;i<row;i++){
        for(int j=0;j<col;j++){
            double P_x = dlg->ob_rect.x() + j*x_space;
            double P_y = dlg->ob_rect.y() + i*y_space;
            double P_z = 0;
            Point P(P_x,P_y,P_z);    //计算点的坐标

            Potential(i,j) = conductorsPotential(cutlist,P,I);
            //            qDebug()<<tr("Potential(%1,%2)").arg(i).arg(j)<<Potential(i,j);
        }
        //        emit progressValueChanged((i+1)*100.0/(MAX_POINT-1));
        emit progressValueChanged(i+1,row-1);
    }


    Potential.save("观测面电位.mat",raw_ascii);
    //    Potential.save("观测面电位.mat");
}

void PotentialCompute::potentialGenerate_cx()
{
    cx_vec I;

    I.load("电流分布cx",arma_ascii);



    int col = dlg->ob_xmesh+1;  //观测点密度，横向
    int row = dlg->ob_ymesh+1;  //观测点密度，纵向

    double x_space = dlg->ob_rect.width() / dlg->ob_xmesh;
    double y_space = dlg->ob_rect.height() / dlg->ob_ymesh;

    cx_mat Potential(row,col);     //结果矩阵

    for(int i=0;i<row;i++){
        for(int j=0;j<col;j++){
            double P_x = dlg->ob_rect.x() + j*x_space;
            double P_y = dlg->ob_rect.y() + i*y_space;
            double P_z = 0;
            Point P(P_x,P_y,P_z);    //计算点的坐标

            Potential(i,j) = conductorsPotential(cutlist,P,I);

        }
        emit progressValueChanged(i+1,row-1);
    }

    Potential.save("观测面电位cx",arma_ascii);

    mat A = real(Potential);
    mat B = imag(Potential);

    mat P = sqrt(A%A + B%B);

    P.save("观测面电位.mat",raw_ascii);
}
