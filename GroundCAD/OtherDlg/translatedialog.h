#ifndef TRANSLATEDIALOG_H
#define TRANSLATEDIALOG_H
#include <QDialog>
#include "Base/conductor.h"
#include "Base/observedline.h"

//平移
namespace Ui {
class TranslateDialog;
}

class TranslateDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TranslateDialog(QWidget *parent = 0);
    ~TranslateDialog();

    double getX();
    double getY();
    double getZ();
    int flag();
private:
    Ui::TranslateDialog *ui;
};

//旋转
namespace Ui {
class RotateDialog;
}

class RotateDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RotateDialog(QWidget *parent = 0);
    ~RotateDialog();

    double angle();
private:
    Ui::RotateDialog *ui;
};

//缩放
namespace Ui {
class ScaleDialog;
}

class ScaleDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ScaleDialog(QWidget *parent = 0);
    ~ScaleDialog();

    double scale();
private:
    Ui::ScaleDialog *ui;
};


//编辑
namespace Ui {
class EditConductorDialog;
}

class EditConductorDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EditConductorDialog(QWidget *parent = 0);
    ~EditConductorDialog();

    void setConductor(Conductor c);
    Conductor getConductor();


private:
    Ui::EditConductorDialog *ui;
};

namespace Ui {
class ObLineEditDialog;
}

class ObLineEditDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ObLineEditDialog(QWidget *parent = 0);
    ~ObLineEditDialog();

    void setObLine(ObservedLine line);
    ObservedLine getObLine();

private:
    Ui::ObLineEditDialog *ui;
};

#endif // TRANSLATEDIALOG_H
