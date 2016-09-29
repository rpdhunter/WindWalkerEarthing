#include "Base/compute.h"
#include <qmath.h>
#include "armadillo"    //加载矩阵计算库
#include <QtDebug>
#include "point.h"

using namespace arma;


Compute::Compute(QObject *parent) :
    QObject(parent)
{
}

//此函数可判断两直线(线段延长线)的位置关系
//返回异面，平行，共线或者相交的枚举变量
//函数使用了armadillo线性代数库
//函数利用了矩阵的秩来判断两直线的位置关系，可参考http://dlib.edu.cnki.net/kns50/detail.aspx?dbname=CJFD2005&filename=XUSJ200503024
Compute::Relation Compute::relationship(Conductor c1, Conductor c2)
{
    if(c1.length()==0 || c2.length()==0){
        qDebug()<<QObject::tr("输入错误！有导体长度为0！");
        Relation l;
        l=error;
        return l;
    }
    double l1,m1,n1,l2,m2,n2;
    l1=c1.x1-c1.x0;
    m1=c1.y1-c1.y0;
    n1=c1.z1-c1.z0;
    l2=c2.x1-c2.x0;
    m2=c2.y1-c2.y0;
    n2=c2.z1-c2.z0;

    mat A;

    A << m1 << -l1 << 0   << m1*c1.x0 - l1*c1.y0 << endr
      << 0  << n1  << -m1 << n1*c1.y0 - m1*c1.z0 << endr
      << n1 << 0   << -l1 << n1*c1.x0 - l1*c1.z0 << endr
      << m2 << -l2 << 0   << m2*c2.x0 - l2*c2.y0 << endr
      << 0  << n2  << -m2 << n2*c2.y0 - m2*c2.z0 << endr
      << n2 << 0   << -l2 << n2*c2.x0 - l2*c2.z0 << endr;

    mat B;

    B << m1 << -l1 << 0   << endr
      << 0  << n1  << -m1 << endr
      << n1 << 0   << -l1 << endr
      << m2 << -l2 << 0   << endr
      << 0  << n2  << -m2 << endr
      << n2 << 0   << -l2 << endr;

    int rA,rB;
    rA=rank(A),rB=rank(B);

    Relation f;
    if(rA==4){
        //        qDebug()<<QObject::tr("两直线异面！");
        return f=skew;
    }else if(rA==2&&rB==2){
        //        qDebug()<<QObject::tr("两直线共线！");
        return f=collinear;
    }else if(rA==3&&rB==3){
        //        qDebug()<<QObject::tr("两直线相交！");
        return f=crossing;
    }else if(rA==3&&rB==2){
        //        qDebug()<<QObject::tr("两直线平行！");
        return f=parallel;
    }else{
        //        qDebug()<<QObject::tr("错误！");
        return f=error;
    }
}

//返回两导体的交角（弧度，范围0-pi）
double Compute::jiaoDu(Conductor c1, Conductor c2)
{
    if(c1.length()==0 || c2.length()==0){
        qDebug()<<tr("输入线段有0长度导体");
        return 0;
    }
    double l1 = c1.x1 - c1.x0;
    double m1 = c1.y1 - c1.y0;
    double n1 = c1.z1 - c1.z0;
    double l2 = c2.x1 - c2.x0;
    double m2 = c2.y1 - c2.y0;
    double n2 = c2.z1 - c2.z0;

    double t = (l1*l2 + m1*m2 + n1*n2) / qSqrt( (l1*l1+m1*m1+n1*n1) * (l2*l2+m2*m2+n2*n2) );

    if(t>=1){      //由于计算机数值计算误差，t是有可能大于1的(有过例子1.00000002)
        return 0;
    }else if(t<=-1){
        return datum::pi;       //这里使用了arma定义的圆周率
    }
    return qAcos(t);
}

//此函数返回两线段间最短距离
//函数使用了armadillo线性代数库
//此函数利先利用relationship()判断两线段所在直线位置关系，
//然后再分别计算线段是否相交，进一步求得最短距离
double Compute::space(Conductor c1, Conductor c2)
{
    if(c1.length()==0 || c2.length()==0){
        qDebug()<<QObject::tr("输入错误！有导体长度为0！");
        return -1;
    }

    double l1,m1,n1,l2,m2,n2;
    l1=c1.x1-c1.x0;
    m1=c1.y1-c1.y0;
    n1=c1.z1-c1.z0;
    l2=c2.x1-c2.x0;
    m2=c2.y1-c2.y0;
    n2=c2.z1-c2.z0;

    vec V1(3),V2(3),Vm(3);
    V1<<l1<<m1<<n1;
    V2<<l2<<m2<<n2;
    Vm<<c2.x0-c1.x0<<c2.y0-c1.y0<<c2.z0-c1.z0;

    switch(relationship(c1,c2))
    {
    case skew|crossing:{    //相交或异面
        return getShortestDistance(c1,c2).length();   //之所以将这个函数独立出去，盖因为其实现了更强的功能，以备将来重复使用
    }
    case collinear:{     //共线
        vec temp(6);
        temp <<norm(V1,2) <<norm(V2,2) <<norm(Vm,2) <<norm(Vm+V2-V1,2)
            <<norm(Vm+V2,2) <<norm(Vm-V1,2);
        double lmax=temp.max(),lmin=temp.min();
        Conductor l;
        //        if(norm(V1,2)+ norm(V2,2)-lmax>-0.001 && norm(V1,2)+ norm(V2,2)-lmax<0.001 && lmin==0){
        if(qFuzzyCompare (norm(V1,2)+ norm(V2,2), lmax) && lmin==0){  //这里使用的是qt的模糊比较函数
            //            qDebug()<<QObject::tr("共线交于一点");
            return 0;
        }else if(lmax > norm(V1,2)+ norm(V2,2)){
            //            qDebug()<<QObject::tr("共线不相交");
            return lmin;
        }else{
            //            qDebug()<<QObject::tr("共线重叠");
            return -1;
        }
    }
    case parallel:{     //平行
        double k=norm(V2,2) / norm(V1,2);
        double p=dot(Vm,V1) / norm(V1,2);
        double t1=p,t2=p+k;
        if( (t1<0 && t2<0) || (t1>1 && t2>1) ){
            vec temp(4);
            temp <<norm(Vm,2) <<norm(Vm+V2-V1,2) <<norm(Vm+V2,2) <<norm(Vm-V1,2);
            return temp.min();
        }else{
            return norm(Vm-V1*t1,2);
        }
    }
    default:
        return -1;

    }


}

//判断两线段是否重叠
bool Compute::isCoincide(Conductor c1, Conductor c2)
{
    if(relationship(c1,c2)==collinear && space(c1,c2)==-1)
        return true;
    return false;
}


//此函数返回两相交或者异面线段间距离最短的线段坐标（以Conductor的格式）
//此函数使用前提需要判断两线段位置关系是异面或者相交，否则程序会出错
//函数使用了armadillo线性代数库
//此函数利用直线的参数方程，求得公垂线和两直线的交点，并判断交点是否位于线段之上，返回结果
Conductor Compute::getShortestDistance(Conductor c1, Conductor c2)
{
    Conductor l;

    double l1,m1,n1,l2,m2,n2;
    l1=c1.x1-c1.x0;
    m1=c1.y1-c1.y0;
    n1=c1.z1-c1.z0;
    l2=c2.x1-c2.x0;
    m2=c2.y1-c2.y0;
    n2=c2.z1-c2.z0;

    vec V1(3),V2(3),Vm(3);
    V1<<l1<<m1<<n1;
    V2<<l2<<m2<<n2;
    Vm<<c2.x0-c1.x0<<c2.y0-c1.y0<<c2.z0-c1.z0;

    //计算参数t1,t2(读者可自行整理)
    mat A(2,2);
    A <<dot(V1,V1) <<-dot(V1,V2) <<endr
     <<dot(V1,V2) <<-dot(V2,V2) <<endr;

    vec B(2);
    B <<dot(Vm,V1) <<dot(Vm,V2);

    vec T(2);

    if(det(A) == 0){    //说明程序入口参数产生偏差，两线段平行或重合（源于机内实数的误差）
        //        double t = - dot(V1,Vm) / dot(V1,V1);
        l.dataNodeInit(c1.x0,c1.y0,c1.z0,c2.x0,c2.y0,c2.z0);
        return l;
    }

    T=solve(A,B);

    double t1=T(0);
    double t2=T(1);

    //        qDebug()<<"t1="<<T(0)<<"t2="<<T(1);

    //比较垂点是否在线段上
    vec P1(3),P2(3);
    if(t1<0){
        P1 <<c1.x0 <<c1.y0 <<c1.z0;
    }else if(t1>1){
        P1 <<c1.x1 <<c1.y1 <<c1.z1;
    }else{
        P1 <<c1.x0+l1*t1 <<c1.y0+m1*t1 <<c1.z0+n1*t1;
    }
    if(t2<0){
        P2 <<c2.x0 <<c2.y0 <<c2.z0;
    }else if(t2>1){
        P2 <<c2.x1 <<c2.y1 <<c2.z1;
    }else{
        P2 <<c2.x0+l2*t2 <<c2.y0+m2*t2 <<c2.z0+n2*t2;
    }
    l.dataNodeInit(P1(0),P1(1),P1(2),P2(0),P2(1),P2(2));
    return l;
}

//此函数返回两线段间距离公垂线段坐标（以Conductor的格式）
//此函数使用前不需要判断两线段位置关系
//函数使用了armadillo线性代数库
//此函数利用直线的参数方程，求得公垂线和两直线的交点，如果两线段平行或共线，取第一条线段上一点作公垂线
Conductor Compute::getCrossPoint(Conductor c1, Conductor c2)
{
    double l1,m1,n1,l2,m2,n2;
    l1=c1.x1-c1.x0;
    m1=c1.y1-c1.y0;
    n1=c1.z1-c1.z0;
    l2=c2.x1-c2.x0;
    m2=c2.y1-c2.y0;
    n2=c2.z1-c2.z0;

    vec V1(3),V2(3),Vm(3);
    V1<<l1<<m1<<n1;
    V2<<l2<<m2<<n2;
    Vm<<c2.x0-c1.x0<<c2.y0-c1.y0<<c2.z0-c1.z0;

    Conductor l;

    //分情况计算
    switch(relationship(c1,c2))
    {
    case skew:
    case crossing:{    //相交或异面
        //计算参数t1,t2(读者可自行整理)
        mat A(2,2);
        A <<dot(V1,V1) <<-dot(V1,V2) <<endr
         <<dot(V1,V2) <<-dot(V2,V2) <<endr;

        vec B(2);
        B <<dot(Vm,V1) <<dot(Vm,V2);

        vec T(2);

        if(rank(A) == 2){
            T=solve(A,B);

            double t1=T(0);
            double t2=T(1);

            //        qDebug()<<"t1="<<T(0)<<"t2="<<T(1);

            vec P1(3),P2(3);
            P1 <<c1.x0+l1*t1 <<c1.y0+m1*t1 <<c1.z0+n1*t1;
            P2 <<c2.x0+l2*t2 <<c2.y0+m2*t2 <<c2.z0+n2*t2;

            l.dataNodeInit(P1(0),P1(1),P1(2),P2(0),P2(1),P2(2));
        }
        else{
            double t = - dot(V1,Vm) / dot(V1,V1);
            l.dataNodeInit(c1.x0,c1.y0,c1.z0,c2.x0+l1*t,c2.y0+m1*t,c2.z0+n1*t);
        }
        break;
    }
    case collinear:{     //共线
        l.dataNodeInit(c1.x0,c1.y0,c1.z0,c1.x0,c1.y0,c1.z0);
        break;
    }
    case parallel:{     //平行
        double t = - dot(V1,Vm) / dot(V1,V1);
        l.dataNodeInit(c1.x0,c1.y0,c1.z0,c2.x0+l1*t,c2.y0+m1*t,c2.z0+n1*t);
        break;
    }
    default:
        break;

    }
    return l;


}

//将输入导体延与导体垂直的方向平移距离a
Conductor Compute::Pingyi(Conductor c, double a)
{
    double l = c.x1 - c.x0;
    double m = c.y1 - c.y0;
    double n = c.z1 - c.z0;

    double lt,mt,nt;

    if(l!= 0 || m!=0){
        lt = - a * m / qSqrt(l*l + m*m);
        mt = a * l / qSqrt(l*l + m*m);
        nt = 0;
    }
    else{
        lt = a;
        mt = 0;
        nt = 0;
    }

    Conductor c1=c;
    c1.dataNodeInit(c.x0+lt,c.y0+mt,c.z0+nt,c.x1+lt,c.y1+mt,c.z1+nt);
    return c1;
}

//平移导体c2（延远离c1的方向）
//移动距离为a
//使用了一个方程组求解c2移动的方向向量
//理解程序时，可能需要做一个详细的向量推导
//
Conductor Compute::Pingyi(Conductor c1, Conductor c2, double a)
{

    vec l1,l2,l3;
    l1 << c1.x1 - c1.x0 << c1.y1 - c1.y0 << c1.z1 - c1.z0;
    l2 << c2.x1 - c2.x0 << c2.y1 - c2.y0 << c2.z1 - c2.z0;

    l3 = cross(l2,cross(l1,l2));    //c2移动的方向向量

    double nor = norm(l3, 2);     //求出l3的2-泛数
    double nor2 = sqrt(pow(l3.at(0),2)+pow(l3.at(1),2)+pow(l3.at(2),2));

    if(nor>0){
        double t = a / nor;
        Conductor c3=c2;
        c3.dataNodeInit(c2.x0+t*l3.at(0),c2.y0+t*l3.at(1),c2.z0+t*l3.at(2),
                        c2.x1+t*l3.at(0),c2.y1+t*l3.at(1),c2.z1+t*l3.at(2));
        //由于同一平面上存在2个符合条件的导体，由于难以预先判断哪个合适，只得做事后判断
        Conductor c4=c2;
        c4.dataNodeInit(c2.x0-t*l3.at(0),c2.y0-t*l3.at(1),c2.z0-t*l3.at(2),
                        c2.x1-t*l3.at(0),c2.y1-t*l3.at(1),c2.z1-t*l3.at(2));
        if(space(c3,c1)>space(c4,c1))
            return c3;
        else
            return c4;
    }
    else{       //若共线或其他情况，可向任意方向平移
        return Pingyi(c2,a);
    }
}

Conductor Compute::Suoduan(Conductor c1, double a)
{
    double l = c1.x1 - c1.x0;
    double m = c1.y1 - c1.y0;
    double n = c1.z1 - c1.z0;

    double Ls = c1.length();

    Conductor c = c1;
    c.dataNodeInit(l*a/Ls + c1.x0, m*a/Ls + c1.y0, n*a/Ls + c1.z0,
                   l*(Ls-a)/Ls + c1.x0, m*(Ls-a)/Ls + c1.y0, n*(Ls-a)/Ls + c1.z0);
    return c;
}



bool Compute::insertPoint(QVector<double> &P, QList<QVector<double> > &points)
{
    //    qDebug()<<"cha ru dian:"<<P[0]<<P[1]<<P[2];
    //    for(int i=0;i<points.count();i++){
    //        qDebug()<<"dao"<<points[i][0]<<points[i][1]<<points[i][2];
    //    }

    double d1,d2;       //表征点与点之间的距离，为了增快速度，使用了坐标绝对值之和
    d1=qAbs(P[0]-points[0][0])+qAbs(P[1]-points[0][1])+qAbs(P[2]-points[0][2]);

    for(int i=1;i<points.count();i++){
        d2=qAbs(points[i][0]-points[0][0])
                +qAbs(points[i][1]-points[0][1])
                +qAbs(points[i][2]-points[0][2]);
        if(d1<d2){
            points.insert(i,P);
            //            qDebug()<<QObject::tr("插入成功！");
            return true;
        }
    }
    return false;
}

//导体组切割函数(交点切割,或称为一次切割)
QList<Conductor> Compute::cutConductor(QList<Conductor> datalist)
{    
    QList<QList<QVector<double> > > pointlist;

    QVector<double> P1,P2;
    QList<QVector<double> > points1;

    //首先先将所有导体端点存储至pointlist
    for(int i=0;i<datalist.count();i++){
        P1.clear();
        P2.clear();
        points1.clear();
        P1.append(datalist[i].x0);
        P1.append(datalist[i].y0);
        P1.append(datalist[i].z0);
        P2.append(datalist[i].x1);
        P2.append(datalist[i].y1);
        P2.append(datalist[i].z1);
        points1.append(P1);
        points1.append(P2);
        pointlist.append(points1);
    }

    //再将所有交点存入pointlist
    Conductor L;
    for(int i=0;i<datalist.count()-1;i++){
        for(int j=i+1;j<datalist.count();j++){
            if(Compute::relationship(datalist[i],datalist[j])==crossing){
                P1.clear();
                L=Compute::getShortestDistance(datalist[i],datalist[j]);
                if(L.length()<0.00001){   //判断两线段相交
                    P1.append(L.x0);
                    P1.append(L.y0);
                    P1.append(L.z0);
                    Compute::insertPoint(P1,pointlist[i]);
                    Compute::insertPoint(P1,pointlist[j]);
                }
            }
        }
        emit progressValueChanged_cut(i+1,datalist.count()-1);
    }


    QList<Conductor> cutlist;

    for(int i=0;i<pointlist.count();i++){
        L = datalist.value(i);      //这里赋值，保证了切割导体具有原导体的一切属性
        for(int j=0;j<pointlist[i].count()-1;j++){
            int k=0;
            do{
                L.dataNodeInit(pointlist[i][j][0],pointlist[i][j][1],pointlist[i][j][2],
                        pointlist[i][j+1+k][0],pointlist[i][j+1+k][1],pointlist[i][j+1+k][2]);
                k++;
            }while(L.length()<0.001 && j+k<pointlist[i].count()-1);
            j+=k-1;
            if(L.length()>0.001)
                cutlist.append(L);
        }
    }

    return cutlist;

}

//返回导体组的总长度
double Compute::totalLength(QList<Conductor> list)
{
    Conductor c;
    double sum = 0; //总导体长度
    foreach(c,list){
        sum += c.length();
    }
    return sum;
}

//返回导体组的平均长度
double Compute::averageLength(QList<Conductor> list)
{
    Q_ASSERT(list.count() > 0);

    double sum = Compute::totalLength(list); //平均长度
    return sum / list.count();
}

//返回导体组的最短长度
double Compute::minLength(QList<Conductor> list)
{
    Q_ASSERT(list.count() > 0);

    Conductor c=list.first();
    double lmin = c.length();
    foreach(c,list){    //通过循环找出最短的导体长度
        if(c.length()<lmin){
            lmin=c.length();
        }
    }
    return lmin;
}

//切割过长导体（定义超过平均长度2倍的导体为过长导体），如果没有过长导体，返回true，否则
//进行切割，并返回false
bool Compute::cutLongConductor(QList<Conductor> &list)
{
    Q_ASSERT(list.count() > 0);
    double ave_len = this->averageLength(list); //平均长度

    QList<Conductor> newlist;
    Conductor c;
    foreach(c,list){
        int k = (int)(c.length()/ave_len);
        newlist.append(averageCut(c,k));
    }
    if(list.count() == newlist.count()){
        list = newlist;
        return true;
    }else{
        list = newlist;
        return false;
    }
}

//二次切割
//包含两个规则：
//1.调用cutLongConductor()函数切割长导体，直到没有长导体
//2.如果平均长度已经小于开始的最小长度，则停止切割，用于避免规则1的无限循环
QList<Conductor> Compute::secendCut(QList<Conductor> list)
{
    Q_ASSERT(list.count() > 0);

    double lmin = Compute::minLength(list); //最短长度

    Compute c;
    int k=0;
    while(!c.cutLongConductor(list)){
        double a = Compute::averageLength(list);    //平均长度
        if(a<lmin)      //切割到最小长度就停止切割
            break;
        k++;
        //        qDebug()<<tr("第%1次计算~~").arg(k);
    }
    return list;
}

//三次切割(仅作研究之用)
//在二次切割的基础上再细分导体，目的是研究电流在导体上泄流的分布规律
//每个导体平分成n份
QList<Conductor> Compute::thirdCut(QList<Conductor> list, int k)
{
    Q_ASSERT(list.count() > 0);

    QList<Conductor> newlist;
    Conductor c;
    foreach(c,list){
        newlist.append(averageCut(c,k));
    }
    return newlist;
}

QList<Conductor> Compute::averageCut(Conductor c, int k)
{
    if(k==0)
        k++;

    double dx = (c.x1 - c.x0)/k;
    double dy = (c.y1 - c.y0)/k;
    double dz = (c.z1 - c.z0)/k;

    QList<Conductor> templist;
    for(int i=0;i<k;i++){
        Conductor con=c;
        con.dataNodeInit(c.x0+i*dx,c.y0+i*dy,c.z0+i*dz,
                         c.x0+(i+1)*dx,c.y0+(i+1)*dy,c.z0+(i+1)*dz);        //妈的，第三项错写成c.z0+(i+i)*dz,半年后才改好
        templist.append(con);
    }
    return templist;
}

