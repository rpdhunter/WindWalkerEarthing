#ifndef FEATURESDIALOG_H
#define FEATURESDIALOG_H

#include <QDialog>

namespace Ui {
class FeaturesDialog;
}

class GroundingCADWindow;

class FeaturesDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit FeaturesDialog(GroundingCADWindow *dlg,QWidget *parent = 0);
    ~FeaturesDialog();

    void getDefult();   //读取设置值
    
private slots:
    void on_lineEdit_Rou_xiangdui_editingFinished();

    void on_lineEdit_U_editingFinished();

    void on_lineEdit_Rou_editingFinished();

    void on_lineEdit_U_juedui_editingFinished();

private:
    Ui::FeaturesDialog *ui;
    GroundingCADWindow *groundingCADWindow;    
};

#endif // FEATURESDIALOG_H
