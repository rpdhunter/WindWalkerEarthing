#include "secoundsubdivision.h"

#include <QtCore/qmath.h>
#include "armadillo"
#include <QtDebug>
#include "Base/point.h"
#include "Base/compute.h"

#include "dianzucompute.h"

using namespace arma;

const double pi = 3.1415926;        //圆周率

//构造函数，完成3个功能：
//1.对输入导体进行坐标变换，三维坐标变为二维，简化计算
//2.求出镜像坐标
//3.切割导体至n段
SecoundSubdivision::SecoundSubdivision(Conductor c, int n)
{
    this->c = c;
    P0.setX(0);
    P0.setY(c.z0);
    P1.setX(qSqrt(qPow((c.x0-c.x1),2)+qPow((c.y0-c.y1),2)));
    P1.setY(c.z1);

    _P0.setX(0);
    _P0.setY(-c.z0);
    _P1.setX(P1.x());
    _P1.setY(-c.z1);

    l = c.length();
    r = c.r;
    this->n = n;

    cutlist_2d.clear();
    _cutlist_2d.clear();
    for(int i=0;i<n;i++){
        cutlist_2d.append(QPointF(P0.x()+i*(P1.x()-P0.x())/n,P0.y()+i*(P1.y()-P0.y())/n));
        _cutlist_2d.append(QPointF(P0.x()+i*(P1.x()-P0.x())/n,-P0.y()-i*(P1.y()-P0.y())/n));
    }

    cutlist.clear();
    cutlist.append(c);
    Compute temp;
    cutlist = temp.thirdCut(cutlist,n);     //进行n等分切割

}

//求自电阻的主函数
double SecoundSubdivision::zizu()
{
    matrixCompute();
    return X.at(n);
}

double SecoundSubdivision::huzu(Conductor c2)
{

}

//求解某一点的电位值
//使用电流平均泄流的模型，但是在点和微段距离过近时，采用陈先璐公式修正
//结果是半空间的（如今后多层算法时有需求，可能需要修改此函数）
double SecoundSubdivision::Vp(Point P)
{
    for(int i=0;i<n;i++){

    }
}


//完成测试电源公式和陈先璐公式
void SecoundSubdivision::test()
{
    int nn=1000;        //设定1000点采样
    mat R(4,nn);
    double rr = r+0.02;

    for(int i=0;i<nn;i++){
        qDebug()<<"i="<<i;
        R(0,i) = i*l/nn;    //横坐标
        R(1,i) = Green_ChenXianLu(rr,i*l/nn);    //在R=r情况下，点电位值
        double ll = qSqrt(pow(i*l/nn,2)+pow(rr,2));
        R(2,i) = 1/(4*pi*ll);
        R(3,i) = 1/(4*pi*(i*l/nn));

    }
    R.save("R.mat", raw_ascii);
    qDebug()<<"end!";
}

//求解二次分域矩阵的主函数（最耗时的计算），完成功能如下：
//1.构成单根导体二次分域后形成的阻抗矩阵
//2.求解矩阵，得到n+1阶向量X，X前n项为电流的分布，第n+1项为自电阻
void SecoundSubdivision::matrixCompute()
{
    mat R(n,n);
    for(int i=0;i<n;i++){
        for(int j=0;j<n;j++){
            Conductor c1 = cutlist.at(i);
            Conductor c2 = cutlist.at(j);
            if(i==j){       //自阻
//                R(i,j) = subZizu(cutlist_2d.at(i));
                if(c1.length()<2*c1.r){
                    R(i,j) = DianZuCompute::ZiZu_Kouteynikoff_Half(c1,1,3);
                }
                else{
                    R(i,j) = DianZuCompute::ZiZu_JiHe_Half(c1,1);
                }
            }
            else{           //互阻
//                R(i,j) = subHuzu(cutlist_2d.at(i),cutlist_2d.at(j));
                if(c1.length()<2*c1.r && Compute::space(c1,c2)<2*(c1.length()+c2.length())){
                    R(i,j) = DianZuCompute::HuZu_Kouteynikoff_Half(c1,c2,1,3);
                }
                else{
                    R(i,j) = DianZuCompute::HuZu_Dawalibi_Half(c1,c2,1,100);
                }
            }
        }
    }

    R.save("R.mat", raw_ascii);

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
    c(n) = 1;

    vec x = solve(RR, c);

    this->X = x;

    x.shed_row(n);
    x.save("X.mat", raw_ascii);
}

//微分段的自阻，使用Kouteynikoff法求解
double SecoundSubdivision::subZizu(QPointF P)
{
    double l1 = l/n/2;     //P导体等效半径（按微段导体长度的一半计算，算法存疑）
    double l2 = qAbs(2*P.y());     //P和镜像点之间的距离

    if(l1==0||l2==0){
        qDebug()<<"zizu compute error!";
    }
    return (1/l1 + 1/l2)/4/pi;
}

double SecoundSubdivision::subHuzu(QPointF P,QPointF Q)
{
    double l1 = qSqrt(qPow((P.x()-Q.x()),2)+qPow((P.y()-Q.y()),2));     //PQ两点之间的距离
    double l2 = qSqrt(qPow((P.x()-Q.x()),2)+qPow((P.y()+Q.y()),2));     //P和Q镜像点之间的距离

    if(l1==0||l2==0){
        qDebug()<<"zizu compute error!";
    }

    double A,B;

    //原像产生的电位
    if(l1 < 10*l/n){    //近距离使用特殊公式（贝塞尔积分公式）
        A = Green_ChenXianLu(0.1,l1);
    }
    else{
        A = 1/(4*pi*l1);
    }

    //镜像产生的点位（第一种情况正常情况下不会用到）
    if(l2 < 10*l/n){    //近距离使用特殊公式（贝塞尔积分公式）
        double Z = (l1*l1 + l2*l2 - 4*Q.y()*Q.y()) / 2 / l1;    //此公式使用余弦定理推导
        double R = qSqrt(l2*l2 - Z*Z);          //勾股定理
        B = Green_ChenXianLu(R,Z);
    }
    else{
        B = 1/(4*pi*l2);
    }

    return A+B;
}

//陈先璐给出的格林函数（全空间带贝塞尔函数的积分公式）
double SecoundSubdivision::Green_ChenXianLu(double R, double Z)
{
    double eps = 0.001;  //精度
    double V = asr(0.00001,100,eps,r,l/n,R,Z),dV;
    int i=0;
    do{
//        qDebug()<<"i="<<i;
        dV = asr(100*pow(10,i),100*pow(10,i+1),eps,r,l/n,R,Z);  //几何级数增长计算区间
        V += dV;
        i++;
    }
    while(dV>eps);     //精度为0.000001

    return V / (pi*pi*r*l/n);
}


//积分函数体
double SecoundSubdivision::F(double x, double a, double b, double r, double z)
{
    double S = second_modified_Bessel(0,x*r) / second_modified_Bessel(1,x*a);
    return S * cos(z*x) * sin(b*x/2) / x / x ;
}

//变步长辛普森公式
double SecoundSubdivision::asr(double aa, double bb, double eps, double a, double b, double r, double z)
{
    double v = asr(aa,bb,eps,simpson(bb-aa,F(aa,a,b,r,z),F(bb,a,b,r,z),F((bb+aa)/2,a,b,r,z)),a,b,r,z);
//    qDebug()<<v;
    return v;
}

//变步长辛普森公式
double SecoundSubdivision::asr(double aa, double bb, double eps, double A, double a, double b, double r, double z)
{
//    qDebug()<<"die dai yi ci";
    double c=(aa+bb)/2;
    double fa,fb,fc,L,R;
    fa=F(aa,a,b,r,z);
    fb=F(bb,a,b,r,z);
    fc=F(c,a,b,r,z);
    L=simpson(c-aa,fa,fc,F((c+aa)/2,a,b,r,z));
    R=simpson(bb-c,fc,fb,F((bb+c)/2,a,b,r,z));
    if(fabs(L+R-A)<=15*eps)
        return L+R+(L+R-A)/15;
    return asr(aa,c,eps/2,L,a,b,r,z)+asr(c,bb,eps/2,R,a,b,r,z);
}

//辛普森函数体
double SecoundSubdivision::simpson(double width, double fa, double fb, double fc)
{
    return (fb+fa+4*fc)*width/6;
}










//第一类修正贝赛尔曲线
double SecoundSubdivision::first_modified_Bessel(int n, double x)
{
    int i,m;
    double t,y,p,b0,b1,q;
    static double a[7]={ 1.0,3.5156229,3.0899424,1.2067492,
                         0.2659732,0.0360768,0.0045813};
    static double b[7]={ 0.5,0.87890594,0.51498869,
                         0.15084934,0.02658773,0.00301532,0.00032411};
    static double c[9]={ 0.39894228,0.01328592,0.00225319,
                         -0.00157565,0.00916281,-0.02057706,
                         0.02635537,-0.01647633,0.00392377};
    static double d[9]={ 0.39894228,-0.03988024,-0.00362018,
                         0.00163801,-0.01031555,0.02282967,
                         -0.02895312,0.01787654,-0.00420059};
    if (n<0) n=-n;
    t=fabs(x);
    if (n!=1)
    {
        if (t<3.75)
        {
            y=(x/3.75)*(x/3.75); p=a[6];
            for (i=5; i>=0; i--)
                p=p*y+a[i];
        }
        else
        {
            y=3.75/t; p=c[8];
            for (i=7; i>=0; i--)
                p=p*y+c[i];
            p=p*exp(t)/sqrt(t);
        }
    }
    if (n==0) return(p);
    q=p;
    if (t<3.75)
    {
        y=(x/3.75)*(x/3.75); p=b[6];
        for (i=5; i>=0; i--) p=p*y+b[i];
        p=p*t;
    }
    else
    {
        y=3.75/t; p=d[8];
        for (i=7; i>=0; i--) p=p*y+d[i];
        p=p*exp(t)/sqrt(t);
    }
    if (x<0.0) p=-p;
    if (n==1) return(p);
    if (x==0.0) return(0.0);
    y=2.0/t; t=0.0; b1=1.0; b0=0.0;
    m=n+(int)sqrt(40.0*n);
    m=2*m;

    for (i=m; i>0; i--)
    {
        p=b0+i*y*b1; b0=b1; b1=p;
        if (fabs(b1)>1.0e+10)
        {
            t=t*1.0e-10; b0=b0*1.0e-10;
            b1=b1*1.0e-10;
        }
        if (i==n) t=b0;
    }
    p=t*q/b1;
    if ((x<0.0)&&(n%2==1)) p=-p;
    return(p);
}

//第二类修正贝塞尔曲线
double SecoundSubdivision::second_modified_Bessel(int n, double x)
{
    int i;
    double y,p,b0,b1;
    static double a[7]={ -0.57721566,0.4227842,0.23069756,
                         0.0348859,0.00262698,0.0001075,0.0000074};
    static double b[7]={ 1.0,0.15443144,-0.67278579,
                         -0.18156897,-0.01919402,-0.00110404,-0.00004686};
    static double c[7]={ 1.25331414,-0.07832358,0.02189568,
                         -0.01062446,0.00587872,-0.0025154,0.00053208};
    static double d[7]={ 1.25331414,0.23498619,-0.0365562,
                         0.01504268,-0.00780353,0.00325614,-0.00068245};
    if (n<0) n=-n;
    if (x<0.0) x=-x;
    if (x==0.0) return(1.0e+70);
    if (n!=1)
    {
        if (x<=2.0)
        {
            y=x*x/4.0; p=a[6];
            for (i=5; i>=0; i--) p=p*y+a[i];
            p=p-first_modified_Bessel(0,x)*log(x/2.0);
        }
        else
        {
            y=2.0/x; p=c[6];
            for (i=5; i>=0; i--) p=p*y+c[i];
            p=p*exp(-x)/sqrt(x);
        }
    }
    if (n==0) return(p);
    b0=p;
    if (x<=2.0)
    {
        y=x*x/4.0; p=b[6];
        for (i=5; i>=0; i--) p=p*y+b[i];
        p=p/x+first_modified_Bessel(1,x)*log(x/2.0);
    }
    else
    {
        y=2.0/x; p=d[6];
        for (i=5; i>=0; i--) p=p*y+d[i];
        p=p*exp(-x)/sqrt(x);
    }
    if (n==1) return(p);
    b1=p;
    y=2.0/x;
    for (i=1; i<n; i++)
    {
        p=b0+i*y*b1; b0=b1; b1=p;
    }
    return(p);
}









