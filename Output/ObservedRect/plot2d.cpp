#include "plot2d.h"
#include "functions.h"

#include <qprinter.h>
#include <qprintdialog.h>
#include <qwt_color_map.h>
#include <qwt_plot_spectrogram.h>
#include <qwt_scale_widget.h>
#include <qwt_scale_draw.h>
#include <qwt_plot_zoomer.h>
#include <qwt_plot_panner.h>
#include <qwt_plot_layout.h>
#include <qwt_plot_renderer.h>

#include "armadillo"

using namespace arma;

//设置鼠标（跟踪器）外形的函数
class MyZoomer: public QwtPlotZoomer
{
public:
    QwtPlotSpectrogram *_spectrogram;   //定义一个变量

    MyZoomer( QwtPlotCanvas *canvas ,QwtPlotSpectrogram *d_spectrogram):
        QwtPlotZoomer( canvas )
    {
        setTrackerMode( AlwaysOn );     //鼠标处永远显示坐标
        setRubberBand(QwtPicker::RectRubberBand);

        _spectrogram = d_spectrogram;
    }

    virtual QwtText trackerTextF( const QPointF &pos ) const
    {
        QColor bg( Qt::white );
        bg.setAlpha( 200 );

        QwtText text = QwtPlotZoomer::trackerTextF( pos );  //将点位置转换成相应字符串的函数
        //以下几行代码加入显示当前图形在某一点的函数值
        double x,y;
        x = text.text().split(",").at(0).toDouble();
        y = text.text().split(",").at(1).toDouble();
        double z = _spectrogram->data()->value(x,y);
        text.setText(text.text().prepend(QString("V = %1\n").arg(z)));
        text.setBackgroundBrush( QBrush( bg ) );
        return text;
    }
};

//提供了要显示的原始数据（和qwtPlot3d是基本一样的）
//因为数据生成的虚函数必须实化，必须派生
class SpectrogramData: public QwtRasterData
{
public:
    SpectrogramData()
    {
        //设置边界
        setInterval( Qt::XAxis, QwtInterval( -1.5, 1.5 ) );
        setInterval( Qt::YAxis, QwtInterval( -1.5, 1.5 ) );
        setInterval( Qt::ZAxis, QwtInterval( 0.0, 10.0 ) );
    }

    virtual double value( double x, double y ) const
    {
        const double c = 0.842;

        const double v1 = x * x + ( y - c ) * ( y + c );
        const double v2 = x * ( y + c ) + x * ( y + c );

        return 1.0 / ( v1 * v1 + v2 * v2 );
    }
};

//色彩条（貌似不必从新派生）
class ColorMap: public QwtLinearColorMap
{
public:
    ColorMap():
        QwtLinearColorMap( Qt::blue, Qt::red )
    {
        addColorStop( 0.1, Qt::cyan );
        addColorStop( 0.5, Qt::green );
        addColorStop( 0.8, Qt::yellow );
    }
};

//显示的主窗口
Plot2D::Plot2D( QString str, QWidget *parent ):
    QwtPlot( parent )
{
    path = str;


    d_spectrogram = new QwtPlotSpectrogram();   //画光谱图的类
    d_spectrogram->setRenderThreadCount( 0 ); // use system specific thread count
    d_spectrogram->setColorMap( new ColorMap() );
    d_spectrogram->setData( new ComputeShape_2D(str) );

    d_spectrogram->attach( this );

    //设置坐标间隔
    QwtInterval interval = d_spectrogram->interval(Qt::ZAxis);
    QList<double> contourLevels;
    for ( double level = 0; level < interval.maxValue(); level += ( interval.maxValue() - interval.minValue() )/ 20 )
        contourLevels += level;
    d_spectrogram->setContourLevels( contourLevels );

    addAxisWidget();

    plotLayout()->setAlignCanvasToScales( true );
    replot();

    // LeftButton for the zooming
    // MidButton for the panning
    // RightButton: zoom out by 1
    // Ctrl+RighButton: zoom out to full size

    QwtPlotZoomer* zoomer = new MyZoomer( canvas() ,d_spectrogram);
    zoomer->setMousePattern( QwtEventPattern::MouseSelect2,
        Qt::RightButton, Qt::ControlModifier );
    zoomer->setMousePattern( QwtEventPattern::MouseSelect3,
        Qt::RightButton );

    QwtPlotPanner *panner = new QwtPlotPanner( canvas() );
    panner->setAxisEnabled( QwtPlot::yRight, false );
    panner->setMouseButton( Qt::MidButton );

    // Avoid jumping when labels with more/less digits
    // appear/disappear when scrolling vertically

    const QFontMetrics fm( axisWidget( QwtPlot::yLeft )->font() );
    QwtScaleDraw *sd = axisScaleDraw( QwtPlot::yLeft );
    sd->setMinimumExtent( fm.width( "100.00" ) );

    const QColor c( Qt::darkBlue );
    zoomer->setRubberBandPen( c );
    zoomer->setTrackerPen( c );

    //显示导体
    this->readCutConductor();
    this->readPointVoltage();

    QDir::setCurrent(path + "/" + tr("计算文件"));
    cx_vec cx_I;        //这里只显示复数了
    bool flag;
    flag = cx_I.load("电流分布cx");
    QDir::setCurrent(path);

    for(int i=0;i<cutlist.count();i++){
        ConductorItem *item = new ConductorItem;
        item->setConductor(cutlist.at(i),cx_I.at(i),i);
        item->setRenderHint( QwtPlotItem::RenderAntialiased, true );
        item->attach(this);
        cutConductorlist.append(item);
    }

    for(int i=0;i<pointvoltagelist.count();i++){
        PointVoltageItem *item = new PointVoltageItem;
        item->setPointVoltage(pointvoltagelist.at(i));
        item->setRenderHint( QwtPlotItem::RenderAntialiased, true );
        item->attach(this);
        pointvoltageitemlist.append(item);
    }

    //初始化
    showBlackWhite(false);
    showConductor(false);
    showConductorText(false);
    showPointVoltage(false);

}

//设置数据
void Plot2D::setData(QString shape)
{
    if(shape == tr("地表电势")){
        d_spectrogram->setData( new ComputeShape_2D(path) );
    }
    else if(shape == tr("跨步电压")){
        d_spectrogram->setData( new Step_2D(path) );
    }
    else if(shape == tr("接触电压")){
        d_spectrogram->setData( new Touch_2D(path) );
    }
    else if(shape.contains(tr("围墙内"))){
        d_spectrogram->setData( new TouchIn_2D(path) );
    }
    else if(shape == "Hat"){
        d_spectrogram->setData( new Hat_2D(path) );
    }
    resetView();
    replot();
}

void Plot2D::showContour(bool on)
{
    d_spectrogram->setDisplayMode( QwtPlotSpectrogram::ContourMode, on );
    replot();
}

void Plot2D::showSpectrogram(bool on)
{
    d_spectrogram->setDisplayMode( QwtPlotSpectrogram::ImageMode, on );
    d_spectrogram->setDefaultContourPen( on ? QPen() : QPen( Qt::NoPen ) );
    replot();
}

void Plot2D::showBlackWhite(bool on)
{
    if(!on)
        d_spectrogram->setColorMap( new ColorMap() );
    else{
        QColor c1(Qt::white),c2(Qt::darkGray);
        c1.setAlphaF(0.5);
        c2.setAlphaF(0.5);  //貌似没用
        d_spectrogram->setColorMap( new QwtLinearColorMap(c1,c2) );
    }
    replot();
}

void Plot2D::showConductor(bool on)
{
    for(int i=0;i<cutConductorlist.count();i++){
        cutConductorlist.at(i)->setVisible(on);
    }
    d_spectrogram->setAlpha(155+int(!on)*100);
    replot();
}

void Plot2D::showConductorText(bool on)
{
    for(int i=0;i<cutConductorlist.count();i++){
        cutConductorlist.at(i)->showText(on);
    }
    replot();
}

void Plot2D::showPointVoltage(bool on)
{
    for(int i=0;i<pointvoltageitemlist.count();i++){
        pointvoltageitemlist.at(i)->setVisible(on);
    }
    d_spectrogram->setAlpha(155+int(!on)*100);
    replot();
}

void Plot2D::setMode(QString str)
{
    for(int i=0;i<cutConductorlist.count();i++){
        cutConductorlist.at(i)->setMode(str);
    }
    for(int i=0;i<pointvoltageitemlist.count();i++){
        pointvoltageitemlist.at(i)->setMode(str);
    }
    replot();
}



//重置视图上一些元素
void Plot2D::resetView()
{
    QwtInterval interval = d_spectrogram->interval(Qt::ZAxis);
    QList<double> contourLevels;
    for ( double level = 0; level < interval.maxValue(); level += ( interval.maxValue() - interval.minValue() )/ 20 )
        contourLevels += level;
    d_spectrogram->setContourLevels( contourLevels );

    //重置图例
//    const QwtInterval zInterval = d_spectrogram->data()->interval( Qt::ZAxis ); //返回了曲线z轴数值的间隔
//    rightAxisWidget->setColorMap( zInterval, new ColorMap() );
    addAxisWidget();

}

//添加一个坐标部件
void Plot2D::addAxisWidget()
{
    const QwtInterval zInterval = d_spectrogram->data()->interval( Qt::ZAxis ); //返回了曲线z轴数值的间隔
    // A color bar on the right axis
    rightAxisWidget = axisWidget( QwtPlot::yRight );      //设置一个比例尺部件
    rightAxisWidget->setTitle( tr("电位") );
    rightAxisWidget->setColorBarEnabled( true );
    rightAxisWidget->setColorMap( zInterval, new ColorMap() );

    setAxisScale( QwtPlot::yRight, zInterval.minValue(), zInterval.maxValue() );
    enableAxis( QwtPlot::yRight );
}

//读取切割导体
void Plot2D::readCutConductor()
{
    QString fileName = path + "/" + tr("计算文件") + "/" + tr("切割.txt");
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    QTextStream in(&file);    // read the data serialized from the file

    QString string;

    while(!in.atEnd())
    {
        string=in.readLine();
        Conductor c;
        c.input(string);
        this->cutlist.append(c);
    };

}

void Plot2D::readPointVoltage()
{
    QString fileName = path + "/" + tr("计算文件") + "/" + tr("节点电压.txt");
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    QTextStream in(&file);    // read the data serialized from the file

    QString string;

    while(!in.atEnd())
    {
        string=in.readLine();
        CurrentSource c;
        c.input(string);
        this->pointvoltagelist.append(c);
    };
}
