#include "oblinemainwindow.h"
#include "ui_oblinemainwindow.h"
#include "oblineplot.h"
#include "windwalkerearthingmaindlg.h"
#include "Base/observedline.h"

#include <QComboBox>
#include <QLabel>
#include <QtDebug>
#include <QDir>

ObLineMainWindow::ObLineMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ObLineMainWindow)
{
    ui->setupUi(this);

    mainDlg = static_cast<WindWalkerEarthingMainDlg *>(parent);

//    ObLinePlot *plot1 = new ObLinePlot(this);
//    ui->tabWidget->addTab(plot1,tr("观测线1"));

    uiInit();
    loadFiles();
    readObLineX();
}

ObLineMainWindow::~ObLineMainWindow()
{
    delete ui;
}

//ui的一些初始化
void ObLineMainWindow::uiInit()
{
    ui->menubar->setHidden(true);   //隐藏菜单栏

    QLabel *label = new QLabel(tr("选择显示的观测线 "));
    comboBox = new QComboBox;
    ui->toolBar_Tool->addWidget(label);
    ui->toolBar_Tool->addWidget(comboBox);

    connect(comboBox,SIGNAL(activated(QString)),this,SLOT(creatNewPlot(QString)));
}

//读取文件
void ObLineMainWindow::loadFiles()
{
    qDebug()<<mainDlg->currentProjectDir;
    QString path = mainDlg->currentProjectDir + "/" + tr("计算文件") + "/" + tr("观测线");
    QDir temp(path);
    if(!temp.exists(path))
        return;
    QDir::setCurrent(path);

    QFileInfoList infoList = temp.entryInfoList(QDir::Files|QDir::NoDotAndDotDot);
    foreach(QFileInfo info,infoList){
        qDebug()<<info.fileName();
        comboBox->addItem(info.baseName());
    }
}

//保存到文件
void ObLineMainWindow::on_atn_Save_triggered()
{

}

//打印
void ObLineMainWindow::on_atn_Print_triggered()
{

}

//comboBox的响应函数
//先执行检查，如果选择项目已生成图形，那么切换到相应图形，如果没有，则新生成这个图形
void ObLineMainWindow::creatNewPlot(QString name)
{
    for(int i=0;i<ui->tabWidget->count();i++){
        if(ui->tabWidget->tabText(i) == name){
            ui->tabWidget->setCurrentIndex(i);
            return;
        }
    }
    foreach(ObservedLine line, observedLineList){
        if(line.name() == name){
            ObLinePlot *plot1 = new ObLinePlot(this,line,name);
//            connect(ui->atn_Zoom,SIGNAL(toggled(bool)),
//                    plot1,SLOT(enableZoomMode(bool)));
            ui->tabWidget->addTab(plot1,name);
            ui->tabWidget->setCurrentWidget(plot1);
            return;
        }
    }
}

//读取观测线(x轴)数据
void ObLineMainWindow::readObLineX()
{
    QString fileName = this->mainDlg->currentProjectDir + "/" + tr("输入数据.txt");

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;
    QTextStream in(&file);    // read the data serialized from the file

    QString string;

    while(!in.atEnd())
    {
        string=in.readLine();

        //观测线数据
        if(string.contains(tr("观测线"))){
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

    };
}

