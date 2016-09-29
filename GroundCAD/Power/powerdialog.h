#ifndef POWERDIALOG_H
#define POWERDIALOG_H

#include <QDialog>
#include "Base/currentsource.h"

namespace Ui {
class PowerDialog;
}

class GroundingCADWindow;

class PowerDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit PowerDialog(QWidget *parent = 0);
    ~PowerDialog();    
    
private slots:
    void on_tBn_Add_clicked();

    void on_tBn_Del_clicked();

    void on_tBn_Up_clicked();

    void on_tBn_Down_clicked();

    void on_buttonBox_accepted();

    void inputCurrentList();     //读取主界面电流源

    void outputCurrentList();     //输出ui电流源

    void insertRow(int i,CurrentSource cs);       //插入一行表格

private:
    Ui::PowerDialog *ui;

    GroundingCADWindow *groundingCADWindow; //父窗口指针
};

#endif // POWERDIALOG_H
