#include "unequal_r.h"

const double pi = 3.1415926;        //圆周率
const double MINVALUE = 0.000001;   //最小值（用于比较实数和零，如果小于这个数明则认为相等）

UnEqual_R::UnEqual_R(QList<Conductor> list, ComputDialog *computeDlg) :
    Equal(list,computeDlg)
{
    crossPointList = this->crossPoint();    //生成交点列表
    n = cutlist.count();     //中点数
    m = crossPointList.count();     //交点数
    powerlistGenerate();        //生成电源列表
}

void UnEqual_R::run()
{
    ////////////////////////////////////////
    /// \矩阵生成
    ////////////////////////////////////////
    mat Gtt = matrixGenerate_GTT();
    mat Gmm = matrixGenerate_GMM();
    mat Gtm = matrixGenerate_GTM();
    mat Gmt = strans(Gtm);              //使用简单转置，复数不进行共轭转置
    mat Grr = inv(matrixGenerate(this->cutlist));

    mat T1 = join_rows(Gmm + Grr,Gmt);
    mat T2 = join_rows(Gtm,Gtt);
    mat G = join_cols(T1,T2);       //生成不点电位计算的系数矩阵

    ////////////////////////////////////////
    /// \矩阵求解
    ////////////////////////////////////////
    deb( tr("求解%1阶矩阵开始").arg(n+m) );
    mat II(m+n,powerlist.count());
    mat vv(m+n,powerlist.count());      //vv用于存储不同电流注入点单独注入时产生的响应
    for(int i=0;i<powerlist.count();i++){
        II.col(i) = currentGenerate_single(i);
        vv.col(i) = solve(G,II.col(i));
    }

    II.save("II",arma_ascii);
    vv.save("vv",arma_ascii);
    vec v = sum(vv,1);


    ////////////////////////////////////////
    /// \保存结果
    ////////////////////////////////////////
    savePointVoltageToFile(v,crossPointList);   //保存节点电压结果

    for(int i=0;i<powerlist.count();i++){
        int k = powerlist.value(i).n +n;
        QString str = tr("不等电位注入点%1(%2)接地电阻：%3Ω").arg(i+1).arg(powerlist.value(i).cs.remarks)
                .arg(vv(k,i)/II(k,i));
        deb(str);
        emit outputTextAdd(str+"\n");
    }

    vec vm = v;
    vm.resize(n,1);
    vec Ir = Grr*vm;
    Ir.save("电流分布.mat",raw_ascii);

    //以下代码段，用于使输出具有统一接口，由于输出显示电流默认都是复数形式的
    vec Im(n);
    Im.zeros();
    cx_vec I_cx(n);
    I_cx.set_real(Ir);
    I_cx.set_imag(Im);
    I_cx.save("电流分布cx",arma_ascii);
}

void UnEqual_R::savePointVoltageToFile(vec v, QList<CrossPoint> crossPointList)
{
    QList<CurrentSource> pointlist; //要保存的数据类型

    //生成导体中点
    for(int i=0;i<n;i++){
        CurrentSource cs(cutlist.value(i).centrePoint());
        cs.setCurrent(v.at(i),0);       //如果v是复数，要做调整
        pointlist.append(cs);
    }
    //生成导体交点
    for(int i=0;i<m;i++){
        CurrentSource cs(crossPointList.value(i).P);
        if(crossPointList.value(i).isPower == true){
            cs = crossPointList.value(i).cs;
            cs.f = -1;      //设定的暗语-1代表电源
        }
        cs.setCurrent(v.at(n+i),0);       //如果v是复数，要做调整
        pointlist.append(cs);
    }

    //输出节点电压
    QString fileName = dlg->currentDir + "/" + tr("节点电压.txt");
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return;
    QTextStream out(&file);
    foreach(CurrentSource cs,pointlist){
        out << cs.output() <<endl;
    }
}

void UnEqual_R::savePointVoltageToFile(cx_vec v, QList<CrossPoint> crossPointList)
{
    QList<CurrentSource> pointlist; //要保存的数据类型

    //生成导体中点
    for(int i=0;i<n;i++){
        CurrentSource cs(cutlist.value(i).centrePoint());
        cs.setCurrent(v.at(i));       //v是复数，要做调整
        pointlist.append(cs);
    }
    //生成导体交点
    for(int i=0;i<m;i++){
        CurrentSource cs(crossPointList.value(i).P);
        if(crossPointList.value(i).isPower == true){
            cs = crossPointList.value(i).cs;
            cs.f = -1;      //设定的暗语-1代表电源
        }
        cs.setCurrent(v.at(n+i));       //v是复数，要做调整
        pointlist.append(cs);
    }

    //输出节点电压
    QString fileName = dlg->currentDir + "/" + tr("节点电压.txt");
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return;
    QTextStream out(&file);
    foreach(CurrentSource cs,pointlist){
        out << cs.output() <<endl;
    }
}

//求得导体所有的交点
QList<CrossPoint> UnEqual_R::crossPoint()
{
    QList<CrossPoint> crossPointList;   //交点列表
    for(int i=0;i<cutlist.count();i++){
        addConductor(i,crossPointList);       //尝试添加一段导体进节点列表中
    }

    deb(tr("共有%1个交点").arg(crossPointList.count()) );
    return crossPointList;
}

void UnEqual_R::powerlistGenerate()
{
    for(int i=0;i<m;i++){
        for(int j=0;j<dlg->currentSourceList.count();j++){
            if(Point::length(crossPointList.at(i).P,dlg->currentSourceList.at(j).P)<0.0000001){ //重合
                //在crossPointList进行标注，为了在输出中区别显示电流源
                crossPointList[i].isPower = true;
                crossPointList[i].cs = dlg->currentSourceList.value(j);
                crossPointList[i].n = i;
                powerlist.append(crossPointList.value(i));      //记录电源点
            }
        }
    }
}

void UnEqual_R::addConductor(int i, QList<CrossPoint> &crossPointList)
{
    Point P0 = cutlist.value(i).P0();      //尝试插入的两个端点
    Point P1 = cutlist.value(i).P1();

    addPoint(P0,i,crossPointList);
    addPoint(P1,i,crossPointList);
}

void UnEqual_R::addPoint(Point P, int i, QList<CrossPoint> &crossPointList)
{
    bool flag = false;
    for(int j=0;j<crossPointList.count();j++){
        if(Point::length(P,crossPointList.at(j).P)<MINVALUE){    //判断两点重合
            flag = true;
            crossPointList[j].c_n.append(i);        //为节点添加导体
            break;
        }
    }
    if(flag == false){          //说明节点库内无当前节点，要插入一个新节点
        CrossPoint newCrossPoint;
        newCrossPoint.P = P;
        newCrossPoint.c_n.append(i);
        crossPointList.append(newCrossPoint);
    }
}

mat UnEqual_R::matrixGenerate_GTT()
{
    mat Gtt(m,m);
    Gtt.zeros();

    for(int i=0;i<m;i++){
        CrossPoint c = crossPointList.at(i);
        double g=0;
        for(int j=0;j<c.c_n.count();j++){
            g += 2*cutlist.value(c.c_n.at(j)).diandao();        //自电导为正（注意调整自己的体系）
        }
        Gtt(i,i) = g;
    }
    return Gtt;
}

mat UnEqual_R::matrixGenerate_GMM()
{
    mat Gmm(n,n);
    Gmm.zeros();

    for(int i=0;i<n;i++){
        Gmm(i,i) = 4*cutlist.value(i).diandao();            //自电导为正（注意调整自己的体系）
    }
    return Gmm;
}

mat UnEqual_R::matrixGenerate_GTM()
{
    mat Gtm(m,n);
    Gtm.zeros();

    for(int i=0;i<m;i++){
        CrossPoint c = crossPointList.at(i);
        for(int j=0;j<c.c_n.count();j++){       //在关联的导体上才和邻近的中点有互电导，否则为0
            int z = c.c_n.at(j);    //和交点相关的导体编号
            Gtm(i,z) = -2*cutlist.value(z).diandao();        //互电导为负（注意调整自己的体系）
        }
    }
    return Gtm;
}

vec UnEqual_R::currentGenerate()
{
    vec I1(n);
    vec I2(m);
    I1.zeros();
    I2.zeros();

    //导入电流源
    for(int i=0;i<powerlist.count();i++){
        I2.at(powerlist.value(i).n) = dlg->currentSourceList.value(i).real();       //赋值，以后要进行复数修正
    }

    vec I = join_cols(I1,I2);
    return I;
}

//生成单一电源点的电流序列
vec UnEqual_R::currentGenerate_single(int i)
{
    vec I(n+m);
    I.zeros();

    I.at(n + powerlist.value(i).n) = dlg->currentSourceList.value(i).real();
    return I;
}

cx_vec UnEqual_R::currentGenerate_single_cx(int i)
{
    cx_vec I(n+m);
    I.zeros();      //这个函数待观察

    I.at(n + powerlist.value(i).n) = dlg->currentSourceList.value(i).current();
    return I;
}

cx_vec UnEqual_R::currentGenerate_cx()
{
    cx_vec I1(n);
    cx_vec I2(m);
    I1.zeros();
    I2.zeros();

    //导入电流源
    for(int i=0;i<powerlist.count();i++){
        I2.at(powerlist.value(i).n) = dlg->currentSourceList.value(i).current();       //赋值，以后要进行复数修正
    }

    cx_vec I = join_cols(I1,I2);
    return I;
}


