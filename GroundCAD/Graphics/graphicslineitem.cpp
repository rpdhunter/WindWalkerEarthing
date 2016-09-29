#include "graphicslineitem.h"
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QtDebug>

GraphicsLineItem::GraphicsLineItem(Conductor c)
{
    setFlags(ItemIsMovable|ItemIsSelectable);
    setNode(c);
    setCursor(Qt::SizeAllCursor);
}

//保证了datanode和item位置的一致性
Conductor GraphicsLineItem::node()
{
    datanode.x0=this->mapToScene(this->line().p1()).x();
    datanode.y0=this->mapToScene(this->line().p1()).y();
    datanode.x1=this->mapToScene(this->line().p2()).x();
    datanode.y1=this->mapToScene(this->line().p2()).y();
    return datanode;
}

void GraphicsLineItem::setNode(Conductor c)
{
    datanode=c;
    setLine(datanode.x0,datanode.y0,datanode.x1,datanode.y1);
}

void GraphicsLineItem::setFeatures(double R,double rou,double U,QString remarks)
{
    datanode.setR(R);
    datanode.setRou(rou);
    datanode.setU(U);
    datanode.setRemark(remarks);
}

//QRectF GraphicsLineItem::boundingRect()
//{
//    QRectF rect=QGraphicsLineItem::boundingRect();
//    return rect.adjusted(-4,-4,4,4);
//}

//void GraphicsLineItem::setScale(double r)
//{
//    scale=r;
//}

void GraphicsLineItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    if(this->isSelected())
        painter->setPen(Qt::red);
//    else
//        painter->setPen(Qt::black);
    painter->drawLine(line());    
}




