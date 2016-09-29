#include "oblineplot.h"
#include "armadillo"
#include <QtDebug>

#include <qwt_plot_grid.h>
#include <qwt_plot_marker.h>
#include <qwt_plot_curve.h>
#include <qwt_legend.h>
#include <qwt_plot_layout.h>
#include <qwt_legend_item.h>
#include <qwt_picker_machine.h>
#include <qwt_plot_zoomer.h>
#include <qwt_plot_panner.h>
#include <qwt_plot_renderer.h>
#include <qwt_symbol.h>

using namespace arma;

class Zoomer: public QwtPlotZoomer
{
public:
    Zoomer( int xAxis, int yAxis, QwtPlotCanvas *canvas ):
        QwtPlotZoomer( xAxis, yAxis, canvas )
    {
        setTrackerMode( QwtPicker::AlwaysOff );
        setRubberBand( QwtPicker::NoRubberBand );

        // RightButton: zoom out by 1
        // Ctrl+RightButton: zoom out to full size

        setMousePattern( QwtEventPattern::MouseSelect2,
            Qt::RightButton, Qt::ControlModifier );
        setMousePattern( QwtEventPattern::MouseSelect3,
            Qt::RightButton );
    }
};

ObLinePlot::ObLinePlot(QWidget *parent, ObservedLine line, QString name) :
    QwtPlot(parent)
{
    mat data;
    data.load(tr("%1.mat").arg(name).toStdString());
//    qDebug()<<data.n_cols<<data.n_rows;

    setTitle( name );
//    setCanvasBackground( QColor( Qt::yellow ) );

    plotLayout()->setAlignCanvasToScales( true );

    QwtLegend *legend = new QwtLegend;
    legend->setItemMode( QwtLegend::CheckableItem );
    insertLegend( legend, QwtPlot::RightLegend );

    QwtPlotGrid *grid = new QwtPlotGrid;
    grid->enableXMin( true );
    grid->enableYMin( true );
    grid->setMajPen( QPen( Qt::darkGray, 0, Qt::DotLine ) );
    grid->setMinPen( QPen( Qt::yellow, 0 , Qt::DotLine ) );
    grid->attach( this );

    picker = new QwtPlotPicker( QwtPlot::xBottom, QwtPlot::yLeft,
        QwtPlotPicker::CrossRubberBand, QwtPicker::AlwaysOn,
        this->canvas() );
    picker->setStateMachine( new QwtPickerDragPointMachine() );
    picker->setRubberBandPen( QColor( Qt::green ) );
    picker->setRubberBand( QwtPicker::CrossRubberBand );
    picker->setTrackerPen( QColor( Qt::black ) );



    setAxisTitle( QwtPlot::xBottom, tr("X / m") );
    setAxisTitle( QwtPlot::yLeft, tr("电位 / V") );

    int ArraySize = line.subsection() + 1;
    double *X = new double[ArraySize];
    double *V0 = new double[ArraySize];
    double *V1 = new double[ArraySize];
    double *V2 = new double[ArraySize];
    double *V3 = new double[ArraySize];

    for ( int i = 0; i < ArraySize; i++ )
    {
        X[i] = 0 + i*line.gap();
        V0[i] = 1000 * data.at(0,i);
        V1[i] = 1000 * data.at(1,i);
        V2[i] = 1000 * data.at(2,i);
        V3[i] = 1000 * data.at(3,i);
    }


    addCurve(tr("观测线电位"),X,V0,ArraySize,Qt::red);
    addCurve(tr("接触电压"),X,V1,ArraySize,Qt::green);
    addCurve(tr("围墙内接触电压"),X,V2,ArraySize,Qt::blue);
    addCurve(tr("跨步电压"),X,V3,ArraySize,Qt::cyan);

    connect( this, SIGNAL( legendChecked( QwtPlotItem *, bool ) ),
        SLOT( showItem( QwtPlotItem *, bool ) ) );

    zoomInit();

    //设置图例选择
    QwtPlotItemList items = itemList( QwtPlotItem::Rtti_PlotCurve  );
//    qDebug()<<items.size();
    for ( int i = 0; i < items.size(); i++ )
    {
        if ( i == 0 )
        {
            QwtLegendItem *legendItem =qobject_cast<QwtLegendItem *>( legend->find( items[i] ) );
            if ( legendItem )
                legendItem->setChecked( true );

            items[i]->setVisible( true );
        }
        else
            items[i]->setVisible( false );
    }

}

void ObLinePlot::showItem(QwtPlotItem *item, bool on)
{
    item->setVisible( on );
//    qDebug()<<on;
    this->replot();
}

void ObLinePlot::enableZoomMode(bool on)
{


    d_zoomer[0]->setEnabled( on );
    d_zoomer[0]->zoom( 0 );

    d_zoomer[1]->setEnabled( on );
    d_zoomer[1]->zoom( 0 );

    panner->setEnabled( on );

    picker->setEnabled( !on );

    replot();
}

void ObLinePlot::addBaseItems()
{

}

void ObLinePlot::addCurve(QString name,double *X,double *V,int ArraySize,QColor color)
{
    QwtPlotCurve *d_curve1 = new QwtPlotCurve( name );
    d_curve1->setRenderHint( QwtPlotItem::RenderAntialiased );
    d_curve1->setPen( QPen( color ) );
    d_curve1->setLegendAttribute( QwtPlotCurve::LegendShowLine );
    d_curve1->setYAxis( QwtPlot::yLeft );
    d_curve1->attach( this );
    d_curve1->setSamples(X,V,ArraySize);
//    d_curve1->setSymbol( new QwtSymbol( QwtSymbol::Cross, Qt::NoBrush,
//        QPen( Qt::black ), QSize( 5, 5 ) ) );

    //添加markers
//    addMarkers(X,V,ArraySize);    //功能暂时不开启

}

void ObLinePlot::addMarkers(double *X,double *V,int ArraySize)
{
    vec chafen(ArraySize-1);
    for(int i=0;i<ArraySize-1;i++){
        chafen(i) = V[i+1]-V[i];
    }
    QList<int> maxPoints;  //极大值
    QList<int> minPoints;  //极小值
    for(int i=0;i<chafen.n_rows-1;i++){
        if(chafen(i+1)*chafen(i)<=0){   //差分变号了，说明此处有极点
            if(chafen(i)>0){    //这是一个极大值
                maxPoints.append( i );
            }else if(chafen(i)<0){   //这是一个极小值
                minPoints.append( i );
            }
            //排除了等于0的情况
        }
    }

    //极大值
    foreach(int i,maxPoints){
        addMarker(X[i],V[i]);
    }

    //极小值
    foreach(int i,minPoints){
        addMarker(X[i],V[i]);
    }
}

void ObLinePlot::addMarker(double x,double y)
{
    QwtPlotMarker *d_marker1 = new QwtPlotMarker();
    d_marker1->setValue(x,y);
    d_marker1->setLineStyle( QwtPlotMarker::NoLine );
    d_marker1->setLabelAlignment( Qt::AlignRight | Qt::AlignBottom );
    d_marker1->setLinePen( QPen( Qt::blue, 0, Qt::DashDotLine ) );
    d_marker1->setSymbol( new QwtSymbol( QwtSymbol::XCross,
        QColor( Qt::yellow ), QColor( Qt::green ), QSize( 5, 5 ) ) );
    d_marker1->attach( this );
}

void ObLinePlot::zoomInit()
{
    d_zoomer[0] = new Zoomer( QwtPlot::xBottom, QwtPlot::yLeft,
        canvas() );
    d_zoomer[0]->setRubberBand( QwtPicker::RectRubberBand );
    d_zoomer[0]->setRubberBandPen( QColor( Qt::green ) );
    d_zoomer[0]->setTrackerMode( QwtPicker::ActiveOnly );
    d_zoomer[0]->setTrackerPen( QColor( Qt::white ) );

    d_zoomer[1] = new Zoomer( QwtPlot::xTop, QwtPlot::yRight,
         canvas() );

    panner = new QwtPlotPanner( canvas() );
    panner->setMouseButton( Qt::MidButton );

}














