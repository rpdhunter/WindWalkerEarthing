#include "pointvoltageitem.h"


PointVoltageItem::PointVoltageItem()
{
    setPen( QPen(Qt::blue) );
}

void PointVoltageItem::setPoint(const Point P)
{
    if ( d_P.x != P.x || d_P.y != P.y || d_P.z != P.z  )
    {
        d_P = P;
        itemChanged();
    }
}

void PointVoltageItem::draw(QPainter *painter, const QwtScaleMap &xMap, const QwtScaleMap &yMap, const QRectF &) const
{
    const QPointF point = QwtScaleMap::transform(xMap, yMap, QPointF(d_P.x,d_P.y));
    painter->setPen( d_pen );
    painter->setBrush( d_brush );
    if(d_cs.f == -1){       //如果是电源点，设置笔为红色
        painter->setPen( QPen(Qt::red) );
        painter->setBrush( Qt::red );
    }

    QPointF p1 = point + QPointF(-2.0, -2.0);
    QPointF p2 = point + QPointF(2.0, 2.0);

    QwtPainter::drawEllipse(painter,QRectF(p1,p2));

    //显示文字信息
    drawComplex(painter,QRectF(p1,p2).topRight(),d_cs);

}

void PointVoltageItem::setPointVoltage(CurrentSource cs)
{
    d_cs = cs;
    setPoint(cs.P);
}
