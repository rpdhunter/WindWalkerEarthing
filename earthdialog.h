#ifndef EARTHDIALOG_H
#define EARTHDIALOG_H

#include <QDialog>
#include "windwalkerearthingmaindlg.h"

namespace Ui {
class EarthDialog;
}

class EarthDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit EarthDialog(QWidget *parent = 0);
    ~EarthDialog();
    
private slots:
    void on_spinBox_valueChanged(int arg1);

    void on_buttonBox_accepted();

    void tableInit();

private:
    Ui::EarthDialog *ui;
    WindWalkerEarthingMainDlg *mainDlg;

    void readFile();    //从标准位置读取文件
    void saveFile();    //保存文件至标准位置

};

#endif // EARTHDIALOG_H
