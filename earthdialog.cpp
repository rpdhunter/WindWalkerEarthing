#include "earthdialog.h"
#include "ui_earthdialog.h"
#include <QtDebug>
#include <QMessageBox>

EarthDialog::EarthDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EarthDialog)
{
    ui->setupUi(this);

    //父窗口指针，用于参数传递
    mainDlg = qobject_cast<WindWalkerEarthingMainDlg *>(parent);

    //设置标题
    QStringList list = mainDlg->currentProjectDir.split("/");
    this->setWindowTitle(list.last() + tr(" - 设置土壤参数"));

    ui->tableWidget->horizontalHeader()->setResizeMode(QHeaderView::Stretch);

    this->tableInit();
}

EarthDialog::~EarthDialog()
{
    delete ui;
}

//读取数据
void EarthDialog::tableInit()
{
    this->readFile();
    QList<double> thickness = mainDlg->earth_thickness;
    QList<double> resistivity = mainDlg->earth_resistivity;
    if(thickness.count()>0){
        int n = thickness.count();
        ui->tableWidget->setRowCount(n+1);
        ui->spinBox->setValue(n);

        for(int i=1;i<n+1;i++){
            QTableWidgetItem *item1 = new QTableWidgetItem(tr("%1").arg(resistivity[i-1]));
            ui->tableWidget->setItem(i,0,item1);
            QTableWidgetItem *item2 = new QTableWidgetItem(tr("%1").arg(thickness[i-1]));
            ui->tableWidget->setItem(i,1,item2);
        }
    }
}



//修改层数的同时，改变表格的列数
void EarthDialog::on_spinBox_valueChanged(int arg1)
{
    ui->tableWidget->setRowCount(arg1+1);
    QStringList list;
    list<<tr("空气层")<<tr("土壤第1层");
    for(int i=0;i<arg1;i++){
        list<<tr("土壤第%1层").arg(i+2);
    }
    ui->tableWidget->setVerticalHeaderLabels(list);
}

void EarthDialog::on_buttonBox_accepted()
{
    mainDlg->earth_thickness.clear();
    mainDlg->earth_resistivity.clear();
    for(int i=1;i<ui->tableWidget->rowCount();i++){
        if(ui->tableWidget->item(i,1) != 0){
            mainDlg->earth_thickness<<ui->tableWidget->item(i,1)->text().toDouble();
        }else{
             mainDlg->earth_thickness<<0;
        }
        if(ui->tableWidget->item(i,0) != 0){
            mainDlg->earth_resistivity<<ui->tableWidget->item(i,0)->text().toDouble();
        }else{
            mainDlg->earth_resistivity<<0;
        }
    }
    saveFile();
}

//从标准位置读取文件
void EarthDialog::readFile()
{
    mainDlg->earth_resistivity.clear();
    mainDlg->earth_thickness.clear();

    QString fileName = this->mainDlg->currentProjectDir + "/" + tr("土壤数据.txt");
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
                mainDlg->earth_resistivity << list.at(0).toDouble();
                mainDlg->earth_thickness << list.at(1).toDouble();
            }
        }
    }
}

//保存文件至标准位置
void EarthDialog::saveFile()
{
    QString fileName = this->mainDlg->currentProjectDir + "/" + tr("土壤数据.txt");
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return;
    QTextStream out(&file);
    out << tr("土壤数据：（%1组）").arg(mainDlg->earth_resistivity.count()) << endl;
    out << tr("土壤电阻率：（Ω·m）") << "," << tr("土壤厚度：（m）") << endl;

    for(int i=0;i<mainDlg->earth_resistivity.count();i++){
        out << mainDlg->earth_resistivity.at(i) << "," << mainDlg->earth_thickness.at(i) << endl;
    }
}

