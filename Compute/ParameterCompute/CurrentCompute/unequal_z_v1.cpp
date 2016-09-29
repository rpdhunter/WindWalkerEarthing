#include "unequal_z_v1.h"
#include "Compute/DianzuCompute/heppe.h"

const double pi = 3.1415926;        //圆周率
const double MINVALUE = 0.000001;   //最小值（用于比较实数和零，如果小于这个数明则认为相等）


UnEqual_Z_V1::UnEqual_Z_V1(QList<Conductor> list, ComputDialog *computeDlg) :
    UnEqual_R(list,computeDlg)
{
}

void UnEqual_Z_V1::run()
{
    ////////////////////////////////////////
    /// \矩阵生成
    ////////////////////////////////////////
    cx_mat Gtt = matrixGenerate_GTT_CX();
    cx_mat Gmm = matrixGenerate_GMM_CX();
    cx_mat Gtm = matrixGenerate_GTM_CX();
    cx_mat Gmt = strans(Gtm);      //使用简单转置，复数不进行共轭转置
    mat Grr = inv(matrixGenerate(this->cutlist));

    cx_mat T1 = join_rows(Gmm + Grr,Gmt);
    cx_mat T2 = join_rows(Gtm,Gtt);
    cx_mat G = join_cols(T1,T2);

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

cx_mat UnEqual_Z_V1::matrixGenerate_GTT_CX()
{
    cx_mat Gtt(m,m);
    Gtt.zeros();

    for(int i=0;i<m;i++){
        CrossPoint c = crossPointList.at(i);
        cx_double g=0;
        for(int j=0;j<c.c_n.count();j++){
            Conductor cc = cutlist.value(c.c_n.at(j));
            Heppe heppe(cc);
            g += cx_double(2)/(cc.neizukang(50)+cx_double(0,2*pi*50*heppe.waizigan()));        //自电导为正（注意调整自己的体系）
        }
        Gtt(i,i) = g;
        emit progressValueChanged(i+1,m);
    }
    return Gtt;
}

cx_mat UnEqual_Z_V1::matrixGenerate_GMM_CX()
{
    cx_mat Gmm(n,n);
    Gmm.zeros();

    for(int i=0;i<n;i++){
        Conductor cc = cutlist.value(i);
        Heppe heppe(cc);
        Gmm(i,i) = cx_double(4)/(cc.neizukang(50)+cx_double(0,2*pi*50*heppe.waizigan()));            //自电导为正（注意调整自己的体系）
        emit progressValueChanged(i+1,n);
    }
    return Gmm;
}

cx_mat UnEqual_Z_V1::matrixGenerate_GTM_CX()
{
    cx_mat Gtm(m,n);
    Gtm.zeros();

    for(int i=0;i<m;i++){
        CrossPoint c = crossPointList.at(i);
        for(int j=0;j<c.c_n.count();j++){       //在关联的导体上才和邻近的中点有互电导，否则为0
            int z = c.c_n.at(j);    //和交点相关的导体编号
            Conductor cc = cutlist.value(z);
            Heppe heppe(cc);
            Gtm(i,z) = -cx_double(2)/(cc.neizukang(50)+cx_double(0,2*pi*50*heppe.waizigan()));        //互电导为负（注意调整自己的体系）
        }
        emit progressValueChanged(i+1,m);
    }
    return Gtm;
}
