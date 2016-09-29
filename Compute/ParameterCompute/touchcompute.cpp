#include "touchcompute.h"
#include "ui_computdialog.h"


TouchCompute::TouchCompute(QList<Conductor> list, ComputDialog *computeDlg)
    :ParameterComputeBase(list,computeDlg)
{    
}

//接触电压主接口
//注意这里的接触电压应当为正数(与CDEGS相一致)
void TouchCompute::run()
{
    //计算接触电压
    if(model() == equal){
        touch_Equal();
    }
    else if(model() == unequal_r){
        touch_UnEqual_R();
    }
    else if(model() == unequal_z_v1 || model() == unequal_z_v2){
        touch_UnEqual_Z();
    }

    //计算围墙内接触电压
    touchIn();
}

//围墙内接触电压
void TouchCompute::touchIn()
{
    mat touch_In;
    touch_In.load("接触电压.mat");

    double x_space = dlg->ob_rect.width()*1.0/(touch_In.n_cols-1);
    double y_space = dlg->ob_rect.height()*1.0/(touch_In.n_rows-1);



    for(uint i=0;i<touch_In.n_rows;i++){
        for(uint j=0;j<touch_In.n_cols;j++){
            QPointF point;
            point.setY(dlg->ob_rect.top() + y_space*i);
            point.setX(dlg->ob_rect.left() + x_space*j);
            if(!dlg->weiqiang.containsPoint(point,Qt::OddEvenFill)){
                touch_In(i,j) = 0;
            }
        }
        emit progressValueChanged(i+1,touch_In.n_rows-1);
    }
    touch_In.save("围墙内接触电压.mat",raw_ascii);
    //    touch_In.save("围墙内接触电压.mat");
    deb(tr("围墙内接触电压计算完成！\t围墙内最大接触电压：%1V").arg(max(max(touch_In))) );

    emit outputTextAdd(tr("围墙内最大接触电压：%1V \n").arg(max(max(touch_In))) );

    deb(tr("******************************本次计算完成！******************************"));
}

//读取节点电压
void TouchCompute::readVoltage()
{
    voltageList.clear();

    QString fileName = dlg->currentDir + "/" + tr("节点电压.txt");
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;
    QTextStream in(&file);    // read the data serialized from the file

    QString string;

    while(!in.atEnd())
    {
        string=in.readLine();
        CurrentSource cs;
        cs.input(string);
        voltageList.append(cs);

    };

//    int n = this->cutlist.count();

//    voltageList.

}

void TouchCompute::touch_Equal()
{
    mat Potential;
    Potential.load("观测面电位.mat");

    vec I;
    I.load("电流分布.mat");
    int n = I.n_rows - 1;
    double GPR = I.at(n);   //地电位升
//    double GPR = R*1;     //地电位升

    mat touch;
    touch = abs(GPR - Potential);       //注意这里为了防止出现不合常理的情况，来掩饰误差，强硬使用了abs函数，实质上是CDEGS的一个小技巧
//    touch = GPR - Potential;                //由于明白了产生问题的根源，我的程序里也就不必要掩饰这个问题了（不过在最后的成品程序中，优化和计算两种模式可能使用两个不同的函数）
    touch.save("接触电压.mat",raw_ascii);
}

void TouchCompute::touch_UnEqual_R()
{
    readVoltage();

    mat Potential;
    Potential.load("观测面电位.mat");

    int col = Potential.n_cols;
    int row = Potential.n_rows;

    double x_space = dlg->ob_rect.width() / dlg->ob_xmesh;
    double y_space = dlg->ob_rect.height() / dlg->ob_ymesh;

    mat GPR(row,col);
    for(int i=0;i<row;i++){
        for(int j=0;j<col;j++){
            double P_x = dlg->ob_rect.x() + j*x_space;
            double P_y = dlg->ob_rect.y() + i*y_space;
            double P_z = 0;
            Point P(P_x,P_y,P_z);    //计算点的坐标
            GPR(i,j) = getNearestGPR(P);
        }
    }


    mat touch;
    touch = abs(GPR - Potential);       //注意这里为了防止出现不合常理的情况，来掩饰误差，强硬使用了abs函数，实质上是CDEGS的一个小技巧
//    touch = GPR - Potential;                //由于明白了产生问题的根源，我的程序里也就不必要掩饰这个问题了（不过在最后的成品程序中，优化和计算两种模式可能使用两个不同的函数）
    touch.save("接触电压.mat",raw_ascii);
}

void TouchCompute::touch_UnEqual_Z()
{
    readVoltage();

    cx_mat Potential;
    Potential.load("观测面电位cx");

    int col = Potential.n_cols;
    int row = Potential.n_rows;

    double x_space = dlg->ob_rect.width() / dlg->ob_xmesh;
    double y_space = dlg->ob_rect.height() / dlg->ob_ymesh;

    cx_mat GPR(row,col);
    for(int i=0;i<row;i++){
        for(int j=0;j<col;j++){
            double P_x = dlg->ob_rect.x() + j*x_space;
            double P_y = dlg->ob_rect.y() + i*y_space;
            double P_z = 0;
            Point P(P_x,P_y,P_z);    //计算点的坐标
            GPR(i,j) = getNearestGPR_cx(P);
        }
    }


    cx_mat touch_cx;

//    touch = abs(GPR - Potential);       //注意这里为了防止出现不合常理的情况，来掩饰误差，强硬使用了abs函数，实质上是CDEGS的一个小技巧
    touch_cx = GPR - Potential;                //由于明白了产生问题的根源，我的程序里也就不必要掩饰这个问题了（不过在最后的成品程序中，优化和计算两种模式可能使用两个不同的函数）

    mat A = real(touch_cx);
    mat B = imag(touch_cx);

    mat touch = sqrt(A%A + B%B);

    touch.save("接触电压.mat",raw_ascii);
}

double TouchCompute::getNearestGPR(Point P)
{
    double l = Point::length(P,voltageList.value(0).P);
    int n = 0;
    for(int i=1;i<voltageList.count();i++){
        double l1 = Point::length(P,voltageList.value(i).P);
        if(l1<l){
            l = l1;
            n = i;
        }
    }
    return voltageList.value(n).norm();
}

cx_double TouchCompute::getNearestGPR_cx(Point P)
{
    double l = Point::length(P,voltageList.value(0).P);
    int n = 0;
    for(int i=1;i<voltageList.count();i++){
        double l1 = Point::length(P,voltageList.value(i).P);
        if(l1<l){
            l = l1;
            n = i;
        }
    }
    return voltageList.value(n).current();
}
