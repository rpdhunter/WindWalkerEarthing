#include "jifenbase.h"
#include "math.h"
#include "Base/point.h"

//作赋值和映射工作
//整体构思是三维降到二维，但是单纯输入导体，无法确定y轴，故先决定横坐标
//(x0,y0,z0)(x1,y1,z1) ----> (0,0) (l,0)
JiFenBase::JiFenBase(Conductor c)
{
    c1 = c;
    l = c1.length();
}

//坐标变换，注意P点永远位于x轴上方
void JiFenBase::trans(Point P)
{    
    r1 = Point::length(Point(c1.x0,c1.y0,c1.z0),P);
    r2 = Point::length(Point(c1.x1,c1.y1,c1.z1),P);
    px = (l*l + r1*r1 - r2*r2) / (2*l);
    py = sqrt(r1*r1 - px*px);
}

//n点矩形公式(注意这里的矩形公式结果除以了(b-a)，目的是为了减少计算量)，以下的所有公式都是这样
double JiFenBase::rect(pFun F, double a, double b, int n)
{
    if(n<=0)
        n=1;
    double sum = 0;
    double h = (b-a)/n; //步长
    for(int i=0;i<n;i++){
        sum += (this->*F)(a+i*h+h/2);
    }

    return sum/n;
}

//n点梯形公式
double JiFenBase::ladder(pFun F, double a, double b, int n)
{
    if(n<=0)
        n=1;
    double sum = 0;
    double h = (b-a)/n; //步长
    for(int i=0;i<n;i++){
        sum += ((this->*F)(a+i*h)+(this->*F)(a+(i+1)*h))/2;
    }

    return sum/n;
}


//自适应simpson公式主过程(使用了函数指针)
double JiFenBase::asr(pFun F, double a, double b, double eps)
{
    double v = asr(F,a,b,eps,simpson(b-a,(this->*F)(a),(this->*F)(b),(this->*F)((b+a)/2)));
//    return v/(b-a);
    return v;
}

//自适应simpson公式递归过程
double JiFenBase::asr(pFun F, double a, double b, double eps, double A)
{
    double c=(a+b)/2;
    double fa,fb,fc,L,R;
    fa=(this->*F)(a);
    fb=(this->*F)(b);
    fc=(this->*F)(c);
    L=simpson(c-a,fa,fc,(this->*F)((c+a)/2));
    R=simpson(b-c,fc,fb,(this->*F)((b+c)/2));
    if(fabs(L+R-A)<=15*eps)
        return L+R+(L+R-A)/15;
    return asr(F,a,c,eps/2,L)+asr(F,c,b,eps/2,R);
}

//三点辛普森公式
double JiFenBase::simpson(double width, double fa, double fb, double fc)
{
    return (fb+fa+4*fc)*width/6;
}
