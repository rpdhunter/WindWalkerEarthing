#include "check.h"
#include "ui_check.h"
#include "armadillo"
#include <QtDebug>
#include "Base/compute.h"

using namespace arma;

Check::Check(QList<Conductor> list, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Check)
{
    ui->setupUi(this);
    datalist=list;
    minLength=1;
    minSpace=1;

    isChecked=uncheck;
}

Check::~Check()
{
    delete ui;
}

bool Check::check_space()
{
    bool flag=true;
    ui->progressBar2->setValue(0);
    for(int i=0;i<datalist.count()-1;i++){
        for(int j=i+1;j<datalist.count();j++){
            double a=Compute::space(datalist[i],datalist[j]); //求得最短距离
            if( a>0.0001 && a<minSpace ){   //注意这里比较的条件
                ui->textEdit->append(tr("第%1段导体和第%2段导体最短距离为%3，低于%4的设定值，过近！").arg(i+1).arg(j+1).arg(a).arg(minSpace));
                flag=false;
            }
        }
        ui->progressBar2->setValue((i+1)*100.0/(datalist.count()-1));
    }
    return flag;
}

bool Check::check_outofearth()
{
    bool flag=true;
    ui->progressBar2->setValue(0);
    for(int i=0;i<datalist.count();i++){
        if(datalist[i].z0<=0||datalist[i].z1<=0){
            ui->textEdit->append(tr("第%1段导体部分位于地表！").arg(i+1));
            flag=false;
        }
        ui->progressBar2->setValue((i+1)*100.0/datalist.count());
    }
    return flag;
}

bool Check::check_overlay()
{
    bool flag=true;
    ui->progressBar2->setValue(0);
    for(int i=0;i<datalist.count()-1;i++){
        for(int j=i+1;j<datalist.count();j++){
            if(Compute::isCoincide(datalist[i],datalist[j])){
                ui->textEdit->append(tr("第%1段导体和第%2段导体重合！").arg(i+1).arg(j+1));
                flag=false;
            }
        }
        ui->progressBar2->setValue((i+1)*100.0/(datalist.count()-1));
    }
    return flag;
}

bool Check::check_short()
{
    bool flag=true;
    ui->progressBar2->setValue(0);
    for(int i=0;i<datalist.count();i++){
        if(datalist[i].length()<minLength){
            ui->textEdit->append(tr("第%1段导体长度为%2，不到%3米，过短！").arg(i+1).arg(datalist[i].length()).arg(minLength));
            flag=false;
        }
        ui->progressBar2->setValue((i+1)*100.0/datalist.count());
    }
    return flag;
}

bool Check::checkAll()
{

    bool flag=true;
    ui->progressBar1->setValue(0);

    if(ui->check_close->isChecked()){
        ui->label_prograss2->setText(ui->check_close->text());
        if(check_space()){
            ui->label_close->setText(tr("<font color=green>无问题</font>"));
        }else{
            ui->label_close->setText(tr("<font color=red>有问题</font>"));
            flag=false;
        }
    }
    ui->progressBar1->setValue(25);
    if(ui->check_outofearth->isChecked()){
        ui->label_prograss2->setText(ui->check_outofearth->text());
        if(check_outofearth()){
            ui->label_outofearth->setText(tr("<font color=green>无问题</font>"));
        }else{
            ui->label_outofearth->setText(tr("<font color=red>有问题</font>"));
            flag=false;
        }
    }
    ui->progressBar1->setValue(50);
    if(ui->check_overlay->isChecked()){
        ui->label_prograss2->setText(ui->check_overlay->text());
        if(check_overlay()){
            ui->label_overlay->setText(tr("<font color=green>无问题</font>"));
        }else{
            ui->label_overlay->setText(tr("<font color=red>有问题</font>"));
            flag=false;
        }
    }
    ui->progressBar1->setValue(75);
    if(ui->check_short->isChecked()){
        ui->label_prograss2->setText(ui->check_short->text());
        if(check_short()){
            ui->label_short->setText(tr("<font color=green>无问题</font>"));
        }else{
            ui->label_short->setText(tr("<font color=red>有问题</font>"));
            flag=false;
        }
    }
    ui->progressBar1->setValue(100);
    return flag;
}

void Check::on_pushButton_check_clicked()
{
    ui->textEdit->clear();
    if(checkAll())
        isChecked=passed;
    else
        isChecked=fail;

//    Conductor A,B,C;
//    A.dataNodeInit(0,0,0,0,98.2,0);
//    B.dataNodeInit(0,98.2,0,0,4,1);

//    C=Compute::getCrossPoint(A,B);

//    qDebug()<<tr("AB之间的距离是")<<C.length();
//    qDebug()<<tr("交点是：")<<C.x0<<','<<C.y0<<','
//           <<C.z0<<','<<C.x1<<','<<C.y1<<','<<C.z1;

}
