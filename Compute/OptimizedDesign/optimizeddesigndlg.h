#ifndef OPTIMIZEDDESIGNDLG_H
#define OPTIMIZEDDESIGNDLG_H

#include <QDialog>

namespace Ui {
class OptimizedDesignDlg;
}

class ComputDialog;

class OptimizedDesignDlg : public QDialog
{
    Q_OBJECT
    
public:
    explicit OptimizedDesignDlg(ComputDialog *computeDlg,QWidget *parent = 0);
    ~OptimizedDesignDlg();
    
private slots:
    void on_pushButton_clicked();

private:
    Ui::OptimizedDesignDlg *ui;

    ComputDialog *dlg;


    void maxPointFound();       //寻找函数最大值
};

#endif // OPTIMIZEDDESIGNDLG_H
