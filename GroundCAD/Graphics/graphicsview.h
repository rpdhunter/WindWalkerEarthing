#ifndef GRAPHICSVIEW_H
#define GRAPHICSVIEW_H

#include <QGraphicsView>
#include <QMap>
#include <QLabel>


class LineWidget;
class GroundingCADWindow;


class GraphicsView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit GraphicsView(QWidget *parent = 0);

    void zoom_out();
    void zoom_in();
    void zoom1();
    void zoom_fit();

    void setShowOb(bool b);

    bool showOB;    //是否显示观测面(注意是公有变量)

    
signals:
    void mousePosChanged(QString );
//    void drawTempline(QPointF,QPointF);

public slots:
    void doScaleChanged();
    void setPenMode(bool f);
    void setObRect(QRectF rect,int ob_mesh_x,int ob_mesh_y,double ob_z);

protected:
    void wheelEvent(QWheelEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void paintEvent(QPaintEvent *event);

    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

//    void drawForeground(QPainter *painter, const QRectF &rect);
    void drawBackground(QPainter *painter, const QRectF &rect);

private:
    GroundingCADWindow *groundingCADWindow; //父窗口指针

    double gridspace;
    double transScale;

    QString posString;
    QLabel *posLabel;
    LineWidget *widget; //用于实时显示铅笔线

    bool isInPenMode;   //是否处于铅笔模式
    QPointF oldPos;     //铅笔模式的初始点
    QPointF newPos;
//    QPointF newPos;

//    QRectF ob_rect;    //观测面范围(逻辑坐标)
    
};



#endif // GRAPHICSVIEW_H
