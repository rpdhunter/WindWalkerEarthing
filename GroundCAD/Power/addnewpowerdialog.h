#ifndef ADDNEWPOWERDIALOG_H
#define ADDNEWPOWERDIALOG_H

#include <QDialog>

namespace Ui {
class AddNewPowerDialog;
}
class CurrentSource;

class AddNewPowerDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit AddNewPowerDialog(QWidget *parent = 0);
    ~AddNewPowerDialog();

    void iniUi(CurrentSource cs);
    CurrentSource readUi();
    
private slots:
    void on_pushButton_clicked();

    void on_lineEdit_Real_editingFinished();

    void on_lineEdit_Imaginary_editingFinished();

    void on_lineEdit_Mo_editingFinished();

    void on_lineEdit_Fujiao_editingFinished();

private:
    Ui::AddNewPowerDialog *ui;
};

#endif // ADDNEWPOWERDIALOG_H
