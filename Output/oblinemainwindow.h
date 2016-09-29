#ifndef OBLINEMAINWINDOW_H
#define OBLINEMAINWINDOW_H

#include <QMainWindow>


namespace Ui {
class ObLineMainWindow;
}

class ObLinePlot;
class QComboBox;
class WindWalkerEarthingMainDlg;
class ObservedLine;

//#include "armadillo"

//using namespace arma;

class ObLineMainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit ObLineMainWindow(QWidget *parent = 0);
    ~ObLineMainWindow();
    
private slots:
    void on_atn_Save_triggered();

    void on_atn_Print_triggered();

    void creatNewPlot(QString);


private:
    Ui::ObLineMainWindow *ui;

    WindWalkerEarthingMainDlg *mainDlg;

    QComboBox *comboBox;
    ObLinePlot *plot;

    QList<ObservedLine> observedLineList; //观测线的X坐标

    void readObLineX();

    void uiInit();
    void loadFiles();
};

#endif // OBLINEMAINWINDOW_H
