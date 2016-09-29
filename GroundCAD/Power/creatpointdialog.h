#ifndef CREATPOINTDIALOG_H
#define CREATPOINTDIALOG_H

#include <QDialog>

namespace Ui {
class CreatPointDialog;
}

class GroundingCADWindow;
class Point;

class CreatPointDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit CreatPointDialog(QWidget *parent = 0);
    ~CreatPointDialog();

    Point getCrossPoint();
    
private slots:
    void on_buttonBox_accepted();

private:
    Ui::CreatPointDialog *ui;

    GroundingCADWindow *groundingCADWindow; //父窗口指针

    void iniUi();       //ui初始化
};

#endif // CREATPOINTDIALOG_H
