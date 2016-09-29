#include "functions.h"
#include <QTextStream>
#include <QMessageBox>
#include <QDebug>
#include "armadillo"
//#include "Base/compute.h"

using namespace arma;

//函数图形（3D和2D）显示的基类
//这里采用了众多派生类的做法，但是由于返回函数的高度一致（都是读取数据文件），以后也可考虑使用一个类的参数不同的做法？
FunctionBase::FunctionBase()
{
    I = 1;        //电流系数
}

//读取观测面
void FunctionBase::readObRect(QString fileName)
{
//    QString fileName = path +  "/" + tr("输入数据.txt");
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    QTextStream in(&file);    // read the data serialized from the file

    QString string;

    while(!in.atEnd())
    {
        string=in.readLine();
        //观测面
        if(string.contains(tr("观测面"))){
            string=in.readLine();
            QStringList list = string.split(",");
            if(list.count()>=4){
                ob_rect.setRect(list.at(0).toDouble(),list.at(1).toDouble(),list.at(2).toDouble(),list.at(3).toDouble());
            }
        }
    };
}

//图形初始化计算工作（2d和3d的共同操作）
void FunctionBase::shapeIni(QString path,char* name)
{
    QDir::setCurrent(path+tr("//计算文件"));
    matrix.load(name);

    readObRect( path+  "/" + tr("输入数据.txt") );

    x_mesh = matrix.n_cols;      //列数
    y_mesh = matrix.n_rows;      //行数


    x_space = ob_rect.width()/(matrix.n_cols-1);
    y_space = ob_rect.height()/(matrix.n_rows-1);

    max_P = I * max(max(matrix));   //设定z轴最大值（最小值视情况而定，暂时为0）

}

//数据返回（因为所有图形显示都用到，所以设置在基类中，为了代码简化）
double FunctionBase::data(double x, double y) const
{
    int col = qRound((x-ob_rect.x())/x_space);
    int row = qRound((y-ob_rect.y())/y_space);

    if(row<(int)matrix.n_rows && row>=0 && col<(int)matrix.n_cols && col>=0)
        return I*matrix(row,col);
    else
        return 0;
}

///////////////////////////////////////////////////
//          地表电势3D
///////////////////////////////////////////////////
ComputeShape::ComputeShape(SurfacePlot &pw, QString path)
    :Function(pw)
{
    shapeIni(path,"观测面电位.mat");

    setMesh(x_mesh,y_mesh);
    setDomain(ob_rect.left(),ob_rect.right(),ob_rect.top(),ob_rect.bottom());
    setMaxZ(max_P);
}

double ComputeShape::operator ()(double x, double y)
{
    return data(x,y);
}

///////////////////////////////////////////////////
//          地表电势2D
///////////////////////////////////////////////////
ComputeShape_2D::ComputeShape_2D(QString path)
{
    shapeIni(path,"观测面电位.mat");

    //设置边界
    setInterval( Qt::XAxis, QwtInterval( ob_rect.left(),ob_rect.right() ));
    setInterval( Qt::YAxis, QwtInterval( ob_rect.top(),ob_rect.bottom() ));
    setInterval( Qt::ZAxis, QwtInterval( 0, max_P ) );
}

double ComputeShape_2D::value(double x, double y) const
{
    return data(x,y);
}

///////////////////////////////////////////////////
//          接触电压3D
///////////////////////////////////////////////////
Touch::Touch(SurfacePlot &pw, QString path)
    :Function(pw)
{
    shapeIni(path,"接触电压.mat");

    setMesh(x_mesh,y_mesh);
    setDomain(ob_rect.left(),ob_rect.right(),ob_rect.top(),ob_rect.bottom());
    setMaxZ(max_P);
}

double Touch::operator ()(double x, double y)
{
    return data(x,y);
}

///////////////////////////////////////////////////
//          接触电压2D
///////////////////////////////////////////////////
Touch_2D::Touch_2D(QString path)
{
    shapeIni(path,"接触电压.mat");

    //设置边界
    setInterval( Qt::XAxis, QwtInterval( ob_rect.left(),ob_rect.right() ));
    setInterval( Qt::YAxis, QwtInterval( ob_rect.top(),ob_rect.bottom() ));
    setInterval( Qt::ZAxis, QwtInterval( 0, max_P ) );
}

double Touch_2D::value(double x, double y) const
{
    return data(x,y);
}


///////////////////////////////////////////////////
//          跨步电压3D
///////////////////////////////////////////////////
Step::Step(SurfacePlot &pw,QString path)
    :Function(pw)
{
    shapeIni(path,"跨步电压.mat");

    setMesh(x_mesh,y_mesh);
    setDomain(ob_rect.left(),ob_rect.right(),ob_rect.top(),ob_rect.bottom());
    setMaxZ(max_P);
}

double Step::operator ()(double x, double y)
{
    return data(x,y);
}


///////////////////////////////////////////////////
//          跨步电压2D
///////////////////////////////////////////////////
Step_2D::Step_2D(QString path)
{
    shapeIni(path,"跨步电压.mat");

    //设置边界
    setInterval( Qt::XAxis, QwtInterval( ob_rect.left(),ob_rect.right() ));
    setInterval( Qt::YAxis, QwtInterval( ob_rect.top(),ob_rect.bottom() ));
    setInterval( Qt::ZAxis, QwtInterval( 0, max_P ) );
}

double Step_2D::value(double x, double y) const
{
    return data(x,y);
}

///////////////////////////////////////////////////
//          接触电压(围墙内)3D
///////////////////////////////////////////////////
TouchIn::TouchIn(SurfacePlot &pw, QString path)
    :Function(pw)
{
    shapeIni(path,"围墙内接触电压.mat");

    setMesh(x_mesh,y_mesh);
    setDomain(ob_rect.left(),ob_rect.right(),ob_rect.top(),ob_rect.bottom());
    setMaxZ(max_P);
}

double TouchIn::operator ()(double x, double y)
{
    return data(x,y);
}

///////////////////////////////////////////////////
//          接触电压(围墙内)2D
///////////////////////////////////////////////////
TouchIn_2D::TouchIn_2D(QString path)
{
    shapeIni(path,"围墙内接触电压.mat");

    //设置边界
    setInterval( Qt::XAxis, QwtInterval( ob_rect.left(),ob_rect.right() ));
    setInterval( Qt::YAxis, QwtInterval( ob_rect.top(),ob_rect.bottom() ));
    setInterval( Qt::ZAxis, QwtInterval( 0, max_P ) );
}

double TouchIn_2D::value(double x, double y) const
{
    return data(x,y);
}


