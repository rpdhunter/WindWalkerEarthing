#include "plot3dwindow.h"
#include "ui_plot3dwindow.h"
#include <QtDebug>
#include <QTimer>
#include <QFileDialog>
#include <QSplitter>
#include <QLabel>
#include <QComboBox>

#include "functions.h"
#include "plotoptiondialog.h"

#include "qwt3d_io.h"
#include "qwt3d_io_gl2ps.h"
#include "qwt3d_io_reader.h"
#include "plot2d.h"

#include <QTextEdit>

using namespace Qwt3D;
using namespace std;

Plot3DWindow::Plot3DWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Plot3DWindow)
{
    ui->setupUi(this);

    mainDlg = static_cast<WindWalkerEarthingMainDlg *>(parent);

    QStringList list = mainDlg->currentProjectDir.split("/");
    this->setWindowTitle(list.last() + tr(" - 图形输出"));

    dataWidget_3D = new SurfacePlot(this);
    plot2d = new Plot2D(mainDlg->currentProjectDir,this);

    ui->tabWidget->addTab(dataWidget_3D,tr("3D图形显示"));
    ui->tabWidget->addTab(plot2d,tr("2D图形显示"));

    initActions();
    iniStatus();


    timer = new QTimer(this);
    connect(timer,SIGNAL(timeout()),this, SLOT(rotate()));
    //根据显示画面改变工具条
    connect(ui->tabWidget,SIGNAL(currentChanged(int)),this,SLOT(doToolbarChange(int)));

    //初始主视图显示地表电势
    ui->atn_Compute->trigger();
//    ui->toolBar_2D->setVisible(false);
//    ui->atn_Denggaoxian->setChecked(true);
//    ui->atn_Guangpu->setChecked(true);
//    ui->atn_
}

Plot3DWindow::~Plot3DWindow()
{
    delete ui;
}

//初始化菜单
void Plot3DWindow::initActions()
{
    maindataGroup = new QActionGroup(this);
    maindataGroup->addAction(ui->atn_Compute);
    maindataGroup->addAction(ui->atn_Touch);
    maindataGroup->addAction(ui->atn_Step);
    maindataGroup->addAction(ui->atn_Touch_In);

    maindataGroup->addAction(ui->atn_Boy);
    maindataGroup->addAction(ui->atn_Cone);
    maindataGroup->addAction(ui->atn_Dini);
    maindataGroup->addAction(ui->atn_Hat);
    maindataGroup->addAction(ui->atn_Ripple);
    maindataGroup->addAction(ui->atn_Torus);
    maindataGroup->addAction(ui->atn_Sombrero);
    maindataGroup->addAction(ui->atn_SeaShells);
    maindataGroup->addAction(ui->atn_Saddle);
    maindataGroup->addAction(ui->atn_Rosenbrock);
    connect(maindataGroup,SIGNAL(triggered(QAction*)),this,SLOT(createFunction(QAction*)));


    coordGroup = new QActionGroup(this);
    coordGroup->addAction(ui->atn_Box);
    coordGroup->addAction(ui->atn_Axis);
    coordGroup->addAction(ui->atn_AxisOff);
    connect(coordGroup,SIGNAL(triggered(QAction*)),this,SLOT(pickCoordSystem(QAction*)));

    plotstyleGroup = new QActionGroup(this);
    plotstyleGroup->addAction(ui->atn_WireFrame);
    plotstyleGroup->addAction(ui->atn_HiddenLine);
    plotstyleGroup->addAction(ui->atn_Polygon);
    plotstyleGroup->addAction(ui->atn_FilledMesh);
    plotstyleGroup->addAction(ui->atn_NoData);
    plotstyleGroup->addAction(ui->atn_PointStyle);
    connect(plotstyleGroup,SIGNAL(triggered(QAction*)),this,SLOT(pickPlotStyle(QAction*)));

    floorstyleGroup = new QActionGroup(this);
    floorstyleGroup->addAction(ui->atn_FloorData);
    floorstyleGroup->addAction(ui->atn_FloorISO);
    floorstyleGroup->addAction(ui->atn_FloorNone);
    connect(floorstyleGroup,SIGNAL(triggered(QAction*)),this,SLOT(pickFloorStyle(QAction*)));

    connect(ui->atn_Front,SIGNAL(toggled(bool)),this,SLOT(pickGrid()));
    connect(ui->atn_Back,SIGNAL(toggled(bool)),this,SLOT(pickGrid()));
    connect(ui->atn_Top,SIGNAL(toggled(bool)),this,SLOT(pickGrid()));
    connect(ui->atn_Bottom,SIGNAL(toggled(bool)),this,SLOT(pickGrid()));
    connect(ui->atn_Left,SIGNAL(toggled(bool)),this,SLOT(pickGrid()));
    connect(ui->atn_Right,SIGNAL(toggled(bool)),this,SLOT(pickGrid()));

    connect(ui->atn_Grad,SIGNAL(toggled(bool)),this,SLOT(showGrad(bool)));

    //以下为在工具栏上添加数据显示功能
    QLabel *label = new QLabel(tr("数据显示为 "));
    ui->toolBar_2D->addWidget(label);

    comboBox = new QComboBox;
    QStringList list;
    list << tr("模值") << tr("幅角") << tr("模和幅角")
         << tr("实部") << tr("虚部") << tr("实部和虚部");
    comboBox->addItems(list);
    ui->toolBar_2D->addWidget(comboBox);
    connect(comboBox,SIGNAL(currentIndexChanged(QString)),this,SLOT(doComboBoxChanged(QString)));

    //    //以下是最新使用的图形进入接口
    //    group_tuxing = new QButtonGroup;  //图形选择组
    //    group_weidu = new QButtonGroup;   //维度选择组

    //    group_tuxing->addButton(ui->rbtn_Step);
    //    group_tuxing->addButton(ui->rbtn_Surface);
    //    group_tuxing->addButton(ui->rbtn_Touch);
    //    group_tuxing->addButton(ui->rbtn_Touch_In);
    //    connect(group_tuxing,SIGNAL(buttonClicked(QAbstractButton*)),this,SLOT(replot()));

    //    group_weidu->addButton(ui->rbtn_2D);
    //    group_weidu->addButton(ui->rbtn_3D);
    //    connect(group_weidu,SIGNAL(buttonClicked(QAbstractButton*)),this,SLOT(replot()));

}

void Plot3DWindow::iniStatus()
{
    filenameWidget = new QLabel("\t", statusBar());
    statusBar()->addWidget(filenameWidget,0);
    dimWidget = new QLabel("", statusBar());
    statusBar()->addWidget(dimWidget,0);
    rotateLabel = new QLabel("", statusBar());
    statusBar()->addWidget(rotateLabel,0);
    shiftLabel = new QLabel("", statusBar());
    statusBar()->addWidget(shiftLabel,0);
    scaleLabel = new QLabel("", statusBar());
    statusBar()->addWidget(scaleLabel,0);
    zoomLabel = new QLabel("", statusBar());
    statusBar()->addWidget(zoomLabel,0);

    connect(dataWidget_3D, SIGNAL(rotationChanged(double,double,double)),this,SLOT(showRotate(double,double,double)));
    connect(dataWidget_3D, SIGNAL(vieportShiftChanged(double,double)),this,SLOT(showShift(double,double)));
    connect(dataWidget_3D, SIGNAL(scaleChanged(double,double,double)),this,SLOT(showScale(double,double,double)));
    connect(dataWidget_3D, SIGNAL(zoomChanged(double)),this,SLOT(showZoom(double)));
}
/*
//重绘主视图
void Plot3DWindow::replot()
{
    if(group_weidu->checkedButton()->text() == ui->rbtn_3D->text()){
        creat3DPlot(group_tuxing->checkedButton()->text());
    }
    else if(group_weidu->checkedButton()->text() == ui->rbtn_2D->text()){
        creat2DPlot(group_tuxing->checkedButton()->text());
    }
}


*/

//根据菜单选项生成主视图的函数
//这是一个相应maindataGroup的槽函数
void Plot3DWindow::createFunction(QAction *action)
{        
    dataWidget_3D->makeCurrent();
    dataWidget_3D->setTitle(maindataGroup->checkedAction()->text());   //设置标题
    dataWidget_3D->legend()->setScale(LINEARSCALE);

    for (unsigned i=0; i!=dataWidget_3D->coordinates()->axes.size(); ++i)
    {
        dataWidget_3D->coordinates()->axes[i].setMajors(7);
        dataWidget_3D->coordinates()->axes[i].setMinors(5);
    }

    QString name=action->text();
    plot2d->setData(name);

    if (name == QString(tr("地表电势")))
    {
        //生成图形函数
        ComputeShape shape(*dataWidget_3D,this->mainDlg->currentProjectDir);
        shape.create();
    }
    else if(name == tr("接触电压")){
        Touch touchShape(*dataWidget_3D,this->mainDlg->currentProjectDir);
        touchShape.create();

    }
    else if(name == tr("跨步电压")){
        Step stepShape(*dataWidget_3D,this->mainDlg->currentProjectDir);
        stepShape.create();
    }
    else if(name == tr("围墙内接触电压")){
        TouchIn touchInShape(*dataWidget_3D,this->mainDlg->currentProjectDir);
        touchInShape.create();
    }
    else if (name == QString("Rosenbrock"))
    {
        Rosenbrock rosenbrock(*dataWidget_3D);

        rosenbrock.setMesh(50,51);
        rosenbrock.setDomain(-1.73,1.55,-1.5,1.95);
        rosenbrock.setMinZ(-100);

        rosenbrock.create();

        dataWidget_3D->coordinates()->axes[Z1].setScale(LOG10SCALE);
        dataWidget_3D->coordinates()->axes[Z2].setScale(LOG10SCALE);
        dataWidget_3D->coordinates()->axes[Z3].setScale(LOG10SCALE);
        dataWidget_3D->coordinates()->axes[Z4].setScale(LOG10SCALE);
        dataWidget_3D->legend()->setScale(LOG10SCALE);
    }
    else if (name == QString("Hat"))
    {
        Hat hat(*dataWidget_3D);

        hat.setMesh(51,72);
        hat.setDomain(-1.5,1.5,-1.5,1.5);
        hat.create();
    }
    else if (name == QString("Ripple"))
    {
        Ripple ripple(*dataWidget_3D);
        ripple.setMesh(120,120);
        ripple.create();
    }
    else if (name == QString("Saddle"))
    {
        Saddle saddle;

        saddle.setMesh(71,71);
        double dom = 2.5;
        saddle.setDomain(-dom, dom, -dom, dom);
        saddle.assign(*dataWidget_3D);
        saddle.create();
    }
    else if (name == QString("Sombrero"))
    {
        Mex mex;

        mex.setMesh(91,91);
        double dom = 15;
        mex.setDomain(-dom, dom, -dom, dom);
        mex.create(*dataWidget_3D);
    }

    if (name == QString("Torus"))
    {
        Torus sf(*dataWidget_3D);
        sf.create();
    }
    else if (name == QString("SeaShell"))
    {
        Seashell ss(*dataWidget_3D);
        ss.create();
    }
    else if (name == QString("Boy"))
    {
        Boy boy(*dataWidget_3D);
        boy.create();
    }
    else if (name == QString("Dini"))
    {
        Dini dini(*dataWidget_3D);
        dini.create();
    }
    else if (name == QString("Cone"))
    {
        TripleField conepos;
        CellField conecell;
        //        createCone(conepos,conecell);
        dataWidget_3D->loadFromData(conepos, conecell);
    }
    on_atn_Fit_triggered();
//    for (unsigned i=0; i!=dataWidget_3D->coordinates()->axes.size(); ++i)
//    {
//        dataWidget_3D->coordinates()->axes[i].setMajors(7);
//        dataWidget_3D->coordinates()->axes[i].setMinors(5);
//    }

    //设置状态栏网格信息
    double a = dataWidget_3D->facets().first;
    double b = dataWidget_3D->facets().second;
    dimWidget->setText(tr("网格数 ") + QString::number(a*b)
                       + " (" + QString::number(a) + "x" + QString::number(b) +")" );

    updateColorLegend(7,5);

    dataWidget_3D->coordinates()->axes[X1].setLabelString(QString("X1"));
    dataWidget_3D->coordinates()->axes[X2].setLabelString(QString("X2"));
    dataWidget_3D->coordinates()->axes[X3].setLabelString(QString("X3"));
    dataWidget_3D->coordinates()->axes[X4].setLabelString(QString("X4"));

    dataWidget_3D->coordinates()->axes[Y1].setLabelString(QString("Y1"));
    dataWidget_3D->coordinates()->axes[Y2].setLabelString(QString("Y2"));
    dataWidget_3D->coordinates()->axes[Y3].setLabelString(QString("Y3"));
    dataWidget_3D->coordinates()->axes[Y4].setLabelString(QString("Y4"));

    dataWidget_3D->coordinates()->axes[Z1].setLabelString(QString("Z1"));
    dataWidget_3D->coordinates()->axes[Z2].setLabelString(QString("Z2"));
    dataWidget_3D->coordinates()->axes[Z3].setLabelString(QString("Z3"));
    dataWidget_3D->coordinates()->axes[Z4].setLabelString(QString("Z4"));

    pickCoordSystem(coordGroup->checkedAction());


}

//选择主视图的坐标系统
//这是一个响应coordGroup的槽函数
void Plot3DWindow::pickCoordSystem(QAction *action)
{
    if (action == ui->atn_Box)
        dataWidget_3D->setCoordinateStyle(BOX);
    if (action == ui->atn_Axis)
        dataWidget_3D->setCoordinateStyle(FRAME);
    if (action == ui->atn_AxisOff)
        dataWidget_3D->setCoordinateStyle(NOCOORD);
}

//选择主视图的绘制风格
//这是一个响应plotstyleGroup的槽函数
void Plot3DWindow::pickPlotStyle(QAction *action)
{
    if (action == ui->atn_Polygon)
    {
        dataWidget_3D->setPlotStyle(FILLED);
    }
    else if (action == ui->atn_FilledMesh)
    {
        dataWidget_3D->setPlotStyle(FILLEDMESH);
    }
    else if (action == ui->atn_WireFrame)
    {
        dataWidget_3D->setPlotStyle(WIREFRAME);
    }
    else if (action == ui->atn_HiddenLine)
    {
        dataWidget_3D->setPlotStyle(HIDDENLINE);
    }
    else if (action == ui->atn_PointStyle)
    {
        dataWidget_3D->setPlotStyle(Qwt3D::POINTS);
        //    Cone d(len,32);
        //    CrossHair d(0.003,0,true,false);
        //    dataWidget->setPlotStyle(d);
    }
    else{
        dataWidget_3D->setPlotStyle(NOPLOT);
    }
    dataWidget_3D->updateData();
    dataWidget_3D->updateGL();
}

//选择投影的绘制风格
//这是一个响应floorstyleGroup的槽函数
void Plot3DWindow::pickFloorStyle(QAction *action)
{
    if (action == ui->atn_FloorData)
    {
        dataWidget_3D->setFloorStyle(FLOORDATA);
    }
    else if (action == ui->atn_FloorISO)
    {
        dataWidget_3D->setFloorStyle(FLOORISO);
    }else
    {
        dataWidget_3D->setFloorStyle(NOFLOOR);
    }
    dataWidget_3D->updateData();
    dataWidget_3D->updateGL();
}

//选择在box模式下是否绘制网格
//响应ui->atn_Front..ui->atn_Bottem等的槽函数
//函数算法改进，使得更简洁明了
void Plot3DWindow::pickGrid()
{
    int sum = dataWidget_3D->coordinates()->grids();
    bool b;
    int s;
    for(int i=0;i<6;i++){
        s = 1 << i; //这里使用了移位，具体请看SIDE的定义
        switch((Qwt3D::SIDE)s){
        case LEFT:  b=ui->atn_Left->isChecked();break;
        case RIGHT: b=ui->atn_Right->isChecked();break;
        case CEIL:  b=ui->atn_Top->isChecked();break;
        case FLOOR: b=ui->atn_Bottom->isChecked();break;
        case FRONT: b=ui->atn_Front->isChecked();break;
        case BACK:  b=ui->atn_Back->isChecked();break;
        default:    b=false;
        }
        if (b)
            sum |= s;
        else
            sum &= ~s;
    }
    dataWidget_3D->coordinates()->setGridLines(sum!=Qwt3D::NOSIDEGRID, sum!=Qwt3D::NOSIDEGRID, sum);
    dataWidget_3D->updateGL();
}

//显示主视图图形的梯度（或许不是梯度，也许是类似的什么东东）
//ui->atn_Grad的槽函数
void Plot3DWindow::showGrad(bool val)
{
    dataWidget_3D->showNormals(val);
    dataWidget_3D->updateNormals();
    dataWidget_3D->updateGL();
}

//主视图还原
void Plot3DWindow::on_atn_Reset_triggered()
{
    on_atn_Fit_triggered();
    dataWidget_3D->setRotation(30,0,15);
    dataWidget_3D->setViewportShift(0,0);
    dataWidget_3D->setZoom(1);
}

//最适合视图
void Plot3DWindow::on_atn_Fit_triggered()
{
    double x_range = dataWidget_3D->coordinates()->axes[X1].length();      //求得坐标数值范围
    double y_range = dataWidget_3D->coordinates()->axes[Y1].length();
    double z_range = dataWidget_3D->coordinates()->axes[Z1].length();

    double a = x_range>y_range?x_range:x_range; //求得x，y的大者
    double b = z_range/a;

    if(b>2){
        dataWidget_3D->setScale(b/2,b/2,1/2.0);
    }
    else if(b<1){
        dataWidget_3D->setScale(1/1.5,1/1.5,1/b/3);
    }

    //设置坐标轴的长度
    for(int i=0;i<12;i++){
        dataWidget_3D->coordinates()->axes[i].setTicLength(5,3);
    }
}

//显示旋转
void Plot3DWindow::showRotate(double x, double y, double z)
{
    rotateLabel->setText(tr(" 角度 (")  + QString::number(x,'g',3) + " ,"
                         + QString::number(y,'g',3) + " ,"
                         + QString::number(z,'g',3) + ")");
}

void Plot3DWindow::showShift(double x, double y)
{
    shiftLabel->setText(tr(" 偏移 (") + QString::number(x,'g',3) + " ,"
                        + QString::number(y,'g',3) + " )"
                        );
}

void Plot3DWindow::showScale(double x, double y, double z)
{
    scaleLabel->setText(tr(" 尺度 (") + QString::number(x,'g',3) + " ,"
                        + QString::number(y,'g',3) + " ,"
                        + QString::number(z,'g',3) + ")");
}

void Plot3DWindow::showZoom(double z)
{
    zoomLabel->setText(tr(" 缩放 ")  + QString::number(z,'g',3));
}

 //相应修改显示数据的格式
void Plot3DWindow::doComboBoxChanged(QString str)
{
    plot2d->setMode(str);
}

//自旋
void Plot3DWindow::on_atn_Animation_toggled(bool arg1)
{
    if (arg1){
        timer->start( 50 ); // 每50ms旋转一度；
    }else{
        timer->stop();
    }
}
//水平(绕z轴)旋转一度
void Plot3DWindow::rotate()
{
//    dataWidget_3D->setRotation(
//                int(dataWidget_3D->xRotation() + 1) % 360,
//                int(dataWidget_3D->yRotation() + 1) % 360,
//                int(dataWidget_3D->zRotation() + 1) % 360);
    dataWidget_3D->setRotation(
                dataWidget_3D->xRotation(),
                dataWidget_3D->yRotation(),
                int(dataWidget_3D->zRotation() + 1) % 360);
}

//设置菜单切换
void Plot3DWindow::doToolbarChange(int index)
{
    if(index == 0)
    { //3D
        ui->toolBar_3D->setVisible(true);
        ui->toolBar_2D->setVisible(false);
    }
    else if(index == 1)
    {
        ui->toolBar_3D->setVisible(false);
        ui->toolBar_2D->setVisible(true);
    }
}

//重置图例
void Plot3DWindow::updateColorLegend(int majors, int minors)
{
    dataWidget_3D->legend()->setMajors(majors);
    dataWidget_3D->legend()->setMinors(minors);
    double start, stop;
    dataWidget_3D->coordinates()->axes[Z1].limits(start,stop);
    dataWidget_3D->legend()->setLimits(start, stop);
}




//选项
void Plot3DWindow::on_atn_Option_triggered()
{
    PlotOptionDialog dlg(dataWidget_3D);
    dlg.exec();
}

//正交
void Plot3DWindow::on_atn_Ortho_toggled(bool arg1)
{
    dataWidget_3D->setOrtho(arg1);
}

//开启光源
void Plot3DWindow::on_atn_Light_toggled(bool arg1)
{
    dataWidget_3D->enableLighting(arg1);
    dataWidget_3D->illuminate(0);
    dataWidget_3D->updateGL();
}

//图例
void Plot3DWindow::on_atn_Legend_toggled(bool arg1)
{
    dataWidget_3D->showColorLegend(arg1);
}

//自动刻度
void Plot3DWindow::on_atn_AutoScale_toggled(bool arg1)
{
    dataWidget_3D->coordinates()->setAutoScale(arg1);
    dataWidget_3D->updateGL();
}

//保存图像
void Plot3DWindow::on_atn_Save_triggered()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("保存文件"),
                                                    "/home/untitled.png",
                                                    tr("png file (*.png );;bmp file (*.bmp );;jpg file (*.jpg *.jpeg );;tif file (*.tif *.tiff )"));
    if(fileName.isEmpty())
        return;

    QStringList list=fileName.split(".");
    QString filetype=list.last();       //保存的扩展名
    //以下为IO接口代码
    VectorWriter* vw = (VectorWriter*)IO::outputHandler("PDF");
    if (vw)
        vw->setSortMode(VectorWriter::BSPSORT);
    IO::save(dataWidget_3D, fileName, filetype);


}

void Plot3DWindow::on_atn_Open_triggered()
{

    //    QString s = QFileDialog::getOpenFileName( this, "", "../../data", "GridData Files (*.mes *.MES)");

    //    if ( s.isEmpty() || !dataWidget)
    //        return;

    //    QFileInfo fi( s );
    //    filenameWidget->setToolTip(s);
    //    QString ext = fi.suffix();

    //    filenameWidget->setText(fi.fileName());
    //    qApp->processEvents(); // enforces repaint;

    //    if (IO::load(dataWidget, s, ext))
    //    {
    //        double a = dataWidget->facets().first;
    //        double b = dataWidget->facets().second;

    //        dimWidget->setText(QString("Cells ") + QString::number(a*b)
    //                           + " (" + QString::number(a) + "x" + QString::number(b) +")" );

    //        dataWidget->setResolution(3);
    //    }

    //    for (unsigned i=0; i!=dataWidget->coordinates()->axes.size(); ++i)
    //    {
    //        dataWidget->coordinates()->axes[i].setMajors(4);
    //        dataWidget->coordinates()->axes[i].setMinors(5);
    //        dataWidget->coordinates()->axes[i].setLabelString("");
    //    }

    //    updateColorLegend(4,5);
    //    pickCoordSystem(activeCoordSystem);
    //    dataWidget->showColorLegend(legend_);

    qDebug()<<"inputFormatList:"<<IO::inputFormatList();
    qDebug()<<"outputFormatList:"<<IO::outputFormatList();
}

//光谱图
void Plot3DWindow::on_atn_Guangpu_toggled(bool arg1)
{
    plot2d->showSpectrogram(arg1);
}

//显示等高线
void Plot3DWindow::on_atn_Denggaoxian_toggled(bool arg1)
{
    plot2d->showContour(arg1);
}

//显示灰阶
void Plot3DWindow::on_atn_Huijie_toggled(bool arg1)
{
    plot2d->showBlackWhite(arg1);
}

//显示电源电压
void Plot3DWindow::on_atn_PointVotage_toggled(bool arg1)
{
    plot2d->showPointVoltage(arg1);
}

//显示导体
void Plot3DWindow::on_atn_ShowConductor_toggled(bool arg1)
{
    plot2d->showConductor(arg1);
}

//显示导体信息
void Plot3DWindow::on_atn_ConductorText_toggled(bool arg1)
{
    plot2d->showConductorText(arg1);
}

//文本输出
void Plot3DWindow::on_atn_Text_triggered()
{
    QTextEdit *edit = new QTextEdit;

    QStringList list = mainDlg->currentProjectDir.split("/");
    //    qDebug()<< list;
    QFile file( mainDlg->currentProjectDir + "/" + tr("输出数据.txt"));
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    QTextStream in(&file);

    QString str = in.readAll();
    edit->setWindowTitle(tr("计算数据清单"));
    edit->setText(str);
    edit->resize(400,300);
    //    edit->setFontPointSize(50);
    edit->show();
}



