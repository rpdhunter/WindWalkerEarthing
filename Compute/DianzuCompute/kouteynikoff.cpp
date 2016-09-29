#include "kouteynikoff.h"
#include "Base/conductor.h"
#include "Base/point.h"
#include "math.h"

#include <QtDebug>

#include "armadillo"

using namespace arma;

const double pi = 3.1415926;

Kouteynikoff::Kouteynikoff(Conductor c)
    :JiFenBase(c)
{
    a = c.r;
    b = c.length();
}

//计算单根导体电荷圆柱形均与分布时，在某一点产生的电位
//这里的积分使用了自适应辛普生公式
//之所以形势比较复杂，是因为积分区间是(0,inf)
//被积函数F是一个递减函数，故使用了循环函数多次调用辛普生公式
double Kouteynikoff::Vp(Point P)
{
    trans(P);
    r = py;
    z = px - b/2;

    return Vp(r,z);
}

double Kouteynikoff::Vp(double R, double Z)
{
    r = R;
    z = Z;

    double eps = 0.001;  //精度
    double V = asr(static_cast<pFun>(&Kouteynikoff::F),0.001,100,eps),dV;
    int i=0;
    do{
        dV = asr(static_cast<pFun>(&Kouteynikoff::F),100*pow(10,i),100*pow(10,i+1),eps);  //几何级数增长计算区间
        V += dV;
        i++;
    }
    while(dV>eps);     //精度为eps

    //    double eps = 0.01;  //精度
    //    double V = ladder(static_cast<pFun>(&Kouteynikoff::F),0.001,100,1000),dV;
    //    int i=0;
    //    do{
    //        dV = asr(static_cast<pFun>(&Kouteynikoff::F),100*pow(10,i),100*pow(10,i+1),eps);  //几何级数增长计算区间
    //        V += dV;
    //        i++;
    //    }
    //    while(dV>eps);     //精度为eps

    return V / (pi*pi*a*b);
    //    return V / (pi*pi*c1.r*l);
}

//计算单根导体电荷圆柱形均匀分布时，在另一条导体（线段）上产生的平均电位
double Kouteynikoff::huzu(Conductor c2, int n)
{
    Point E(c2.x0,c2.y0,c2.z0);
    Point F(c2.x1,c2.y1,c2.z1);

    double sum = 0;

    for(int i=0;i<n;i++){
        Point S = Point::ChaZhi(E,F,i,n);
        sum += Vp(S);
    }
    return sum/n;
}

double Kouteynikoff::zizu(int n)
{
    double sum = 0;
    double h;
    if(n>1){
        h = l/(n-1)/2;
    }
    else{
        h = l;
        n = 1;
    }

    //    a = a/1.4;

    for(int i=0;i<n;i++){
        sum += Vp(c1.r,i*h);   //
    }
    return sum/n;
}

void Kouteynikoff::test()
{
    double l = 20;  //观测区间
    int n = 1000;   //观测点数
    mat A(3,n);

    double h = l/n; //步长
    for(int i=0;i<n;i++){
        A(0,i) = h+i*h;     //横坐标
        A(1,i) = second_modified_Bessel(0,h+i*h);
        A(2,i) = second_modified_Bessel(1,h+i*h);
    }
    A.save("second_modified_Bessel.mat", raw_ascii);
}

double Kouteynikoff::F(double x)
{
    double xr = x*r;
    if((r<1.5*a && x*r<0.001)||x==0)
        return 0;

    double S = second_modified_Bessel(0,x*r) / second_modified_Bessel(1,x*a);
    return S * cos(z*x) * sin(b*x/2) / x / x ;
}




//第一类修正贝赛尔曲线
double Kouteynikoff::first_modified_Bessel(int n, double x)
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
double Kouteynikoff::second_modified_Bessel(int n, double x)
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

