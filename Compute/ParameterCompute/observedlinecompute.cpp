#include "observedlinecompute.h"

ObservedLineCompute::ObservedLineCompute(QList<Conductor> list, ComputDialog *computeDlg)
    :ParameterComputeBase(list,computeDlg),StepCompute(list,computeDlg),TouchCompute(list,computeDlg)
{
    readVoltage();      //读取节点电压
}

void ObservedLineCompute::run()
{
    if(model() == equal){
        observedLine();
    }
    else if(model() == unequal_r){
        observedLine_R();
    }
    else if(model() == unequal_z_v1 || model() == unequal_z_v2){
        observedLine_Z();
    }

}

//电位分布计算
void ObservedLineCompute::observedLine()
{
    vec I;
    I.load("电流分布.mat");

    QDir temp;
    temp.mkdir(dlg->currentDir + "/" + tr("观测线"));
    QDir::setCurrent(dlg->currentDir + "/" + tr("观测线"));

    foreach(ObservedLine line,dlg->observedLineList){
        if(line.isValid()){
            mat obLines(4,line.subsection()+1);
            obLines.zeros();
            double x_interval = (line.P2().x - line.P1().x) / line.subsection();
            double y_interval = (line.P2().y - line.P1().y) / line.subsection();
            double z_interval = (line.P2().z - line.P1().z) / line.subsection();
            for(int i=0;i<line.subsection()+1;i++){
                double P_x = line.P1().x + i*x_interval;
                double P_y = line.P1().y + i*y_interval;
                double P_z = line.P1().z + i*z_interval;
                Point P(P_x,P_y,P_z);
                obLines(0,i) = conductorsPotential(cutlist,P,I);    //观测线电位
            }
            if(line.P2().z == 0 && line.P1().z == 0){        //观测线在地表

                double GPR = I.at(I.n_rows-1);  //地电位升
                for(int i=0;i<line.subsection()+1;i++){
                    double P_x = line.P1().x + i*x_interval;
                    double P_y = line.P1().y + i*y_interval;
                    Point P(P_x,P_y,0);

                    //观测线接触电压
                    obLines(1,i) = qAbs(GPR - obLines(0,i));

                    //观测线围墙内接触电压
                    if(dlg->weiqiang.containsPoint(QPointF(P_x,P_y),Qt::OddEvenFill)){
                        obLines(2,i) = obLines(1,i);
                    }
                    else{
                        obLines(2,i) = 0;
                    }

                    //观测线上跨步电压(只采用了第一种计算方法)
                    obLines(3,i) = step_singlepoint_m1(P,cutlist,I,obLines(0,i));

                }
            }

            //保存
            std::string a = line.name().toStdString() + ".mat";
            obLines.save(a ,raw_ascii);
            qDebug()<<tr("观测线%1计算完毕").arg(line.name());
        }
    }

    QDir::setCurrent(dlg->currentDir);
}

void ObservedLineCompute::observedLine_R()
{
    vec I;
    I.load("电流分布.mat");

    QDir temp;
    temp.mkdir(dlg->currentDir + "/" + tr("观测线"));
    QDir::setCurrent(dlg->currentDir + "/" + tr("观测线"));

    foreach(ObservedLine line,dlg->observedLineList){
        if(line.isValid()){
            mat obLines(4,line.subsection()+1);
            obLines.zeros();
            double x_interval = (line.P2().x - line.P1().x) / line.subsection();
            double y_interval = (line.P2().y - line.P1().y) / line.subsection();
            double z_interval = (line.P2().z - line.P1().z) / line.subsection();
            for(int i=0;i<line.subsection()+1;i++){
                double P_x = line.P1().x + i*x_interval;
                double P_y = line.P1().y + i*y_interval;
                double P_z = line.P1().z + i*z_interval;
                Point P(P_x,P_y,P_z);
                obLines(0,i) = conductorsPotential(cutlist,P,I);    //观测线电位
                obLines(1,i) = getNearestGPR(P);                    //观测点上的GPR
            }

            if(line.P2().z == 0 && line.P1().z == 0){        //观测线在地表

                for(int i=0;i<line.subsection()+1;i++){
                    double P_x = line.P1().x + i*x_interval;
                    double P_y = line.P1().y + i*y_interval;
                    Point P(P_x,P_y,0);

                    //观测线接触电压
                    obLines(1,i) = qAbs(obLines(1,i) - obLines(0,i));

                    //观测线围墙内接触电压
                    if(dlg->weiqiang.containsPoint(QPointF(P_x,P_y),Qt::OddEvenFill)){
                        obLines(2,i) = obLines(1,i);
                    }
                    else{
                        obLines(2,i) = 0;
                    }

                    //观测线上跨步电压(只采用了第一种计算方法)
                    obLines(3,i) = step_singlepoint_m1(P,cutlist,I,obLines(0,i));

                }
            }

            //保存
            std::string a = line.name().toStdString() + ".mat";
            obLines.save(a ,raw_ascii);
            qDebug()<<tr("观测线%1计算完毕").arg(line.name());
        }
    }

    QDir::setCurrent(dlg->currentDir);
}

void ObservedLineCompute::observedLine_Z()
{
    cx_vec I;
    I.load("电流分布.mat");

    QDir temp;
    temp.mkdir(dlg->currentDir + "/" + tr("观测线"));
    QDir::setCurrent(dlg->currentDir + "/" + tr("观测线"));

    foreach(ObservedLine line,dlg->observedLineList){
        if(line.isValid()){
            mat obLines(4,line.subsection()+1);
            obLines.zeros();
            double x_interval = (line.P2().x - line.P1().x) / line.subsection();
            double y_interval = (line.P2().y - line.P1().y) / line.subsection();
            double z_interval = (line.P2().z - line.P1().z) / line.subsection();
            for(int i=0;i<line.subsection()+1;i++){
                double P_x = line.P1().x + i*x_interval;
                double P_y = line.P1().y + i*y_interval;
                double P_z = line.P1().z + i*z_interval;
                Point P(P_x,P_y,P_z);
                cx_double aa = conductorsPotential(cutlist,P,I);
                obLines(0,i) = sqrt(pow(aa.real(),2) + pow(aa.imag(),2));    //观测线电位
                aa = getNearestGPR_cx(P);
                obLines(1,i) =  sqrt(pow(aa.real(),2) + pow(aa.imag(),2));   //观测点上的GPR
            }

            if(line.P2().z == 0 && line.P1().z == 0){        //观测线在地表

                for(int i=0;i<line.subsection()+1;i++){
                    double P_x = line.P1().x + i*x_interval;
                    double P_y = line.P1().y + i*y_interval;
                    Point P(P_x,P_y,0);

                    //观测线接触电压
                    obLines(1,i) = qAbs(obLines(1,i) - obLines(0,i));

                    //观测线围墙内接触电压
                    if(dlg->weiqiang.containsPoint(QPointF(P_x,P_y),Qt::OddEvenFill)){
                        obLines(2,i) = obLines(1,i);
                    }
                    else{
                        obLines(2,i) = 0;
                    }

                    //观测线上跨步电压(只采用了第一种计算方法的复数模式)
                    double s_max1 = step_singlepoint_m1_cx(P,cutlist,I);
                    obLines(3,i) = s_max1;

                }
            }

            //保存
            std::string a = line.name().toStdString() + ".mat";
            obLines.save(a ,raw_ascii);
            qDebug()<<tr("观测线%1计算完毕").arg(line.name());
        }
    }

    QDir::setCurrent(dlg->currentDir);
}
