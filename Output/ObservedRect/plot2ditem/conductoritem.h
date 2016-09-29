#ifndef CONDUCTORITEM_H
#define CONDUCTORITEM_H

#include "plot2ditembase.h"
#include "Base/conductor.h"


//用于显示导体形状的类

class ConductorItem : public Plot2dItemBase
{
public:
    ConductorItem();

    void setLine(const QLineF &line );
    virtual void draw( QPainter *painter, const QwtScaleMap &xMap, const QwtScaleMap &yMap,
        const QRectF & ) const;

//    void setConductor(Conductor conductor, double I, int n);
    void setConductor(Conductor conductor, Complex I, int n);

    void showText(bool on);

private:
    QLineF d_line;
    Conductor d_conductor;  //导体对象
    CurrentSource d_I;     //泄漏电流，这里之所以不直接用Complex类，是因为我这个版本的复数类不完善，无法实现模和幅角的输出
    int d_n;        //导体编号
    bool d_showText;    //是否显示导体编号，泄漏电流等    
};

#endif // CONDUCTORITEM_H
