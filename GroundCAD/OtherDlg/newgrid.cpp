#include "newgrid.h"
#include "ui_newgrid.h"
#include <QDebug>
#include <QMessageBox>
#include <math.h>
#include "featuresdialog.h"

//////////////////////////////////////////
//本类完成了添加对象对话框的生成和相关控制计算
//////////////////////////////////////////

NewGrid::NewGrid(GraphicsScene *scene,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewGrid)
{
    ui->setupUi(this);
    ui->listWidget->setCurrentRow(1);

    groundingCADWindow = qobject_cast<GroundingCADWindow*>(parent);

    //设置预览图
    this->scene=scene;
    datalist=scene->datalist();
    myscene = new GraphicsScene;
    myscene->addConductor(datalist);
    ui->graphicsView->setScene(myscene);
    myscene->clearSelection();


//    myObservedLineList = groundingCADWindow->observedLineList;

    iniData();


    connect(ui->buttonBox,SIGNAL(clicked(QAbstractButton*)),this,SLOT(doDataAdded(QAbstractButton*)));
    connect(ui->listWidget,SIGNAL(currentRowChanged(int)),this,SLOT(doClearPreview()));
    connect(ui->tabWidget,SIGNAL(currentChanged(int)),this,SLOT(doClearPreview()));

    connect(ui->pushButton_feature1,SIGNAL(clicked()),this,SLOT(doFeatureDlg()));
    connect(ui->pushButton_feature2,SIGNAL(clicked()),this,SLOT(doFeatureDlg()));
    connect(ui->pushButton_feature3,SIGNAL(clicked()),this,SLOT(doFeatureDlg()));
}

NewGrid::~NewGrid()
{
    delete ui;
}

//初始化信息（只需要初始化观测面和围墙，其他都不需要初始化）
void NewGrid::iniData()
{
    //初始化观测面信息
    if(groundingCADWindow->ob_xmesh>=2 && groundingCADWindow->ob_xmesh<=10000){
        ui->ob5_xmesh->setValue(groundingCADWindow->ob_xmesh);
    }
    if(groundingCADWindow->ob_ymesh>=2 && groundingCADWindow->ob_ymesh<=10000)
        ui->ob5_ymesh->setValue(groundingCADWindow->ob_ymesh);
    if(groundingCADWindow->ob_rect.isValid()){
        ui->ob5_x->setText(tr("%1").arg(groundingCADWindow->ob_rect.x()));
        ui->ob5_y->setText(tr("%1").arg(groundingCADWindow->ob_rect.y()));
        ui->ob5_w->setText(tr("%1").arg(groundingCADWindow->ob_rect.width()));
        ui->ob5_h->setText(tr("%1").arg(groundingCADWindow->ob_rect.height()));
    }
    else{
        QRectF rect = scene->itemsBoundingRect();
        ui->ob5_x->setText(tr("%1").arg(rect.x()));
        ui->ob5_y->setText(tr("%1").arg(rect.y()));
        ui->ob5_w->setText(tr("%1").arg(rect.width()));
        ui->ob5_h->setText(tr("%1").arg(rect.height()));
    }
    ui->ob5_grid_dx->setText(QString("%1").arg(ui->ob5_w->text().toDouble() / ui->ob5_xmesh->text().toDouble()) );
    ui->ob5_grid_dy->setText(QString("%1").arg(ui->ob5_h->text().toDouble() / ui->ob5_ymesh->text().toDouble()) );
    if(groundingCADWindow->ob_z >=0)
        ui->ob5_z->setText(tr("%1").arg(groundingCADWindow->ob_z));

    //初始化围墙
//    qDebug()<<tr("初始化围墙")<<groundingCADWindow->weiqiang();
    if(groundingCADWindow->weiqiang().isEmpty())
        return;
    QPolygonF polygon = groundingCADWindow->weiqiang();
    ui->tableWidget_WeiQiang->setRowCount(polygon.count());
    for(int i=0;i<polygon.count();i++){
        QPointF point = polygon.at(i);
        QTableWidgetItem *itemX = new QTableWidgetItem(QString("%1").arg(point.x()));
        QTableWidgetItem *itemY = new QTableWidgetItem(QString("%1").arg(point.y()));
        ui->tableWidget_WeiQiang->setItem(i,0,itemX);
        ui->tableWidget_WeiQiang->setItem(i,1,itemY);
    }
}

//保存添加的数据（包括导体和观测信息）
void NewGrid::doDataAdded(QAbstractButton *button)
{
    if(ui->buttonBox->buttonRole(button) == (QDialogButtonBox::ApplyRole)
                                            ||(QDialogButtonBox::YesRole))
    {
        readData();   //读取ui相应数据
    }
}

//读取数据
void NewGrid::readData()
{
    switch(ui->listWidget->currentRow())
    {
    case 0: break;
    case 1: readData1();break;
    case 2: readData2();break;
    case 3: readData3();break;
    case 4: break;
    case 5: readData4();break;
    case 6: readData5();break;
    case 7: break;
    case 8: readData6();break;
    default:break;
    }
}


//读取单根导体
void NewGrid::readData1()
{
    uiDatalist.clear();
    Conductor node;
    node.x0=ui->lineEdit_X0->text().toDouble();
    node.y0=ui->lineEdit_Y0->text().toDouble();
    node.z0=ui->lineEdit_Z0->text().toDouble();
    node.x1=ui->lineEdit_X1->text().toDouble();
    node.y1=ui->lineEdit_Y1->text().toDouble();
    node.z1=ui->lineEdit_Z1->text().toDouble();
    node.setR(groundingCADWindow->defultR);
    node.setRou(groundingCADWindow->defultRou);
    node.setU(groundingCADWindow->defultU);
    node.setRemark(groundingCADWindow->defultRemarks);
    uiDatalist.append(node);
    emit dataAdded(uiDatalist);
}

//读取成组矩形导体
void NewGrid::readData2()
{
    uiDatalist.clear();
    if(ui->radio1->isChecked()){        //均匀
        uiDatalist.append(createGrid(1));
    }else if(ui->radio2->isChecked()){  //比例
        uiDatalist.append(createGrid(ui->doubleSpinBox_2_1->text().toDouble()));
    }else if(ui->radio3->isChecked()){  //国标
        uiDatalist.append(createGrid_GB());
    }else if(ui->radio4->isChecked()){
        uiDatalist.append(createGrid_CaoXiaobin());     //曹晓斌
    }
    emit dataAdded(uiDatalist);
}

//读取成组的复杂导体
void NewGrid::readData3()
{
    uiDatalist.clear();
    if(ui->tabWidget->currentIndex()==0){   //多边形
        int N=ui->spinBox3_1_N->value();    //多边形边数
        int M=ui->spinBox3_1_M->value();    //多边形层数
        double D=ui->lineEdit3_1_D->text().toDouble();  //埋深
        double R=ui->lineEdit3_1_R->text().toDouble();  //半径
        double rate=ui->doubleSpinBox3_1_Rate->value(); //压缩比

        Conductor node;

        double l,Rd;

        if(M==1||rate==1){
            l=R/M;
        }else{
            l=R*(1-rate)/(1-pow(rate,M-1));
        }

        for(int i=0;i<M;i++){
            if(M==1){
                Rd=R;
            }else if(rate==1){
                Rd=l*(i+1);
            }else{
                Rd=l*(1-pow(rate,i))/(1-rate);
            }
            for(int j=0;j<N;j++){
                node.dataNodeInit(Rd*cos(2*pi*j/N),Rd*sin(2*pi*j/N),D,Rd*cos(2*pi*(j+1)/N),Rd*sin(2*pi*(j+1)/N),D);
                node.setR(groundingCADWindow->defultR);
                node.setRou(groundingCADWindow->defultRou);
                node.setU(groundingCADWindow->defultU);
                node.setRemark(groundingCADWindow->defultRemarks);
                uiDatalist.append(node);
            }
        }
    }else if(ui->tabWidget->currentIndex()==1){ //放射形
        int N=ui->spinBox3_2_N->value();    //放射线条数
        double D=ui->lineEdit3_2_D->text().toDouble();  //埋深
        double R=ui->lineEdit3_2_R->text().toDouble();  //半径
        double Angel=ui->doubleSpinBox3_2_Angle->text().toDouble()*pi/180; //与水平面夹角
        Conductor node;

        if(ui->radioButton3_2_Earth->isChecked()){
            for(int i=0;i<N;i++){
                node.dataNodeInit(0,0,D,R*cos(2*pi*i/N),R*sin(2*pi*i/N),D);
                node.setR(groundingCADWindow->defultR);
                node.setRou(groundingCADWindow->defultRou);
                node.setU(groundingCADWindow->defultU);
                node.setRemark(groundingCADWindow->defultRemarks);
                uiDatalist.append(node);
            }
        }else if(ui->radioButton3_2_Angle->isChecked()){
            for(int i=0;i<N;i++){
                node.dataNodeInit(0,0,D,R*cos(2*pi*i/N)*cos(Angel),R*sin(2*pi*i/N)*cos(Angel),D+R*sin(Angel));
                node.setR(groundingCADWindow->defultR);
                node.setRou(groundingCADWindow->defultRou);
                node.setU(groundingCADWindow->defultU);
                node.setRemark(groundingCADWindow->defultRemarks);
                uiDatalist.append(node);
            }
        }

    }else if(ui->tabWidget->currentIndex()==2){ //螺旋形
        double Z0=ui->lineEdit3_3_Z0->text().toDouble();
        double Z1=ui->lineEdit3_3_Z1->text().toDouble();
        double R0=ui->lineEdit3_3_R0->text().toDouble();
        double R1=ui->lineEdit3_3_R1->text().toDouble();

        int M=ui->spinBox3_3_M->value();    //圈数
        int N;  //段的总数
        if(ui->radioButton3_3_NM->isChecked())
            N=M*ui->spinBox3_3_NM->value();
        else
            N=ui->spinBox3_3_N->value();    //段的总数

        Conductor node;
        double dR=(R1-R0)/(N+1);
        double dZ=(Z1-Z0)/(N+1);
        double dAngle=M*2*pi/N;
        for(int i=0;i<N;i++){
            node.dataNodeInit((R0+dR*i)*cos(dAngle*i),(R0+dR*i)*sin(dAngle*i),Z0+dZ*i,
                              (R0+dR*(i+1))*cos(dAngle*(i+1)),(R0+dR*(i+1))*sin(dAngle*(i+1)),Z0+dZ*(i+1));
            node.setR(groundingCADWindow->defultR);
            node.setRou(groundingCADWindow->defultRou);
            node.setU(groundingCADWindow->defultU);
            node.setRemark(groundingCADWindow->defultRemarks);
            uiDatalist.append(node);
        }
    }
    emit dataAdded(uiDatalist);
}

//读取观测线（待添加）
void NewGrid::readData4()
{
    ObservedLine line;
    line.setPoints(Point(ui->ObLine_X1->text().toDouble(),ui->ObLine_Y1->text().toDouble(),ui->ObLine_Z1->text().toDouble()),
                   Point(ui->ObLine_X2->text().toDouble(),ui->ObLine_Y2->text().toDouble(),ui->ObLine_Z2->text().toDouble()));
    line.setName(ui->ObLine_Name->text());

    if(ui->rbn_ObLine_gap->isChecked()){
        line.setGap(ui->ObLine_Gap->text().toDouble());
    }else if(ui->rbn_ObLine_subsection->isChecked()){
        line.setSubsection(ui->ObLine_Subsection->value());
    }
    if(line.isValid()){
        emit observedLineAdded(line);
    }
    else{
        qDebug()<<tr("观测线添加失败");
    }

}

//读取观测面
void NewGrid::readData5()
{
    QRectF r;
    double z;
    int ob_xmesh,ob_ymesh;
    if(ui->ob5_rbtn1->isChecked()){ //自动生成观测面
        r=this->scene->itemsBoundingRect();
        double k=ui->ob5_spinBox->value()/100.0;    //自动生成的比率
        r.adjust(-r.width()*k,-r.height()*k,r.width()*k,r.height()*k);
        z = 0; //默认位于地表
    }else{                          //手动生成观测面
        r.setLeft(ui->ob5_x->text().toDouble());
        r.setTop(ui->ob5_y->text().toDouble());
        r.setWidth(ui->ob5_w->text().toDouble());
        r.setHeight(ui->ob5_h->text().toDouble());
        z=ui->ob5_z->text().toDouble();
    }
    if(ui->ob5_rbtn3->isChecked()){ //按网格数目决定密度
        ob_xmesh = ui->ob5_xmesh->value();
        ob_ymesh = ui->ob5_ymesh->value();
    }else{                          //按网格大小决定密度
        ob_xmesh = r.width() / ui->ob5_grid_dx->text().toDouble();
        ob_ymesh = r.height() / ui->ob5_grid_dy->text().toDouble();
    }
    emit obAdded(r,ob_xmesh,ob_ymesh,z);      //发出观测面改变的信号
}

//读取围墙信息
void NewGrid::readData6()
{
    QPolygonF polygon;

    if(ui->radio_8_1->isChecked()){     //自动生成
        QRectF r = scene->itemsBoundingRect();
        polygon << r.topLeft() << r.topRight() << r.bottomRight() << r.bottomLeft();
    }
    else if(ui->radio_8_2->isChecked()){  //手动输入生成
        for(int row=0;row<ui->tableWidget_WeiQiang->rowCount();row++){
            QPointF point;
            point.setX(ui->tableWidget_WeiQiang->item(row,0)->text().toDouble());
            point.setY(ui->tableWidget_WeiQiang->item(row,1)->text().toDouble());
            polygon << point;
        }
    }
    groundingCADWindow->setWeiqiang(polygon); //这里没有用信号的方式传递变量，有些不一致，以后视情况修改

//    qDebug()<<groundingCADWindow->weiqiang();
}

//根据输入生成网络
QList<Conductor > NewGrid::createGrid(const double k)
{
    QList<Conductor > datalist;
    Conductor node;
    double w=ui->lineEdit_W_2_1->text().toDouble();         //宽度
    double h=ui->lineEdit_H_2_1->text().toDouble();         //高度
    double d=ui->lineEdit_D_2_1->text().toDouble();         //深度
    int hor=ui->spinBox_Hor_2_1->text().toDouble();         //水平导体数目
    int ver=ui->spinBox_Ver_2_1->text().toDouble();         //垂直导体数目

    node.dataNodeInit(-w/2,h/2,d,w/2,h/2,d);
    node.setR(groundingCADWindow->defultR);
    node.setRou(groundingCADWindow->defultRou);
    node.setU(groundingCADWindow->defultU);
    node.setRemark(groundingCADWindow->defultRemarks);
    datalist.append(node);
    double x;   //x存储中心导体段的长度
    int i;
    if(hor%2==0)    //hor是偶数
    {
        if((k-1.0)<0.00001&&(k-1.0)>-0.00001)
            x=h/(hor-1);
        else
            x=h*(1-k)/(1+k-2*pow(k,hor/2));
        for(i=hor/2-1;i>=0;i--)
        {
            node.y0=node.y0-x*pow(k,i);
            node.y1=node.y0;
            datalist.append(node);
        }
        node=datalist.last();
        for(i=1;i<hor/2;i++)
        {
            node.y0=node.y0-x*pow(k,i);
            node.y1=node.y0;
            datalist.append(node);
        }
    }
    else            //hor是奇数
    {
        if((k-1.0)<0.00001&&(k-1.0)>-0.00001)
            x=h/(hor-1);
        else
            x=h*(1-k)/2/(1-pow(k,(hor-1)/2));
        for(i=0;i<(hor-1)/2;i++)
        {
            node.y0=node.y0-x*pow(k,(hor-3)/2-i);
            node.y1=node.y0;
            datalist.append(node);
        }
        node=datalist.last();
        for(i=0;i<(hor-1)/2;i++)
        {
            node.y0=node.y0-x*pow(k,i);
            node.y1=node.y0;
            datalist.append(node);
        }
    }

    node.dataNodeInit(-w/2,h/2,d,-w/2,-h/2,d);
    node.setR(groundingCADWindow->defultR);
    node.setRou(groundingCADWindow->defultRou);
    node.setU(groundingCADWindow->defultU);
    node.setRemark(groundingCADWindow->defultRemarks);
    datalist.append(node);
    if(ver%2==0)    //ver是偶数
    {
        if((k-1.0)<0.00001&&(k-1.0)>-0.00001)
            x=w/(ver-1);
        else
            x=w*(1-k)/(1+k-2*pow(k,ver/2));
        for(i=ver/2-1;i>=0;i--)
        {
            node.x0=node.x0+x*pow(k,i);
            node.x1=node.x0;
            datalist.append(node);
        }
        node=datalist.last();
        for(i=1;i<ver/2;i++)
        {
            node.x0=node.x0+x*pow(k,i);
            node.x1=node.x0;
            datalist.append(node);
        }
    }
    else            //ver是奇数
    {
        if((k-1.0)<0.00001&&(k-1.0)>-0.00001)
            x=w/(ver-1);
        else
            x=w*(1-k)/2/(1-pow(k,(ver-1)/2));
        for(i=0;i<(ver-1)/2;i++)
        {
            node.x0=node.x0+x*pow(k,(ver-3)/2-i);
            node.x1=node.x0;
            datalist.append(node);
        }
        node=datalist.last();
        for(i=0;i<(ver-1)/2;i++)
        {
            node.x0=node.x0+x*pow(k,i);
            node.x1=node.x0;
            datalist.append(node);
        }
    }
    return datalist;
}

//曹晓斌公式
QList<Conductor> NewGrid::createGrid_CaoXiaobin()
{
    QList<Conductor > datalist;
    Conductor node;
    double w=ui->lineEdit_W_2_1->text().toDouble();         //宽度
    double h=ui->lineEdit_H_2_1->text().toDouble();         //高度
    double d=ui->lineEdit_D_2_1->text().toDouble();         //深度
    int hor=ui->spinBox_Hor_2_1->text().toDouble();         //水平导体数目
    int ver=ui->spinBox_Ver_2_1->text().toDouble();         //垂直导体数目
    node.setR(groundingCADWindow->defultR);                 //半径
    node.setRou(groundingCADWindow->defultRou);
    node.setU(groundingCADWindow->defultU);
    node.setRemark(groundingCADWindow->defultRemarks);

    hor--;
    ver--;


    //竖排导体
    for(int i=0;i<=ver;i++){
        double an = w * (0.5 + sin(2*pi*i/ver)/100 - cos(pi*i/ver)/2);
        node.dataNodeInit(-w/2 + an,h/2,d,-w/2 + an,-h/2,d);
        datalist.append(node);
    }
    //横排导体
    for(int i=0;i<=hor;i++){
        double an = h * (0.5 + sin(2*pi*i/hor)/100 - cos(pi*i/hor)/2);
        node.dataNodeInit(-w/2,-h/2 + an,d,w/2,-h/2 + an,d);
        datalist.append(node);
    }
    return datalist;
}

//国标（陈先璐）
QList<Conductor> NewGrid::createGrid_GB()
{
    QList<Conductor > datalist;
    Conductor node;
    double w=ui->lineEdit_W_2_1->text().toDouble();         //宽度
    double h=ui->lineEdit_H_2_1->text().toDouble();         //高度
    double d=ui->lineEdit_D_2_1->text().toDouble();         //深度
    int hor=ui->spinBox_Hor_2_1->text().toDouble();         //水平导体数目
    int ver=ui->spinBox_Ver_2_1->text().toDouble();         //垂直导体数目
    node.setR(groundingCADWindow->defultR);
    node.setRou(groundingCADWindow->defultRou);
    node.setU(groundingCADWindow->defultU);
    node.setRemark(groundingCADWindow->defultRemarks);

    if(hor>21)      //国标只到20
        hor=21;
    if(ver>21)
        ver=21;

    hor--;
    ver--;

    //竖排导体
    for(int i=0;i<=ver;i++){
        double an = w*GB_dingzhi(i,ver)/100;
        node.dataNodeInit(-w/2 + an,h/2,d,-w/2 + an,-h/2,d);
        datalist.append(node);
    }
    //横排导体
    for(int i=0;i<=hor;i++){
        double an = h*GB_dingzhi(i,hor)/100;
        node.dataNodeInit(-w/2,-h/2 + an,d,w/2,-h/2 + an,d);
        datalist.append(node);
    }
    return datalist;
}

//国标规定的边距(k为当前编号，n为总分段数)
//k=0,1,2...n
//n=1,2,3...20
//为了使得总和为100%，有部分数字进行了修改
double NewGrid::GB_dingzhi(int k,int n)
{
    //定义二维数组存储定值
    static double A[20][20] =
    {
        {100},
        {50},
        {27.5,45.0},
        {17.5,32.5},
        {12.5,23.33,28.33},         //最后一个数字从28.34改成28.34
        {8.75,17.5,23.75},
        {7.14,13.57,18.57,21.44},   //最后一个数字从21.43改成21.44
        {5.50,10.83,15.67,18.00},
        {4.50,8.94,12.83,15.33,16.8},   //最后一个数字从16.78改成16.8
        {3.75,7.50,11.08,13.08,14.59},  //最后一个数字从14.58改成14.59
        {3.18,6.36,9.54,11.36,12.73,13.66},     //最后一个数字从13.64改成13.66
        {2.75,5.42,8.17,10.00,11.33,12.33},
        {2.38,4.69,6.77,8.92,10.23,11.15,11.72},    //最后一个数字从11.69改成11.72
        {2.00,3.86,6.00,7.86,9.28,10.24,10.76},
        {1.56,3.62,5.35,6.82,8.07,9.12,10.01,10.9},
        {1.46,3.27,4.82,6.14,7.28,8.24,9.07,9.72},
        {1.38,2.97,4.35,5.54,6.57,7.47,8.24,8.90,9.16},
        {1.14,2.58,3.86,4.95,5.91,6.76,7.50,8.15,9.15},     //最后一个数字从8.71改成9.15，改动很大，原数据有错误
        {1.05,2.32,3.47,4.53,5.47,6.26,6.95,7.53,8.11,8.62},
        {0.95,2.15,3.20,4.15,5.00,5.75,6.40,7.00,7.50,7.90}
    };

    //将半接地网数据，扩大到全接地网
    for(int i=0;i<20;i++){
        for(int j=(i+1)/2;j<=i;j++){
            A[i][j] = A[i][i-j];
        }
    }

    double sum=0;
    for(int i=0;i<k;i++){
        sum += A[n-1][i];
    }

    return sum;
}

//void NewGrid::on_lineEdit_X0_textEdited(const QString &arg1)
//{
//    inputCheck(arg1);
//}

//void NewGrid::inputCheck(const QString &arg1)
//{
//    bool ok=true;
//    arg1.toDouble(&ok);
//    if(ok==false)
//    {
//        QMessageBox msgBox;
//        msgBox.setText(tr("请输入一个实数！"));
//        msgBox.exec();
//        qDebug()<<arg1;
//    }
//}



//更新预览视图
void NewGrid::doClearPreview()
{
    myscene->selectAll();
    myscene->deleteSelected();
    myscene->addConductor(datalist);
    myscene->clearSelection();
    QRectF rect=myscene->itemsBoundingRect();
    ui->graphicsView->fitInView(rect.adjusted(rect.width()*(-0.1),rect.height()*(-0.1),rect.width()*0.1,rect.height()*0.1),
                                Qt::KeepAspectRatio);

}

//【更新预览】按钮的响应函数
void NewGrid::on_pushButton_update_clicked()
{
    doClearPreview();
    readData();         //读取数据（目的是更新uiDatalist）
    myscene->addConductor(uiDatalist);
    QRectF rect=myscene->itemsBoundingRect();
    ui->graphicsView->fitInView(rect.adjusted(rect.width()*(-0.1),rect.height()*(-0.1),rect.width()*0.1,rect.height()*0.1),
                                Qt::KeepAspectRatio);
}

//void NewGrid::on_ob_generate_clicked()
//{
//    QRectF rect=myscene->itemsBoundingRect();
//    //ob_Rect = new QRect;
//    ob_Rect = &rect.adjusted(-rect.width()*0.25,-rect.height()*0.25,
//                             rect.width()*0.25,rect.height()*0.25);

//}

//围墙多边形顶点增加
void NewGrid::on_tBn_Add_clicked()
{
    int row = ui->tableWidget_WeiQiang->currentRow();
    ui->tableWidget_WeiQiang->insertRow(row+1);
    for(int i=0;i<2;i++){
        QTableWidgetItem *item = new QTableWidgetItem("0");
        ui->tableWidget_WeiQiang->setItem(row+1,i,item);
    }
}

//围墙多边形顶点删除
void NewGrid::on_tBn_Del_clicked()
{
    int row = ui->tableWidget_WeiQiang->currentRow();
    if(row >=0){
        ui->tableWidget_WeiQiang->removeRow(row);
    }
}

//围墙多边形顶点上移
void NewGrid::on_tBn_Up_clicked()
{
    int row = ui->tableWidget_WeiQiang->currentRow();
    if(row >=1){
        //两行内容做一个交换
        for(int i=0;i<2;i++){
            QTableWidgetItem *item1 = ui->tableWidget_WeiQiang->takeItem(row-1,i);
            QTableWidgetItem *item2 = ui->tableWidget_WeiQiang->takeItem(row,i);
            ui->tableWidget_WeiQiang->setItem(row-1,i,item2);
            ui->tableWidget_WeiQiang->setItem(row,i,item1);
        }
        ui->tableWidget_WeiQiang->selectRow(row-1);
    }
}

//围墙多边形顶点下移
void NewGrid::on_tBn_Down_clicked()
{
    int row = ui->tableWidget_WeiQiang->currentRow();
    if(row >=0 && row<ui->tableWidget_WeiQiang->rowCount()-1){
        //两行内容做一个交换
        for(int i=0;i<2;i++){
            QTableWidgetItem *item1 = ui->tableWidget_WeiQiang->takeItem(row,i);
            QTableWidgetItem *item2 = ui->tableWidget_WeiQiang->takeItem(row+1,i);
            ui->tableWidget_WeiQiang->setItem(row,i,item2);
            ui->tableWidget_WeiQiang->setItem(row+1,i,item1);
        }
        ui->tableWidget_WeiQiang->selectRow(row+1);
    }
}

//设置默认导体半径
void NewGrid::doFeatureDlg()
{
    FeaturesDialog dlg(groundingCADWindow);
    if(dlg.exec() == 1){
        dlg.getDefult();     //更新默认的半径
    }
}
