#ifndef GRAPHICSLINEITEM_H
#define GRAPHICSLINEITEM_H

#include <QGraphicsLineItem>
#include "Base/conductor.h"

class GraphicsLineItem : public QGraphicsLineItem
{
public:
    GraphicsLineItem(Conductor c);

//    QRectF boundingRect();
    Conductor node();
    void setNode(Conductor c);

    void setFeatures(double R, double rou, double U, QString remarks);

public slots:
//    void setScale(double r);


protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    //QVariant itemChange(GraphicsItemChange change, const QVariant &value);

private:
    Conductor datanode;
};

#endif // GRAPHICSLINEITEM_H
