#include "computdialog.h"
#include "ui_computdialog.h"

#include "cutthread.h"
#include "matrixthread.h"
#include "DianzuCompute/dianzucompute.h"
#include "DianzuCompute/secoundsubdivision.h"
#include "DianzuCompute/kouteynikoff.h"
#include "DianzuCompute/heppe.h"
#include "OptimizedDesign/optimizeddesigndlg.h"
#include "Base/currentsource.h"

#include <QMessageBox>
#include <QTimer>
#include <QTime>
#include <QtDebug>
#include <qmath.h>

#include "DianzuCompute/jifenfa.h"

ComputDialog::ComputDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ComputDialog)
{
    ui->setupUi(this);

    WindWalkerEarthingMainDlg *mainDlg = qobject_cast<WindWalkerEarthingMainDlg *>(parent);

    currentProjectDir = mainDlg->currentProjectDir;
    //建立工作文件夹
    QDir *temp = new QDir;
    temp->mkdir(currentProjectDir + "/" + tr("计算文件"));
    if(temp->exists(currentProjectDir + "/" + tr("计算文件")))
        this->currentDir = currentProjectDir + "/" + tr("计算文件");

    connect(ui->progressBar,SIGNAL(valueChanged(int)),this,SLOT(totleProgress(int)));

    //读取输入文件数据
    readDataList();
    readEarth();

    ui->textEdit->setVisible(false);

    //计算线程初始化
    cutThread = new CutThread(this->dataList,this);

    connect(cutThread,SIGNAL(currentName(QString,QString)),this,SLOT(setTitle(QString,QString)));
    connect(cutThread,SIGNAL(computeCompleted()),this,SLOT(computFinished()));

    //终止计算的信号槽连接
    connect(this,SIGNAL(breakCompute()),cutThread,SLOT(terminate()));
}

ComputDialog::~ComputDialog()
{
    delete ui;
}

//开始计算
void ComputDialog::on_btn_OK_clicked()
{
    //qt的时间类
    startTime = new QTime;
    startTime->start();

    //定时器，每隔一秒种，发出一次信号
    timer = new QTimer;
    timer->start(10);
    connect(timer,SIGNAL(timeout()),this,SLOT(timeUpdate()));



    ui->btn_Terminate->setEnabled(true);
    ui->btn_OK->setEnabled(false);

    //开始计算线程
    computBegin();
    cutThread->start();
}


//每隔一秒种，更新一次时间图标
void ComputDialog::timeUpdate()
{
    timer->start(1000);
    ui->label_time->setText(QTime(0,0,0).addMSecs(startTime->elapsed()).toString("hh:mm:ss"));
}

//设置总进度条
void ComputDialog::totleProgress(int n)
{
    if(ui->titleLabel->text().contains(tr("交点切割"))){        //占总进度10%
        ui->progressBar_Total->setValue(n/10);
    }
    else if(ui->titleLabel->text().contains(tr("系数矩阵"))){   //占总进度50%
        ui->progressBar_Total->setValue(10+n/2);
    }
    else if(ui->titleLabel->text().contains(tr("地表电位"))){   //占总进度30%
        ui->progressBar_Total->setValue(60+n*3/10);
    }
    else if(ui->titleLabel->text().contains(tr("接触电压"))){   //占总进度10%
        ui->progressBar_Total->setValue(90+n/10);
    }
}

void ComputDialog::computBegin()
{
    outputText.clear();
    outputText += tr("接地计算计算结果清单：\n\n");
    QStringList list = currentDir.split("/");
    list.removeLast();
    outputText += tr("工程名：%1 \n").arg(list.last());

    outputText += tr("接地导体总长度：%1米 \n").arg(Compute::totalLength(this->dataList));
}

void ComputDialog::computFinished()
{
    disconnect(timer,SIGNAL(timeout()),this,SLOT(timeUpdate()));    //停止计时

    ui->btn_Terminate->setEnabled(false);       //终止按钮不可用
    ui->btn_OK->setEnabled(true);

    //设置输出文件位置
    QStringList list = currentDir.split("/");
    list.removeLast();
    list.append(tr("输出数据.txt"));
    QFile file(list.join("/"));
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return;
    QTextStream out(&file);
    out << outputText;
}

int ComputDialog::thirdCut()
{
    return ui->spinBox->value();
}

void ComputDialog::doOutputTextAdd(QString str)
{
    outputText += str;
}

void ComputDialog::doDebugTextAdd(QString str)
{
    debugText += str;
    ui->textEdit->appendPlainText(str);
}


void ComputDialog::closeEvent(QCloseEvent *event)
{
    cutThread->terminate();
    event->accept();
}

void ComputDialog::setTitle(QString title, QString buzhou)
{
    ui->titleLabel->setText(title);
    ui->label_buzhou->setText(tr("步骤") + buzhou);

//    QTextDocument *doc = ui->textEdit->document();
//    doc->setPlainText(doc->toPlainText().append(tr("正在计算步骤") + buzhou + "\n"));
//    ui->textEdit->setDocument(doc);

}

//设置进度条的进度
void ComputDialog::setValue(int i, int n)
{
    ui->progressBar->setValue(i*100/n);
}


//终止计算进程
void ComputDialog::on_btn_Terminate_clicked()
{
    cutThread->terminate();
    ui->btn_OK->setEnabled(true);
    ui->btn_Terminate->setEnabled(false);
    disconnect(timer,SIGNAL(timeout()),this,SLOT(timeUpdate()));    //停止计时
}


//切换精简/复杂模式
void ComputDialog::on_pushBtn_More_toggled(bool checked)
{
    ui->textEdit->setVisible(checked);
    if(!checked){
        ui->pushBtn_More->setText(tr("详细 <<"));
    }
    else{
        ui->pushBtn_More->setText(tr("简略 >>"));
    }
}


//////////////////////////////////////////////////////////////////////
///读取文件数据
//////////////////////////////////////////////////////////////////////

//在标准位置读取导体数据
void ComputDialog::readDataList()
{
    QString fileName = this->currentProjectDir + "/" + tr("输入数据.txt");

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;
    QTextStream in(&file);    // read the data serialized from the file

    QString string;
    int ret=QMessageBox::No ;

    while(!in.atEnd())
    {
        string=in.readLine();
        //导体
        if(string.contains(tr("导体数据："))){
            string = string.remove(tr("导体数据：（"));
            string = string.remove(tr("组）"));
            int n = string.toInt(); //得到导体组数

            for(int i=0;i<n;i++){   //
                string=in.readLine();
                Conductor c;
                c.input(string);
                this->dataList.append(c);
            }
        }
        //观测面数据
        else if(string.contains(tr("观测面"))){
            string=in.readLine();
            QStringList list = string.split(",");
            if(list.count()>=4){
                ob_rect.setRect(list.at(0).toDouble(),list.at(1).toDouble(),list.at(2).toDouble(),list.at(3).toDouble());
            }
//            string=in.readLine();   //第二行是导体外接矩形
//            if(list.count()>=4){
//                conductor_rect.setRect(list.at(0).toDouble(),list.at(1).toDouble(),list.at(2).toDouble(),list.at(3).toDouble());
//            }
            string=in.readLine();   //第二行是ob_z,ob_xmesh,ob_ymesh
            list = string.split(",");
            if(list.count()>=3){
                ob_z = list.at(0).toDouble();
                ob_xmesh = list.at(1).toDouble();
                ob_ymesh = list.at(2).toDouble();

//                qDebug()<<"ob_z="<<ob_z;
//                qDebug()<<"ob_xmesh="<<ob_xmesh;
//                qDebug()<<"ob_ymesh="<<ob_ymesh;

            }
        }
        //观测线数据
        else if(string.contains(tr("观测线"))){
            this->observedLineList.clear();
            string = string.remove(tr("观测线数据：（"));
            string = string.remove(tr("组）"));
            int n = string.toInt(); //得到导体组数

            for(int i=0;i<n;i++){   //
                string=in.readLine();
                QStringList list = string.split(",");
                if(list.count()<8){             //一行不够8个信息就补零
                    int fill=8-list.count();
                    for(int i=0;i<fill;i++){
                        list.append("0");
                    }
                }
                ObservedLine line;
                line.setPoints(Point(list.at(0).toDouble(),list.at(1).toDouble(),list.at(2).toDouble()),
                               Point(list.at(3).toDouble(),list.at(4).toDouble(),list.at(5).toDouble()));
                line.setName(list.at(6));
                line.setSubsection(list.at(7).toInt());
                this->observedLineList.append(line);
            }
        }
        //围墙数据
        else if(string.contains(tr("围墙"))){
            QPolygonF polygon;
            string=in.readLine();
            QStringList list = string.split(",");
            for(int i=0;i<list.count();i++){
                double x = list.at(i).split(" ").at(0).toDouble();
                double y = list.at(i).split(" ").at(1).toDouble();
                QPointF point(x,y);
                polygon << point;
            }
//            qDebug()<<polygon;
            weiqiang = polygon;
        }
        //电源数据
        else if(string.contains(tr("电源"))){
            this->currentSourceList.clear();    //清空
            string = string.remove(tr("电源数据：（"));
            string = string.remove(tr("组）"));
            int n = string.toInt(); //得到电流源组数

            string=in.readLine();
            this->I_in = string.toDouble();  //得到等电位电流
            if(this->I_in == 0)
                this->I_in = 1000;

            for(int i=0;i<n;i++){   //得到不等电位电流源信息
                string=in.readLine();
                CurrentSource cs;
                cs.input(string);
                this->currentSourceList.append(cs);
            }
        }

    };
}

//在标准位置读取土壤数据
void ComputDialog::readEarth()
{
    earth_resistivity.clear();
    earth_thickness.clear();

    QString fileName = this->currentProjectDir + "/" + tr("土壤数据.txt");
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;
    QTextStream in(&file);    // read the data serialized from the file
    QString string;
    while(!in.atEnd())
    {
        string=in.readLine();
        //导体
        if(string.contains(tr("土壤数据"))){
            string = string.remove(tr("土壤数据：（"));
            string = string.remove(tr("组）"));
            int n = string.toInt(); //得到土壤数据组数
            string=in.readLine();

            for(int i=0;i<n;i++){   //
                string=in.readLine();
                QStringList list = string.split(",");
                if(list.count()!=2)
                    break;
                earth_resistivity << list.at(0).toDouble();
                earth_thickness << list.at(1).toDouble();
            }
        }
    }
}

//优化设计
void ComputDialog::on_pbn_OptimizeDesign_clicked()
{
    OptimizedDesignDlg dlg(this);
    dlg.exec();
}




//测试函数
void ComputDialog::on_pushButton_clicked()
{
    Conductor c1,c2,c3;

    double rou = 100;

//    c1.dataNodeInit(0,0,0.5,1,0,0.5,0.01,1.7e-7,636,"");
    c1.dataNodeInit(0,0,0.5,2,0,0.5,0.002,1.7e-7,1,"");
    qDebug()<<c1.dianzu()<<c1.neizigan();
    qDebug()<<c1.dianzu(50)<<c1.neizigan(50)/(2*50*3.1416);

    Heppe heppe(c1);
    qDebug()<<heppe.waizigan();

/*    c1.dataNodeInit(0,0,0.5,10,0,0.5);
    c1.setR(0.01);
    c2.dataNodeInit(0,10,0.5,0,20,0.5);
    c2.setR(0.01);
    c3.dataNodeInit(50,50,0.5,50,50,30.5);
    c3.setR(0.01);

    qDebug()<<"Dawalibi \t"<<DianZuCompute::HuZu_Dawalibi_Half(c1,c3,rou,10)
              <<"\t"<<DianZuCompute::HuZu_Dawalibi_Half(c2,c3,rou,10);
    qDebug()<<"Jifen \t"<<DianZuCompute::HuZu_JiFen_Half(c1,c3,rou,10)
              <<"\t"<<DianZuCompute::HuZu_JiFen_Half(c2,c3,rou,10);
    qDebug()<<"JiHe \t"<<DianZuCompute::HuZu_JiHe_Half(c1,c3,rou)
              <<"\t"<<DianZuCompute::HuZu_JiHe_Half(c2,c3,rou);*/

    /*
    qDebug()<<tr("积分法计算互阻");
    for(int i=0;i<4;i++){
        for(int j=1;j<6;j++){
            int n = j*round(qPow(10.0,i));
            qDebug()<<"n="<<n<<"R="<<DianZuCompute::HuZu_JiFen_Half(c1,c2,rou,n);
        }
    }

    qDebug()<<tr("dawalibi法计算互阻和自阻");
    for(int i=0;i<4;i++){
        for(int j=1;j<6;j++){
            int n = j*round(qPow(10.0,i));
            qDebug()<<"n="<<n<<"\tRh="<<DianZuCompute::HuZu_Dawalibi_Totle(c1,c2,rou,n)
                      <<"\tRz="<<DianZuCompute::ZiZu_Dawalibi_Totle(c1,rou,n);
        }
    }

    qDebug()<<tr("几何法计算互阻和自阻")<<"Rh="<<DianZuCompute::HuZu_JiHe_Totle(c1,c2,rou)
           <<"Rz="<<DianZuCompute::ZiZu_JiHe_Totle(c1,rou);

    qDebug()<<tr("Kouteyniloff法计算互阻和自阻");
    for(int i=0;i<4;i++){
        for(int j=1;j<6;j++){
            int n = j*round(qPow(10.0,i));
            qDebug()<<"n="<<n<<"\tRh="<<DianZuCompute::HuZu_Kouteynikoff_Totle(c1,c2,rou,n)
                      <<"\tRz="<<DianZuCompute::ZiZu_Kouteynikoff_Totle(c1,rou,n);
        }
    }
//    int n=3;
//    qDebug()<<"n="<<n<<"R="<<DianZuCompute::HuZu_Kouteynikoff_Half(c1,c2,rou,n);
    Kouteynikoff kk(c1);
    kk.test();

*/

//    Conductor c1;
//    c1.dataNodeInit(0,0,0.5,1,0,0.5);
//    c1.setR(0.01);

//    JifenFa a(c1);
//    qDebug()<<tr("计算出自阻为：")<<a.Vp(Point(0,0,2),100);

//    qDebug()<<tr("计算出自阻为：")<<DianZuCompute::Segment_Point_Potential(c1,Point(0,0,2),100);
//    SecoundSubdivision s(c1,4);
//    qDebug()<<tr("计算出自阻为：")<<s.zizu();

//    qDebug()<<tr("计算出自阻为：")<<s.value()*rou;
//    qDebug()<<tr("几何法计算出自阻为：")<<DianZuCompute::ZiZu_JiHe_Half(c1,rou);
//    qDebug()<<tr("二次分域法计算出自阻为：")<<DianZuCompute::ZiZu_SecSubdivision_Half(c1,rou,200);

}
