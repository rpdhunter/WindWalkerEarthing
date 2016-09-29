#ifndef POINTVOLTAGEITEM_H
#define POINTVOLTAGEITEM_H

#include "plot2ditembase.h"

class PointVoltageItem : public Plot2dItemBase
{
public:
    PointVoltageItem();

    void setPoint(const Point P);
    virtual void draw( QPainter *painter, const QwtScaleMap &xMap, const QwtScaleMap &yMap,
                       const QRectF & ) const;
    void setPointVoltage(CurrentSource cs);

private:
    Point d_P;
    CurrentSource d_cs;  //节点对象
};

#endif // POINTVOLTAGEITEM_H
