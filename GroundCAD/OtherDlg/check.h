#ifndef CHECK_H
#define CHECK_H

#include <QDialog>
#include "Base/conductor.h"

namespace Ui {
class Check;
}

class Check : public QDialog
{
    Q_OBJECT
    
public:
    explicit Check(QList<Conductor> list,QWidget *parent = 0);
    ~Check();

    bool check_space();
    bool check_outofearth();
    bool check_overlay();
    bool check_short();

    bool checkAll();

    enum{uncheck,passed,fail} isChecked;
    
private slots:
    void on_pushButton_check_clicked();

private:
    Ui::Check *ui;
    QList<Conductor> datalist;
    double minLength;
    double minSpace;
};

#endif // CHECK_H
