#include "currentsource.h"
#include <math.h>
#include <QStringList>

const double pi = 3.1416;

CurrentSource::CurrentSource(Point P, Complex I, double f, QString remarks)
{
    this->P = P;
    this->I = I;
    this->f = f;
    this->remarks = remarks;
}

void CurrentSource::setCurrent(Complex c)
{
    this->I = c;
}

void CurrentSource::setCurrent(double real, double imag)
{
    this->I = Complex(real,imag);
}

void CurrentSource::setCurrent_arg(double norm, double arg)
{
    double real = norm*cos(arg*pi/180);
    double imag = norm*sin(arg*pi/180);
    this->I = Complex(real,imag);
}

std::complex<double> CurrentSource::current()
{
    return this->I;
}

double CurrentSource::real() const
{
    return this->I.real();
}

double CurrentSource::imag() const
{
    return this->I.imag();
}

double CurrentSource::norm() const
{
    double r = I.real();
    double i = I.imag();
    return sqrt(r*r+i*i);
}

//幅角变化在[-180°-180°)
double CurrentSource::arg() const
{
    double r = I.real();
    double i = I.imag();
    double l = norm();

    if(l==0){
        return 0;
    }
    else if(i>=0){
        return acos(r/l)*180/pi;
    }
    else{
        return (-acos(r/l))*180/pi;
    }
}

QString CurrentSource::output()
{    
    return QString("%1,%2,%3,%4,%5,%6,%7").arg(P.x).arg(P.y).arg(P.z).arg(real()).arg(imag()).arg(f).arg(remarks);
}

void CurrentSource::input(QString str)
{
    QStringList list = str.split(",");

    if(list.count()<7){             //一行不够7个信息就补零(应当不会用到)
        int fill=7-list.count();
        for(int i=0;i<fill;i++){
            list.append("0");
        }
    }

    P = Point(list.at(0).toDouble(),list.at(1).toDouble(),list.at(2).toDouble());
    I = Complex(list.at(3).toDouble(),list.at(4).toDouble());
    f = list.at(5).toDouble();
    remarks = list.at(6);
}
