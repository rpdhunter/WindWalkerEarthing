#ifndef OBLINEPLOT_H
#define OBLINEPLOT_H

#include <qwt_plot.h>
#include "Base/observedline.h"

class QwtPlotZoomer;
class QwtPlotPanner;
class QwtPlotPicker;


class ObLinePlot : public QwtPlot
{
    Q_OBJECT
public:
    explicit ObLinePlot(QWidget *parent,ObservedLine line,QString name);
    
signals:
    
public slots:
    void showItem( QwtPlotItem * item, bool on );

    void enableZoomMode(bool on);    //改成zoom模式
private:
    QwtPlotZoomer *d_zoomer[2];
    QwtPlotPanner *panner;
    QwtPlotPicker *picker;

    void addBaseItems();

    void addCurve(QString name, double *X, double *V, int ArraySize, QColor color);

    void addMarkers(double *X,double *V,int ArraySize);
    void addMarker(double x, double y);
    void zoomInit();
//    void showPeak( double x, double y );
};

#endif // OBLINEWINDOW_H
