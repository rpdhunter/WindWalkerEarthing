#include "conductor.h"
#include <qmath.h>
#include <QStringList>
#include "point.h"

const double pi = 3.14159;
const double miu = 4*pi*1e-7;      //真空中的磁导率
const double eps = 1e-7;       //计算精度

Conductor::Conductor()
{
    x0=0;
    x1=0;
    y0=0;
    y1=0;
    z0=0;
    z1=0;
    r=0.0067056;
    rou=1.7e-8;
    this->u = 1;
    this->remarks = tr("铜");
}

Conductor::Conductor(double X0, double Y0, double Z0, double X1, double Y1, double Z1,
                     double R, double rou, double U, QString mark)
{
    x0 = X0;
    y0 = Y0;
    z0 = Z0;
    x1 = X1;
    y1 = Y1;
    z1 = Z1;
    r  = R ;
    this->rou = rou;
    this->u = U;
    this->remarks = mark;
}

void Conductor::dataNodeInit(double X0, double Y0, double Z0, double X1, double Y1, double Z1)
{
    if(qAbs(X0)<0.00001)x0=0;else x0=X0;
    if(qAbs(Y0)<0.00001)y0=0;else y0=Y0;
    if(qAbs(Z0)<0.00001)z0=0;else z0=Z0;
    if(qAbs(X1)<0.00001)x1=0;else x1=X1;
    if(qAbs(Y1)<0.00001)y1=0;else y1=Y1;
    if(qAbs(Z1)<0.00001)z1=0;else z1=Z1;
}

void Conductor::dataNodeInit(double X0, double Y0, double Z0, double X1, double Y1, double Z1,
                             double R, double rou, double U, QString mark)
{
    if(qAbs(X0)<0.00001)x0=0;else x0=X0;
    if(qAbs(Y0)<0.00001)y0=0;else y0=Y0;
    if(qAbs(Z0)<0.00001)z0=0;else z0=Z0;
    if(qAbs(X1)<0.00001)x1=0;else x1=X1;
    if(qAbs(Y1)<0.00001)y1=0;else y1=Y1;
    if(qAbs(Z1)<0.00001)z1=0;else z1=Z1;

    r  = R ;
    this->rou = rou;
    this->u = U;
    this->remarks = mark;
}

void Conductor::setR(double R)
{
    r=R;
}

void Conductor::setRou(double rou)
{
    this->rou = rou;
}

void Conductor::setU(double U)
{
    this->u = U;
}

void Conductor::setRemark(QString re)
{
    this->remarks = re;
}

QString Conductor::output()
{
    return QString("%1,%2,%3,%4,%5,%6,%7,%8,%9,%10").arg(x0).arg(y0).arg(z0).arg(x1).arg(y1).arg(z1)
                                                    .arg(r).arg(rou).arg(u).arg(remarks);
}

void Conductor::input(QString str)
{
    QStringList list = str.split(",");

    if(list.count()<10){             //一行不够10个信息就补零(应当不会用到)
        int fill=10-list.count();
        for(int i=0;i<fill;i++){
            list.append("0");
        }
    }

    this->dataNodeInit(list.at(0).toDouble(),list.at(1).toDouble(),list.at(2).toDouble(),
                       list.at(3).toDouble(),list.at(4).toDouble(),list.at(5).toDouble(),
                       list.at(6).toDouble(),list.at(7).toDouble(),list.at(8).toDouble(),
                       list.at(9) );
}

bool Conductor::operator ==(const Conductor &other) const
{
    if(other.x0==this->x0 && other.x1==this->x1
            && other.y0==this->y0 && other.y1==this->y1
            && other.z0==this->z0 && other.z1==this->z1
            && other.r==this->r
            && other.u==this->u
            && other.rou==this->rou)
        return true;
    else
        return false;
}

//为了加速函数引入的导体比较函数
//2013-10-26引入了u,rou的比较，得以完善
bool Conductor::operator >(const Conductor &other) const
{
    if(this->x0 > other.x0){
        return true;
    }
    else if(this->x0 < other.x0){
        return false;
    }
    else{   //this->x0 == other.x0
        if(this->y0 > other.y0){
            return true;
        }
        else if(this->y0 < other.y0){
            return false;
        }
        else{   //this->y0 == other.y0
            if(this->z0 > other.z0){
                return true;
            }
            else if(this->z0 < other.z0){
                return false;
            }
            else{       //this->z0 == other.z0
                if(this->x1 > other.x1){
                    return true;
                }
                else if(this->x1 < other.x1){
                    return false;
                }
                else{       //this->x1 == other.x1
                    if(this->y1 > other.y1){
                        return true;
                    }
                    else if(this->y1 < other.y1){
                        return false;
                    }
                    else{   //this->y1 == other.y1
                        if(this->z1 > other.z1){
                            return true;
                        }
                        else if(this->z1 < other.z1){
                            return false;
                        }
                        else{   //this->z1 == other.z1
                            if(this->r > other.r){
                                return true;
                            }
                            else if(this->r < other.r){
                                return false;
                            }
                            else{   //this->r == other.r
                                if(this->u > other.u){
                                    return true;
                                }
                                else if(this->u < other.u){
                                    return false;
                                }
                                else{   //this->u == other.u
                                    if(this->rou > other.rou){
                                        return true;
                                    }
                                    else if(this->rou < other.rou){
                                        return false;
                                    }
                                    else{           //this->rou == other.rou
                                        return false;       //到这里是全部相等，*this == other
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

bool Conductor::operator <(const Conductor &other) const
{
    if(*this == other || *this > other){
        return false;
    }
    else{
        return true;
    }
}

double Conductor::length()
{
    return qSqrt(qPow((x0-x1),2)+qPow((y0-y1),2)+qPow((z0-z1),2));
}

double Conductor::dianzu()
{
    return rou*length()/pow(r,2)/pi;
}

double Conductor::diandao()
{
    return pi*pow(r,2)/rou/length();
}

//导体内自感（外自感需要介质信息，难以在此计算）
double Conductor::neizigan()
{
    return miu*u*length()/8/pi;
}

//导体电阻(频率为f时)
//m*r>8时，使用了《圆导线内阻抗的数值计算》吴命利，电工技术学报2004.3的结论
double Conductor::dianzu(double f)
{
    return neizukang(f).real();
}

double Conductor::neizigan(double f)
{
    return neizukang(f).imag();
}

std::complex<double> Conductor::neizukang(double f)
{
    double m = sqrt(2*pi*f*miu*u/rou);  //一个重要的中间变量
    if(m*r>=8){  //这是拟合方法计算
        return this->length()*neizukang_Z(m);
    }
    else{       //这是正常方法计算
        return this->length()*Complex(ber(m*r),bei(m*r))*Complex(0,m*rou)/Complex(ber1(m*r),bei1(m*r))/(2*pi*r);
    }
}

std::complex<double> Conductor::neidaona(double f)
{
//    Complex C = neizukang(f);
//    return 1/C;
}

//返回镜像导体
Conductor Conductor::image()
{
    Conductor c=*this;
    c.dataNodeInit(x0,y0,-z0,x1,y1,-z1);
    return c;
}

Point Conductor::centrePoint()
{
    return Point((x0+x1)/2,(y0+y1)/2,(z0+z1)/2);
}

Point Conductor::P0()
{
    return Point(x0,y0,z0);
}

Point Conductor::P1()
{
    return Point(x1,y1,z1);
}

Complex Conductor::neizukang_fai(double x)
{
    return Complex(0.7071068,0.7071068)+
            + Complex(-0.0625001,-0.0000001)*pow(8/x,1)
            + Complex(-0.0013813,0.0013811)*pow(8/x,2)
            + Complex(0.0000005,0.0002452)*pow(8/x,3)
            + Complex(0.0000346,0.0000338)*pow(8/x,4)
            + Complex(0.0000117,-0.0000024)*pow(8/x,5)
            + Complex(0.0000016,-0.0000032)*pow(8/x,6);
}

Complex Conductor::neizukang_Z(double m)
{
    return Complex(0,m*rou)/(2*pi*r*neizukang_fai(m*r));
}

double Conductor::ber(double x)
{
    double value = 1,temp;
    int n=1;
    do{
        temp = _ber(x,n);
        value += temp;
        n++;
    }
    while(temp>eps);
    return value;
}

double Conductor::_ber(double x,int n)
{
    return pow(-1,n)*pow(x/2,4*n)/pow(jiecheng(2*n),2);
}

double Conductor::ber1(double x)
{
    double value = 0,temp;
    int n=1;
    do{
        temp = _ber1(x,n);
        value += temp;
        n++;
    }
    while(temp>eps);
    return value;
}

double Conductor::_ber1(double x, int n)
{
    return 2*n*pow(-1,n)*pow(x/2,4*n-1)/pow(jiecheng(2*n),2);
}

double Conductor::bei(double x)
{
    double value = 0,temp;
    int n=1;
    do{
        temp = _bei(x,n);
        value += temp;
        n++;
    }
    while(temp>eps);
    return value;
}

double Conductor::_bei(double x, int n)
{
    return pow(-1,n+1)*pow(x/2,4*n-2)/pow(jiecheng(2*n-1),2);
}

double Conductor::bei1(double x)
{
    double value = 0,temp;
    int n=1;
    do{
        temp = _bei1(x,n);
        value += temp;
        n++;
    }
    while(temp>eps);
    return value;
}

double Conductor::_bei1(double x, int n)
{
    return (2*n-1)*pow(-1,n+1)*pow(x/2,4*n-3)/pow(jiecheng(2*n-1),2);
}

int Conductor::jiecheng(int n)
{
    register int i,f=1;  //定义寄存器变量

    for(i=1;i<=n;i++)
        f*=i;

    return f;
}
