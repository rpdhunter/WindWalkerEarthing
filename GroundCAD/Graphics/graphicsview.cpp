#include "graphicsview.h"
#include <QtDebug>
#include <QWheelEvent>
#include <QMouseEvent>
#include <qmath.h>
#include "graphicsscene.h"
#include "graphicslineitem.h"
#include "../GroundingCADWindow.h"

GraphicsView::GraphicsView(QWidget *parent) :
    QGraphicsView(parent)
{

    setContextMenuPolicy(Qt::ActionsContextMenu);

    setResizeAnchor(QGraphicsView::AnchorViewCenter);
//    setTransformationAnchor(QGraphicsView::NoAnchor);
    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
//    setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);

    setDragMode(QGraphicsView::RubberBandDrag);
    setRubberBandSelectionMode(Qt::IntersectsItemShape);

    setContextMenuPolicy(Qt::CustomContextMenu);

//    setRenderHint(QPainter::SmoothPixmapTransform);


    setMouseTracking(true);

    groundingCADWindow = qobject_cast<GroundingCADWindow*>(parent); //父窗口指针

    gridspace=100;
    transScale=1;

    isInPenMode=false;

    showOB=false;
}

void GraphicsView::zoom_out()
{
    scale(0.9,0.9);
    doScaleChanged();
}

void GraphicsView::zoom_in()
{
    scale(1.1,1.1);
    doScaleChanged();
}

void GraphicsView::zoom1()
{
    QTransform trans=this->transform();
    trans.reset();
    this->setTransform(trans);
    this->centerOn(0,0);
    doScaleChanged();
}

void GraphicsView::zoom_fit()
{
    QRectF rect1,rect2;
    rect1=this->scene()->itemsBoundingRect();
    rect2.setTopLeft(QPointF(rect1.left()-rect1.width()*0.1,rect1.top()-rect1.height()*0.1));
    rect2.setBottomRight(QPointF(rect1.right()+rect1.width()*0.1,rect1.bottom()+rect1.height()*0.1));
    this->fitInView(rect2,Qt::KeepAspectRatio);
    doScaleChanged();
}

//显示观测面
void GraphicsView::setShowOb(bool b)
{
    this->showOB = b;
    scale(0.9,0.9);
    scale(1/0.9,1/0.9);
    this->repaint();
}


//本函数完成鼠标滚轮事件的响应
//主要功能有：
//1.完成按滚轮方向缩放视图
//2.缩放时，保持鼠标位置的场景坐标不变（为了完成这一功能，使用了centerOn()函数，之前经过了较复杂的计算场景中心点的过程，不知道有无简化做法）
//3.缩放完成调用doScaleChanged()，设置坐标刻度
void GraphicsView::wheelEvent(QWheelEvent *event)
{
    QPointF p0=event->pos();  //鼠标位置
    QPointF p1(this->viewport()->rect().center());//视口中心点
    qreal dx=p1.x()-p0.x();
    qreal dy=p1.y()-p0.y();
    QPointF P0=this->mapToScene(p0.x(),p0.y());
    //开始比例变换
    int numDegrees = event->delta() / 8;    // 鼠标滚轮旋转的度数
    if(numDegrees>0){
        this->scale(1.1,1.1);
    }else{
        this->scale(1/1.1,1/1.1);
    }

    QPointF pp0=this->mapFromScene(P0);
    QPointF pp1(pp0.x()+dx,pp0.y()+dy);
    QPointF PP1=this->mapToScene(pp1.x(),pp1.y());
    this->centerOn(PP1);
    //改变坐标刻度
    doScaleChanged();
}



//本函数完成了视图窗口的重绘
//主要功能：
//1.绘制场景为白色，背景为灰色
//2.根据变量gridspace绘制坐标系统
//3.在视图窗口四周动态显示坐标刻度
void GraphicsView::paintEvent(QPaintEvent *event)
{
    QPainter painter(this->viewport());
    QRectF v=this->viewport()->rect();  //物理坐标矩形
    QRectF s(this->mapToScene(v.left(),v.top()),this->mapToScene(v.right(),v.bottom()));    //逻辑坐标矩形

    painter.setPen(Qt::lightGray);
    painter.setBrush(Qt::lightGray);
    painter.drawRect(v);
    painter.setPen(Qt::white);
    painter.setBrush(Qt::white);
    painter.drawRect(QRectF(this->mapFromScene(this->sceneRect().topLeft()),this->mapFromScene(this->sceneRect().bottomRight())));

    painter.setPen(Qt::yellow);
    double margin_x=s.x()-int(s.x()/gridspace)*gridspace;
    double margin_y=s.y()-int(s.y()/gridspace)*gridspace;

    QPointF p1,p2;

    for(int i=0;(i-1)*gridspace<s.width() ;i++)
    {
        p1=this->mapFromScene(s.left()-margin_x+i*gridspace,s.top());
        p2=this->mapFromScene(s.left()-margin_x+i*gridspace,s.bottom());
        painter.drawLine(p1,p2);
        painter.save();
        painter.setPen(Qt::gray);
        painter.drawText(p1.x()+1,10,tr("%1").arg(s.left()-margin_x+i*gridspace));
        painter.drawText(p1.x()+1,v.bottom(),tr("%1").arg(s.left()-margin_x+i*gridspace));
        painter.restore();
    }
    for(int i=0;(i-1)*gridspace<s.height() ;i++)
    {   p1=this->mapFromScene(s.left(),s.top()-margin_y+i*gridspace);
        p2=this->mapFromScene(s.right(),s.top()-margin_y+i*gridspace);
        painter.drawLine(p1,p2);
        painter.save();
        painter.setPen(Qt::gray);
        painter.drawText(0,p1.y(),tr("%1").arg(s.top()-margin_y+i*gridspace));
        painter.drawText(QRectF(v.right()-100,p1.y()-10,100,10),Qt::AlignRight,tr("%1").arg(s.top()-margin_y+i*gridspace));
        painter.restore();
    }
    painter.setPen(Qt::green);
    painter.drawLine(this->mapFromScene(s.left(),0),this->mapFromScene(s.right(),0));
    painter.drawLine(this->mapFromScene(0,s.top()),this->mapFromScene(0,s.bottom()));


    QList<QGraphicsItem *> list=this->scene()->selectedItems();
    if(list.count()>0){
        QGraphicsItem *item;
        QPolygonF polygon;
        foreach(item,list){
            QRectF R=item->mapRectToScene(item->boundingRect());
            polygon<<R.topLeft()<<R.bottomRight();
        }
        QRectF R1=this->mapFromScene(polygon).boundingRect();

        double x0,y0,x1,y1;
        if(R1.width()<20){
            x0=R1.center().x()-10;
            x1=R1.center().x()+10;
        }else{
            x0=R1.left()-4;
            x1=R1.right()+4;
        }
        if(R1.height()<20){
            y0=R1.center().y()-10;
            y1=R1.center().y()+10;
        }else{
            y0=R1.top()-4;
            y1=R1.bottom()+4;
        }

        painter.setPen(Qt::black);
        painter.setBrush(Qt::black);

        painter.drawRect(x0,y0,4,4);
        painter.drawRect(x1-4,y0,4,4);
        painter.drawRect(x0,y1-4,4,4);
        painter.drawRect(x1-4,y1-4,4,4);
        painter.drawRect(x0,(y0+y1)/2-2,4,4);
        painter.drawRect(x1-4,(y0+y1)/2-2,4,4);
        painter.drawRect((x0+x1)/2-2,y0,4,4);
        painter.drawRect((x0+x1)/2-2,y1-4,4,4);
    }

//    if(isInPenMode == true && !oldPos.isNull()){        //处理铅笔模式
//        painter.drawLine(oldPos,newPos);
//    }

    if(showOB == true){
        painter.setRenderHint(QPainter::Antialiasing);
        //画围墙
        painter.setPen(QPen(Qt::black,2,Qt::DashDotLine));
        painter.setBrush(Qt::NoBrush);
        painter.drawPolygon(mapFromScene(groundingCADWindow->weiqiang()));

        //画观测面        
        QRectF r(mapFromScene(groundingCADWindow->ob_rect.topLeft()),
                 mapFromScene(groundingCADWindow->ob_rect.bottomRight()));  //观测面矩形
        QColor color(Qt::darkGreen);
        color.setAlpha(50);
        painter.setBrush(color);
        painter.setPen(Qt::NoPen);
        painter.drawRect(r);

        painter.setPen(color);
        double x_space = r.width()/groundingCADWindow->ob_xmesh;
        double y_space = r.height()/groundingCADWindow->ob_ymesh;
        QVector<QLine> lineList;
        for(int i=0;i<=groundingCADWindow->ob_xmesh;i++){
            QLine line(r.left()+i*x_space,r.top(),r.left()+i*x_space,r.bottom());
            lineList << line;
        }
        for(int i=0;i<=groundingCADWindow->ob_ymesh;i++){
            QLine line(r.left(),r.top()+i*y_space,r.right(),r.top()+i*y_space);
            lineList << line;
        }
        painter.drawLines(lineList);

        //画观测线
        color.setAlpha(255);
        painter.setPen(color);
        lineList.clear();
        foreach(ObservedLine obLine,groundingCADWindow->observedLineList){
            QPoint p1 = mapFromScene(obLine.P1().x,obLine.P1().y);
            QPoint p2 = mapFromScene(obLine.P2().x,obLine.P2().y);
            QLine line(p1,p2);
            painter.drawLine(line);
            painter.drawText(p1,obLine.name());
        }
    }

    QGraphicsView::paintEvent(event);
}

void GraphicsView::mousePressEvent(QMouseEvent *event)
{
//    this->update();
    if(isInPenMode == true){        //处理铅笔模式
        if(oldPos.isNull() || oldPos==event->pos()){
            oldPos=event->pos();

        }else{
//            this->scene()->addLine(QLineF(this->mapToScene(oldPos.toPoint()),this->mapToScene(event->pos())));
            GraphicsScene *scene= static_cast<GraphicsScene *>(this->scene());
            QList<Conductor> list;
            Conductor c;
            QPointF p1=this->mapToScene(oldPos.toPoint());
            QPointF p2=this->mapToScene(event->pos());
            c.dataNodeInit(p1.x(),p1.y(),0.5,p2.x(),p2.y(),0.5);
            c.setR(groundingCADWindow->defultR);
            c.setRou(groundingCADWindow->defultRou);
            c.setU(groundingCADWindow->defultU);
            c.setRemark(groundingCADWindow->defultRemarks);
            list.append(c);
            scene->addConductor(list);
            scene->doItemChanged();
            oldPos=QPointF();

        }
    }
    QGraphicsView::mousePressEvent(event);
}

void GraphicsView::mouseReleaseEvent(QMouseEvent *event)
{
//    if(isInPenMode == true){        //处理铅笔模式
//        QpointF newPos=event->pos();

//    }
    QGraphicsView::mouseReleaseEvent(event);
}

//画观测线，在前景上
//void GraphicsView::drawForeground(QPainter *painter, const QRectF &rect)
//{

//}

void GraphicsView::drawBackground(QPainter *painter, const QRectF &rect)
{
//    if(showOB == true){
//        QColor color(Qt::darkGreen);
//        color.setAlpha(50);
//        painter->setBrush(color);
//        painter->setPen(Qt::NoPen);

//        QRectF r = this->scene()->itemsBoundingRect();
//        painter->drawRect(r.adjusted(-r.width()*0.25,-r.height()*0.25,
//                                     r.width()*0.25,r.height()*0.25));
//    }
    QGraphicsView::drawBackground(painter,rect);

}

//鼠标移动时提供鼠标位置
void GraphicsView::mouseMoveEvent(QMouseEvent *event)
{
    QPointF point=this->mapToScene(event->pos());
    posString=tr("%1  %2").arg(point.x()).arg(point.y());
    emit mousePosChanged(posString);

    if(isInPenMode == true){        //处理铅笔模式
        posLabel->move(event->pos()+QPoint(10, 5));
        newPos=event->pos();
    }


    QGraphicsView::mouseMoveEvent(event);
}





//本函数完成了根据映射比例改变显示的坐标刻度功能
//基本思路是：
//1.根据映射前后的窗口宽度，计算映射比例rate
//2.根据rate的值，计算出与之相适应的刻度值（刻度值表示成科学计数法，整数部分选取1,2,5三类）
void GraphicsView::doScaleChanged()
{
    QPointF p1=this->mapToScene(this->viewport()->rect().topLeft());
    QPointF p2=this->mapToScene(this->viewport()->rect().bottomRight());
    double rate=(p2.x()-p1.x())/this->viewport()->rect().width();

    double a=200*rate,space;
    int step=0;
    while(a>10||a<1){
        if(a>10){
            a=a/10;
            step++;
        }else{
            a=a*10;
            step--;
        }
    }
    int b;
    if(a>=1&&a<2){
        b=1;
    }else if(a>=2&&a<5){
        b=2;
    }else{
        b=5;
    }
    if(step>=0){
        space=b*qPow(10,step);
    }else{
        space=b*qPow(0.1,(-step));
    }

    gridspace=space;
    transScale=rate;

}

void GraphicsView::setPenMode(bool f)
{
    isInPenMode=f;
    if(isInPenMode==true){
        posLabel=new QLabel(this);
        posLabel->resize(250,10);
        connect(this,SIGNAL(mousePosChanged(QString)),posLabel,SLOT(setText(QString)));
        posLabel->show();
    }else{
        delete posLabel;
    }
}

//已废弃
void GraphicsView::setObRect(QRectF rect, int ob_mesh_x, int ob_mesh_y, double ob_z)
{
//    this->ob_rect = rect;
    scale(0.9,0.9);
    scale(1/0.9,1/0.9);
    this->repaint();
}




