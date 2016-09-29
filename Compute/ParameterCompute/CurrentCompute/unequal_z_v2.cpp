#include "unequal_z_v2.h"
#include "Base/compute.h"
#include "Compute/DianzuCompute/heppe.h"
#include "Compute/SpeedUp/speedupcx.h"

const double pi = 3.1415926;        //圆周率
const double MINVALUE = 0.000001;   //最小值（用于比较实数和零，如果小于这个数明则认为相等）

UnEqual_Z_V2::UnEqual_Z_V2(QList<Conductor> list, ComputDialog *computeDlg) :
    UnEqual_R(list,computeDlg)
{
}

void UnEqual_Z_V2::run()
{
    ////////////////////////////////////////
    /// \矩阵生成
    ////////////////////////////////////////

    //在分组基础上再切割
    Compute compute;
    QList<Conductor> datalist = compute.thirdCut(this->cutlist,2);

    mat A(n+m,2*n);
    A.zeros();
    for(int i=0;i<n;i++){   //中点关联矩阵赋值
        A(i,2*i)=1;
        A(i,2*i+1)=1;
    }
    for(int i=0;i<m;i++){   //交点关联矩阵赋值
        CrossPoint c = crossPointList.at(i);
        for(int j=0;j<c.c_n.count();j++){       //在关联的导体上才和邻近的中点有互电导，否则为0
            int k = c.c_n.at(j);
            if(c.P == datalist.value(2*k).P0() || c.P == datalist.value(2*k).P1() )
                A(n+i,2*k) = -1;
            if(c.P == datalist.value(2*k+1).P0() || c.P == datalist.value(2*k+1).P1() )
                A(n+i,2*k+1) = -1;
        }
    }
    cx_mat Y = inv(matrixGenerate_Z(datalist));
    cx_mat G = A*Y*strans(A);
    mat Grr = inv(matrixGenerate(this->cutlist));
    mat G12 = zeros(n,m);
    mat G21 = zeros(m,n);
    mat G22 = zeros(m,m);
    mat T1 = join_rows(Grr,G12);
    mat T2 = join_rows(G21,G22);
    G = G + join_cols(T1,T2);

    ////////////////////////////////////////
    /// \矩阵求解
    ////////////////////////////////////////
    deb( tr("求解%1阶矩阵开始").arg(n+m) );
    cx_mat II(m+n,powerlist.count());
    cx_mat vv(m+n,powerlist.count());      //vv用于存储不同电流注入点单独注入时产生的响应
    for(int i=0;i<powerlist.count();i++){
        II.col(i) = currentGenerate_single_cx(i);
        vv.col(i) = solve(G,II.col(i));
    }

    II.save("II",arma_ascii);
    vv.save("vv",arma_ascii);
    cx_vec v = sum(vv,1);


    ////////////////////////////////////////
    /// \保存结果
    ////////////////////////////////////////
    savePointVoltageToFile(v,crossPointList);   //保存节点电压结果

    for(int i=0;i<powerlist.count();i++){
        int k = powerlist.value(i).n +n;
        CurrentSource aa;
        aa.setCurrent( vv(k,i)/II(k,i) );
        QString str = tr("不等电位注入点%1(%2)接地电阻：%3∠%4°Ω").arg(i+1).arg(powerlist.value(i).cs.remarks)
                .arg(aa.norm() ).arg(aa.arg() );
        deb(str);
        emit outputTextAdd(str+"\n");
    }

    cx_vec vm = v;
    vm.resize(n,1);
    cx_vec Im = Grr*vm;
    Im.save("电流分布cx",arma_ascii);      //保存复数的电流
}

cx_mat UnEqual_Z_V2::matrixGenerate_Z(QList<Conductor> cutlist)
{
    int n=cutlist.count();
    cx_mat Z(n,n);     //电阻系数矩阵

    deb(tr("开始初始化%1阶阻抗矩阵").arg(n) );
    wall_clock t;
    t.tic();

    SpeedUpCx speedUp;    //加速算法类

    //生成系数矩阵
    for(int i=0;i<n;i++){
        for(int j=i;j<n;j++){
            Conductor c1= cutlist.at(i);
            Conductor c2= cutlist.at(j);
            if(i == j){     //自阻
                cx_double z = speedUp.lookup(c1);
                if(z!=cx_double(-1,0)){
                    Z(i,j) = z;
                }
                else{
                    Heppe h1(c1);
                    Z(i,j) = c1.neizukang(50) + Complex(0,2*pi*50*h1.waizigan());
                    speedUp.addZ(c1,z);
                }
            }
            else{          //互阻
                cx_double z = speedUp.lookup(c1,c2);
                if(z!=cx_double(-1,0)){
                    Z(i,j) = z;
                    Z(j,i) = z;
                }
                else{
                    Heppe h1(c1),h2(c2);
                    cx_double z1 = Complex(0,2*pi*50*h1.hugan(c2));
                    cx_double z2 = Complex(0,2*pi*50*h2.hugan(c1));
                    Z(i,j) = z1;
                    Z(i,j) = z2;

                    cx_double a;
                    if(qAbs(a.real()) >0.001 || qAbs(a.imag()) >0.001){   //求得两值不对称
                        qDebug()<<tr("当前方法计算互阻错误:")<<i<<j<<z1.real()<<z1.imag()<<"   "
                               <<z2.real()<<z2.imag();
//                        a = DianZuCompute::HuZu_JiFen_Half(c1,c2,rou,1000);     //误差较大时采用最稳定的积分法计算
//                        a = DianZuCompute::HuZu_Dawalibi_Half(c1,c2,rou,10);
//                        qDebug()<<tr(" 积分法计算互阻:")<<a;
//                        if(qAbs(r1-a)<qAbs(r2-a))
//                            a=r1;
//                        else
//                            a=r2;
                    }
                    else{
//                        a = (r1+r2)/2;   //在误差较小的情况下，进一步消除差异，保证电阻矩阵对称
                    }

                    speedUp.addZ(c1,c2,z1);
                }
            }
            emit progressValueChanged(i+1,n);
        }
    }
    speedUp.show_CX(n); //显示优化信息
    Z.save("阻抗矩阵.mat",raw_ascii);
    double n_secs1 = t.toc();
    deb(tr("初始化%1阻抗阶矩阵结束，用时%2秒").arg(n).arg(n_secs1) );
    return Z;
}
