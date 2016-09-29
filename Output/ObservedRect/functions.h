#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <math.h>
#include "qwt3d_parametricsurface.h"
#include "qwt3d_function.h"
#include "Base/conductor.h"
#include "Compute/computdialog.h"
#include <QCoreApplication>
#include "armadillo"
#include <qwt_plot_spectrogram.h>

using namespace arma;
using namespace Qwt3D;


///////////////////////////////////////////////////
//     所有数据生成函数的基类
///////////////////////////////////////////////////
class FunctionBase
{
    Q_DECLARE_TR_FUNCTIONS(FunctionBase)

public:
    FunctionBase();

    void readObRect(QString fileName);

    void shapeIni(QString path,char *name);
    double data(double x,double y) const;

    QList<Conductor> cutlist;   //切割导体清单
    QRectF ob_rect;              //观测面
    double I;                   //电流系数
    double x_space,y_space;
    int x_mesh,y_mesh;
    mat matrix;
    double max_P;

private:

};


///////////////////////////////////////////////////
//          地表电势3D
///////////////////////////////////////////////////
class ComputeShape : public Function , public FunctionBase
{
    Q_DECLARE_TR_FUNCTIONS(ComputeShape)

public:
    ComputeShape(SurfacePlot& pw,QString path);
    double operator()(double x, double y);

private:

};


///////////////////////////////////////////////////
//          地表电势2D
///////////////////////////////////////////////////
class ComputeShape_2D : public QwtRasterData , public FunctionBase
{
    Q_DECLARE_TR_FUNCTIONS(ComputeShape_2D)

public:
    ComputeShape_2D(QString path);
    virtual double value( double x, double y ) const;

private:

};


///////////////////////////////////////////////////
//          接触电压3D
///////////////////////////////////////////////////
class Touch : public Function , public FunctionBase
{
    Q_DECLARE_TR_FUNCTIONS(Touch)

public:
    Touch(SurfacePlot& pw,QString path);
    double operator()(double x, double y);

private:

};

///////////////////////////////////////////////////
//          接触电压2D
///////////////////////////////////////////////////
class Touch_2D : public QwtRasterData , public FunctionBase
{
    Q_DECLARE_TR_FUNCTIONS(Touch_2D)

public:
    Touch_2D(QString path);
    virtual double value( double x, double y ) const;

private:

};

///////////////////////////////////////////////////
//          接触电压(围墙内)3D
///////////////////////////////////////////////////
class TouchIn : public Function , public FunctionBase
{
    Q_DECLARE_TR_FUNCTIONS(TouchIn)

public:
    TouchIn(SurfacePlot& pw,QString path);
    double operator()(double x, double y);

private:

};

///////////////////////////////////////////////////
//          接触电压(围墙内)2D
///////////////////////////////////////////////////
class TouchIn_2D : public QwtRasterData , public FunctionBase
{
    Q_DECLARE_TR_FUNCTIONS(TouchIn_2D)

public:
    TouchIn_2D(QString path);
    virtual double value( double x, double y ) const;

private:

};

///////////////////////////////////////////////////
//          跨步电压3D
///////////////////////////////////////////////////
class Step : public Function , public FunctionBase
{
    Q_DECLARE_TR_FUNCTIONS(Step)

public:
    Step(SurfacePlot& pw,QString path);
    double operator()(double x, double y);

private:

};


///////////////////////////////////////////////////
//          跨步电压2D
///////////////////////////////////////////////////
class Step_2D : public QwtRasterData , public FunctionBase
{
    Q_DECLARE_TR_FUNCTIONS(Step_2D)

public:
    Step_2D(QString path);
    virtual double value( double x, double y ) const;

private:

};




/////////////////////////////////////////////////
//以下是测试函数
/////////////////////////////////////////////////
class Rosenbrock : public Function
{
public:

    Rosenbrock(SurfacePlot& pw)
        :Function(pw)
    {
    }

    double operator()(double x, double y)
    {
        return 0.7 * log10((1-x)*(1-x) + 10 * (y - x*x)*(y - x*x));
    }
    //	QString name() const {return "Almost {\\it Rosenbrock}\\,:\\quad$\\frac{\\ln((1-x)^2 + 100(y-x^2)^2)}{8}$";}
};

class Hat : public Function
{
public:

    Hat(SurfacePlot& pw)
        :Function(pw)
    {
        //setMinZ(0.3);
        setDomain(0,10,0,10);
    }

    double operator()(double x, double y)
    {
        return 1.0 / (x*x+y*y+0.5);
        //return x*x*y/(x*x*x*x+y*y);
    }
};

class Hat_2D : public QwtRasterData
{
public:
    Hat_2D(QString path){
        setInterval( Qt::XAxis, QwtInterval( -10,10 ));
        setInterval( Qt::YAxis, QwtInterval( -10,10 ));
        setInterval( Qt::ZAxis, QwtInterval( 0, 2 ) );
    }
    virtual double value( double x, double y ) const
    {
        return 1.0 / (x*x+y*y+0.5);
    }
};



class Ripple : public Function
{
public:

    Ripple(SurfacePlot& pw)
        :Function(pw)
    {
        double l = 24;
        setDomain(-l,l,-l,l);
    }

    double operator()(double x, double y)
    {
        return (cos(sqrt(x*x+y*y) + cos(sqrt(((x+.913*2*Qwt3D::PI)*(x+.913*2*Qwt3D::PI))+y*y))
                    + cos(sqrt(((x-.913*2*Qwt3D::PI)*(x-.913*2*Qwt3D::PI))+(y*y))))*4);
    }
};

class Saddle : public Function
{
public:

    Saddle()
        :Function()
    {
        //	setMaxZ(0.8);
    }

    double operator()(double x, double y)
    {
        return x*x - y*y;
    }
    //	QString name() const {return "$x^2-y^2$";}
};

class Mex : public Function
{
public:

    Mex()
        :Function()
    {
        //	setMaxZ(0.8);
    }

    double operator()(double x, double y)
    {
        double n = sqrt(x*x+y*y);

        if (n < DBL_MIN)
            return 20;

        return 20 * sin(sqrt(x*x+y*y)) / n;
    }
    //	QString name() const {return "$\\frac{20\\sin\\sqrt{x^2+y^2}}{\\sqrt{x^2+y^2}}$";}
};

class Torus : public ParametricSurface
{
public:

    Torus(SurfacePlot& pw)
        :ParametricSurface(pw)
    {
        setMesh(81,61);
        setDomain(-2*Qwt3D::PI, 0,-2*Qwt3D::PI,0);
        setPeriodic(true,true);
    }


    Triple operator()(double u, double v)
    {
        double x,y,z;
        double c = 1.9;
        x = (c + cos(v)) * cos(u);
        y = (c + cos(v)) * sin(u);
        z = sin(v) + cos(v);
        return Triple(x,y,z);
    }
};

class Seashell : public ParametricSurface
{
public:

    Seashell(SurfacePlot& pw)
        :ParametricSurface(pw)
    {
        setMesh(41,131);
        setDomain(0,2*Qwt3D::PI,0,2*Qwt3D::PI);
        setPeriodic(true,true);
    }


    Triple operator()(double u, double v)
    {
        double x,y,z;
        double a = 1;
        double b = 6;
        double c = 0.5;
        int n = 3;

        double f = v/(2*Qwt3D::PI);

        x = a*(1-f)*cos(n*v)*(1+cos(u)) + c*cos(n*v) ;
        y = a*(1-f)*sin(n*v)*(1+cos(u)) + c*sin(n*v) ;
        z = b*f + a*(1-f)*sin(u);
        return Triple(x,y,z);
    }
};

class Boy : public ParametricSurface
{
public:

    Boy(SurfacePlot& pw)
        :ParametricSurface(pw)
    {
        setMesh(141,131);
        setDomain(0,Qwt3D::PI,0,Qwt3D::PI);
        setPeriodic(true,true);
    }


    Triple operator()(double u, double v)
    {
        double x,y,z;
        double a = 2/3.;
        double b = sqrt(2.);

        x = a*(cos(u)*cos(2*v)+b*sin(u)*cos(v))*cos(u) / (b-sin(2*u)*sin(3*v));
        y = a*(cos(u)*sin(2*v)-b*sin(u)*sin(v))*cos(u) / (b-sin(2*u)*sin(3*v));
        z = b*cos(u)*cos(u) / (b-sin(2*u)*sin(2*v));

        return Triple(x,y,z);
    }
};

class Dini : public ParametricSurface
{
public:

    Dini(SurfacePlot& pw)
        :ParametricSurface(pw)
    {
        setMesh(141,35);
        setDomain(0,5*Qwt3D::PI,0.001, 2);
        setPeriodic(true,true);
    }


    Triple operator()(double u, double v)
    {
        double x,y,z;
        double a = 5;
        double b = 1;


        x=a*cos(u)*sin(v);
        y=a*sin(u)*sin(v);
        z=a*(cos(v)+log(tan(v/2)))+b*u;

        return Triple(x,y,z);
    }
};

void createCone(Qwt3D::TripleField& conepos, Qwt3D::CellField& conecell);

#endif // FUNCTIONS_H
