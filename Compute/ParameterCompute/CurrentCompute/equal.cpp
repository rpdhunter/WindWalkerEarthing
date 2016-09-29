#include "equal.h"

#include "ui_computdialog.h"
#include "Compute/SpeedUp/speedup.h"
#include "Compute/DianzuCompute/dianzucompute.h"


Equal::Equal(QList<Conductor> list, ComputDialog *computeDlg) :
    ParameterComputeBase(list,computeDlg)
{    
}

void Equal::run()
{
    emit(currentName(tr("正在生成系数矩阵"),tr("4/9")));
    mat R = matrixGenerate(this->cutlist);           //生成系数矩阵
    emit(currentName(tr("正在求解矩阵"),tr("5/9")));
    double r = matrixSolve(R);              //系数矩阵求解,返回接地电阻

    emit outputTextAdd( tr("故障入地电流：%1A \n").arg(dlg->I_in) );
    emit outputTextAdd( tr("故障时地电位升高：%1V \n").arg(dlg->I_in * r) );
    emit outputTextAdd( tr("接地网的接地电阻：%1Ω \n").arg(r) );
}

mat Equal::matrixGenerate(QList<Conductor> cutlist)
{
    int n=cutlist.count();

    mat R(n,n);     //电阻系数矩阵

    deb(tr("开始初始化%1阶矩阵").arg(n) );
    wall_clock t;
    t.tic();


    //生成系数矩阵

    double rou = dlg->earth_resistivity.at(0);

    SpeedUp speedUp;    //加速算法类

    for(int i=0;i<n;i++){
        for(int j=i;j<n;j++){
            Conductor c1= cutlist.at(i);
            Conductor c2= cutlist.at(j);
            if(i == j){     //自阻
                double r = speedUp.lookup(c1);
                if(r!=-1){
                    R(i,j) = r;
                }
                else{
                    if(dlg->ui->radioButton_Dawalibi->isChecked()){         //dawalibi法
                        R(i,j) = DianZuCompute::ZiZu_Dawalibi_Half(c1,rou,100);
                    }
                    else if(dlg->ui->radioButton_ercifenyu->isChecked()){   //二次分域法
                        R(i,j) = DianZuCompute::ZiZu_SecSubdivision_Half(c1,rou,10);
                    }
                    else if(dlg->ui->radioButton_Jifen->isChecked()){       //积分法
                        R(i,j) = DianZuCompute::ZiZu_JiFen_Half(c1,rou,dlg->ui->spinBox_dianshu->value());
                    }
                    else if(dlg->ui->radioButton_Jihe->isChecked()){        //几何法
                        R(i,j) = DianZuCompute::ZiZu_JiHe_Half(c1,rou);
                    }
                    speedUp.addR(c1,R(i,j));
                }
            }
            else{          //互阻
                double r = speedUp.lookup(c1,c2);
                if(r!=-1){
                    R(i,j) = r;
                    R(j,i) = r;
                }
                else{
                    double r1=0,r2=0;
                    if(dlg->ui->radioButton_Dawalibi->isChecked()){         //dawalibi法
                        r1 = DianZuCompute::HuZu_Dawalibi_Half(c1,c2,rou,100);
                        r2 = DianZuCompute::HuZu_Dawalibi_Half(c2,c1,rou,100);
                    }
                    else if(dlg->ui->radioButton_ercifenyu->isChecked()){   //二次分域法(仍旧使用几何法计算互阻)
                        r1 = DianZuCompute::HuZu_JiHe_Half(c1,c2,rou);
                        r2 = DianZuCompute::HuZu_JiHe_Half(c2,c1,rou);
                    }
                    else if(dlg->ui->radioButton_Jifen->isChecked()){       //积分法
                        double a = DianZuCompute::HuZu_JiFen_Half(c1,c2,rou,dlg->ui->spinBox_dianshu->value());
                        r1 = a;
                        r2 = a;
                    }
                    else if(dlg->ui->radioButton_Jihe->isChecked()){        //几何法
                        r1 = DianZuCompute::HuZu_JiHe_Half(c1,c2,rou);
                        r2 = DianZuCompute::HuZu_JiHe_Half(c2,c1,rou);
                    }

                    double a;
                    if(qAbs(r1 - r2) >0.001){
                        //                        qDebug()<<tr("当前方法计算互阻错误:")<<i<<j<<r1<<r2;
                        //                        a = DianZuCompute::HuZu_JiFen_Half(c1,c2,rou,1000);     //误差较大时采用最稳定的积分法计算
                        a = DianZuCompute::HuZu_Dawalibi_Half(c1,c2,rou,10);
                        //                        qDebug()<<tr(" 积分法计算互阻:")<<a;
                        if(qAbs(r1-a)<qAbs(r2-a))
                            a=r1;
                        else
                            a=r2;
                    }
                    else{
                        a = (r1+r2)/2;   //在误差较小的情况下，进一步消除差异，保证电阻矩阵对称
                    }

                    speedUp.addR(c1,c2,a);
                    R(i,j) = a;
                    R(j,i) = a;
                }
            }
            emit progressValueChanged(i+1,n);
        }
    }

    speedUp.show(n); //显示优化信息


    R.save("系数矩阵.mat",raw_ascii);

    //    qDebug()<<tr("矩阵最大值")<<max(max(R));
    //    qDebug()<<tr("矩阵最小值")<<min(min(R));

    double n_secs1 = t.toc();
    deb(tr("初始化%1阶矩阵结束，用时%2秒").arg(n).arg(n_secs1) );
    return R;
}

double Equal::matrixSolve(mat R)
{
    //    mat R;
    //    R.load("系数矩阵.mat");

    int n = R.n_rows;   //获取矩阵维数

    mat A = ones<mat>(1,n);
    mat B(n+1,1);
    for(int i=0;i<n;i++){
        B(i,0) = -1 ;
    }
    B(n,0) = 0;

    mat RR = join_cols(R,A);
    RR = join_rows(RR,B);

    vec c(n+1,1);
    for(int i=0;i<n;i++){
        c(i) = 0 ;
    }
    c(n) = dlg->I_in;       //2013-10-20修改为正统的电流输入

    vec x = solve(RR, c);

    x.save("电流分布.mat",raw_ascii);

    //以下代码段，用于使输出具有统一接口，由于输出显示电流默认都是复数形式的
    vec Ir=x;
    Ir.resize(n);
    vec Im(n);
    Im.zeros();
    cx_vec I_cx(n);
    I_cx.set_real(Ir);
    I_cx.set_imag(Im);
    I_cx.save("电流分布cx",arma_ascii);

    //    mat XX = x;
    //    XX.resize(n,1);

    //    XX.reshape(thirdCut,n/thirdCut);      //用作生成测试电流分布
    //    XX.save("电流分布2.mat",raw_ascii);

    //    double Vg = x.at(n);    //地电位升高

    deb(tr("接地网的接地电阻：%1Ω").arg(x.at(n)/dlg->I_in) );
    return x.at(n)/dlg->I_in;
}
