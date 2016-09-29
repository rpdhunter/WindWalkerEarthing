#include "powerdialog.h"
#include "ui_powerdialog.h"
#include "../GroundingCADWindow.h"
#include "addnewpowerdialog.h"
#include "Base/currentsource.h"
#include <QtDebug>

PowerDialog::PowerDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PowerDialog)
{
    ui->setupUi(this);

    groundingCADWindow = qobject_cast<GroundingCADWindow*>(parent);

    inputCurrentList();
}

PowerDialog::~PowerDialog()
{
    delete ui;
}

//读取当前电流源设置，并初始化ui
void PowerDialog::inputCurrentList()
{
    ui->doubleSpinBox->setValue(groundingCADWindow->current);

    for(int i=0;i<groundingCADWindow->currentSourceList.count();i++){
        CurrentSource cs = groundingCADWindow->currentSourceList.at(i);
        insertRow(i,cs);
    }
}

void PowerDialog::outputCurrentList()
{
    groundingCADWindow->current = ui->doubleSpinBox->value();
    groundingCADWindow->currentSourceList.clear();

    //读取列表中的电流源
    for(int i=0;i<ui->tableWidget->rowCount();i++){
        CurrentSource cs;
        QString str;

        str = ui->tableWidget->item(i,0)->text();
        str.remove("(");
        str.remove(")");
        QStringList list = str.split(",");
        if(list.count()<3){
            int fill=3-list.count();
            for(int i=0;i<fill;i++){
                list.append("0");
            }
        }
        cs.P = Point(list.at(0).toDouble(),list.at(1).toDouble(),list.at(2).toDouble());

        str = ui->tableWidget->item(i,1)->text();
        str.remove("°");
        list = str.split("∠");
        if(list.count()<2){
            int fill=2-list.count();
            for(int i=0;i<fill;i++){
                list.append("0");
            }
        }
        cs.setCurrent(list.at(0).toDouble(),list.at(1).toDouble());

        str = ui->tableWidget->item(i,2)->text();
        cs.f = str.toDouble();

        str = ui->tableWidget->item(i,3)->text();
        cs.remarks = str;

        groundingCADWindow->currentSourceList.append(cs);
    }
}

//在第i行处插入一行数据
//注意生成新的行数为i,原来i行的数据下移
void PowerDialog::insertRow(int i, CurrentSource cs)
{
    ui->tableWidget->insertRow(i);
    QTableWidgetItem *item0 = new QTableWidgetItem(QString("(%1,%2,%3)").arg(cs.P.x).arg(cs.P.y).arg(cs.P.z));
    QTableWidgetItem *item1 = new QTableWidgetItem(QString("%1∠%2°").arg(cs.norm()).arg(cs.arg()));
    QTableWidgetItem *item2 = new QTableWidgetItem(QString("%1").arg(cs.f));
    QTableWidgetItem *item3 = new QTableWidgetItem(cs.remarks);
    ui->tableWidget->setItem(i,0,item0);
    ui->tableWidget->setItem(i,1,item1);
    ui->tableWidget->setItem(i,2,item2);
    ui->tableWidget->setItem(i,3,item3);
}

void PowerDialog::on_tBn_Add_clicked()
{
    AddNewPowerDialog dlg(this);
    if(dlg.exec()==1){
        CurrentSource cs = dlg.readUi();
        int row = ui->tableWidget->currentRow();
        insertRow(row+1,cs);
    };
}

void PowerDialog::on_tBn_Del_clicked()
{
    int row = ui->tableWidget->currentRow();
    if(row>=0){
        ui->tableWidget->removeRow(row);
    }
}

void PowerDialog::on_tBn_Up_clicked()
{
    int row = ui->tableWidget->currentRow();
    if(row >=1){
        //两行内容做一个交换
        for(int i=0;i<4;i++){
            QTableWidgetItem *item1 = ui->tableWidget->takeItem(row-1,i);
            QTableWidgetItem *item2 = ui->tableWidget->takeItem(row,i);
            ui->tableWidget->setItem(row-1,i,item2);
            ui->tableWidget->setItem(row,i,item1);
        }
        ui->tableWidget->selectRow(row-1);
    }
}

void PowerDialog::on_tBn_Down_clicked()
{
    int row = ui->tableWidget->currentRow();
    if(row >=0 && row<ui->tableWidget->rowCount()-1){
        //两行内容做一个交换
        for(int i=0;i<4;i++){
            QTableWidgetItem *item1 = ui->tableWidget->takeItem(row,i);
            QTableWidgetItem *item2 = ui->tableWidget->takeItem(row+1,i);
            ui->tableWidget->setItem(row,i,item2);
            ui->tableWidget->setItem(row+1,i,item1);
        }
        ui->tableWidget->selectRow(row+1);
    }
}

void PowerDialog::on_buttonBox_accepted()
{
    outputCurrentList();
}
