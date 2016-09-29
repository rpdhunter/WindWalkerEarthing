#include "plot2ditembase.h"

Plot2dItemBase::Plot2dItemBase()
{
    setPen(QPen(Qt::black));
    setBrush(Qt::NoBrush);
    d_mode = Norm;      //默认显示幅值
}

void Plot2dItemBase::setPen(const QPen &pen)
{
    if ( pen != d_pen )
    {
        d_pen = pen;
        itemChanged();
    }
}

void Plot2dItemBase::setBrush(const QBrush &brush)
{
    if ( brush != d_brush )
    {
        d_brush = brush;
        itemChanged();
    }
}

void Plot2dItemBase::setRect(const QRectF &rect)
{
    if ( d_rect != rect )
    {
        d_rect = rect;
        itemChanged();
    }
}

void Plot2dItemBase::setMode(QString str)
{
    if(str == QObject::tr("幅角")){
        d_mode = Arg;
    }
    else if(str == QObject::tr("模和幅角")){
        d_mode = Norm_Arg;
    }
    else if(str == QObject::tr("实部")){
        d_mode = Real;
    }
    else if(str == QObject::tr("虚部")){
        d_mode = Imag;
    }
    else if(str == QObject::tr("实部和虚部")){
        d_mode = Real_Imag;
    }
    else{
        d_mode = Norm;      //默认显示幅值
    }
}

QRectF Plot2dItemBase::boundingRect() const
{
    return d_rect;
}

//用于统一根据模式显示复数的类
//point参数用于指定显示位置
//c参数用于指定显示的复数
//str参数用于在显示字符串后增加附加信息（比如A,V等单位），可以为空
void Plot2dItemBase::drawComplex(QPainter *painter, QPointF point, CurrentSource c, QString str) const
{
    if(d_mode == Norm){
        QwtPainter::drawText( painter, point, QString("%1").arg(c.norm())+str );
    }
    else if(d_mode == Arg){
        QwtPainter::drawText( painter, point, QString("%1°").arg(c.arg())+str );
    }
    else if(d_mode == Norm_Arg){
        QwtPainter::drawText( painter, point, QString("%1∠%2°").arg(c.norm()).arg(c.arg())+str );
    }
    else if(d_mode == Real){
        QwtPainter::drawText( painter, point, QString("%1").arg(c.real())+str );
    }
    else if(d_mode == Imag){
        QwtPainter::drawText( painter, point, QString("%1").arg(c.imag())+str );
    }
    else if(d_mode == Real_Imag){
        QwtPainter::drawText( painter, point, QString("%1 + %2j").arg(c.real()).arg(c.imag())+str );
    }
}
