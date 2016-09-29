#ifndef PLOT2DITEMBASE_H
#define PLOT2DITEMBASE_H

#include <QPen>
#include <qglobal.h>
#include <qpainter.h>
#include <qwt_plot_grid.h>
#include <qwt_plot_canvas.h>
#include <qwt_plot_layout.h>
#include <qwt_interval.h>
#include <qwt_painter.h>
#include <qwt_plot_item.h>
#include "Base/currentsource.h"

class Plot2dItemBase : public QwtPlotItem
{
public:
    Plot2dItemBase();

    enum Mode{          //复数的显示模式
        Norm,       //模值
        Arg,        //幅角
        Norm_Arg,   //模和幅角
        Real,       //实部
        Imag,       //虚部
        Real_Imag   //实部和虚部
    };

    void setPen( const QPen &pen );
    void setBrush( const QBrush &brush );
    void setRect( const QRectF &rect );
    void setMode(QString str);      //设置复数显示模式

    virtual QRectF boundingRect() const;

    void drawComplex(QPainter *painter, QPointF point, CurrentSource c, QString str = "") const;        //绘制复数文字

protected:
    QPen d_pen;
    QBrush d_brush;
    QRectF d_rect;
    Mode d_mode;    //复数的显示模式
};

#endif // PLOT2DITEMBASE_H
