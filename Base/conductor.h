#ifndef CONDUCTOR_H
#define CONDUCTOR_H

#include <QString>
#include <QCoreApplication>
#include <complex>


typedef std::complex<double> Complex;   //定义复数
class Point;


class Conductor
{
    Q_DECLARE_TR_FUNCTIONS(Conductor)
public:
    Conductor();
    Conductor(double X0, double Y0, double Z0,double X1, double Y1, double Z1,
              double R=0.0067056,double rou=1.7e-8,double U=1,QString mark=tr("铜"));

    //提供了不同的初始化函数
    void dataNodeInit(double X0, double Y0, double Z0,double X1, double Y1, double Z1);
    void dataNodeInit(double X0, double Y0, double Z0,double X1, double Y1, double Z1,
                      double R,double rou,double U,QString mark);
    void setR(double R);
    void setRou(double rou);
    void setU(double U);
    void setRemark(QString re);

    QString output();           //输出字符串
    void input(QString str);    //导入字符串

    bool operator == (const Conductor &other) const;
    bool operator > (const Conductor &other) const;
    bool operator < (const Conductor &other) const;

    double length();        //导体长度
    double dianzu();        //导体电阻(直流)
    double diandao();       //导体电导
    double neizigan();      //导体内自感（直流）
    std::complex<double>  neizukang(double f); //导体内阻抗
    std::complex<double>  neidaona(double f); //导体内导纳
    double dianzu(double f);//导体电阻(频率为f时)
    double neizigan(double f); //导体内自感(频率为f时)


    Conductor image();      //导体镜像
    Point centrePoint();    //导体中点
    Point P0();             //导体端点P0
    Point P1();             //导体端点P1

    double x0;
    double y0;
    double z0;
    double x1;
    double y1;
    double z1;
    double r;       //导体半径
    double rou;     //导体电阻率
    double u;       //导体相对磁导率
    QString remarks;//标注

private:
    Complex neizukang_fai(double x);    //求电阻的中间函数
    Complex neizukang_Z(double m);

    double ber(double x);
    double _ber(double x, int n);
    double ber1(double x);
    double _ber1(double x, int n);
    double bei(double x);
    double _bei(double x, int n);
    double bei1(double x);
    double _bei1(double x, int n);

    int jiecheng(int n);

};

#endif // CONDUCTOR_H
