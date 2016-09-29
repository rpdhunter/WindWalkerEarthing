#include "conductoritem.h"

ConductorItem::ConductorItem()
{
    d_showText = false;
}


void ConductorItem::setLine(const QLineF &line)
{
    if ( d_line != line )
    {
        d_line = line;
        itemChanged();
    }
}


void ConductorItem::draw(QPainter *painter, const QwtScaleMap &xMap, const QwtScaleMap &yMap, const QRectF &) const
{
    if ( !d_line.isNull() )
    {
        const QPointF p1 = QwtScaleMap::transform(xMap, yMap, d_line.p1());
        const QPointF p2 = QwtScaleMap::transform(xMap, yMap, d_line.p2());
        painter->setPen( d_pen );
        painter->setBrush( d_brush );
        QwtPainter::drawLine( painter, QLineF(p1,p2));
        if(d_showText){
            //显示编号
            QwtPainter::drawText( painter, QPointF( (p1.x()+p2.x())/2, (p1.y()+p2.y())/2 + 10),
                                  QString("#%1").arg(d_n));
            //显示电流
            drawComplex(painter,QPointF( (p1.x()+p2.x())/2, (p1.y()+p2.y())/2),d_I);
        }
    }
}

//void ConductorItem::setConductor(Conductor conductor, double I, int n)
//{
//    d_conductor = conductor;
//    d_I.setCurrent(I,0);  //设置实部
//    d_n = n;
//    setLine(QLineF(d_conductor.x0,d_conductor.y0,d_conductor.x1,d_conductor.y1));
//}

void ConductorItem::setConductor(Conductor conductor, Complex I, int n)
{
    d_conductor = conductor;
    d_I.setCurrent(I);
    d_n = n;
    setLine(QLineF(d_conductor.x0,d_conductor.y0,d_conductor.x1,d_conductor.y1));
}

void ConductorItem::showText(bool on)
{
    d_showText = on;
}





