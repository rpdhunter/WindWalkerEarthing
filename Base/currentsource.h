#ifndef CURRENTSOURCE_H
#define CURRENTSOURCE_H

//表征一个电流源
//使用了std的comlex库
//也可用作复数的转换类
#include "point.h"
#include <complex>
#include <QString>

typedef std::complex<double> Complex;

class CurrentSource
{
public:
    CurrentSource(Point P=Point(0,0,0), Complex I=Complex(0,0), double f=50, QString remarks=QString());

    //由于操作复数，所以提供了比较全的接口，已备不时之需
    void setCurrent(Complex c);   //设置电流
    void setCurrent(double real,double imag);   //设置电流
    void setCurrent_arg(double norm, double arg);  //设置电流(模和幅角)
    std::complex<double> current();
    double real() const;      //实部
    double imag() const;      //虚部
    double norm() const;      //模
    double arg() const;       //幅角（角度制）

    QString output();           //输出字符串
    void input(QString str);    //导入字符串

    Point P;        //电流注入点11
    double f;       //电流频率
    QString remarks;//标识

private:
    Complex I;      //电流值（复数）
};

#endif // CURRENTSOURCE_H
