#ifndef WINDWALKEREARTHINGMAINDLG_H
#define WINDWALKEREARTHINGMAINDLG_H

#include <QDialog>
#include "Base/conductor.h"
#include <QDir>
#include <QMap>

namespace Ui {
class WindWalkerEarthingMainDlg;
}

class GroundingCADWindow;
class Plot3DWindow;
class QListWidgetItem;


class WindWalkerEarthingMainDlg : public QDialog
{
    Q_OBJECT
    
public:
    explicit WindWalkerEarthingMainDlg(QWidget *parent = 0);
    ~WindWalkerEarthingMainDlg();

    QString currentDir;           //程序所在文件夹
    QString currentProjectDir;    //当前工作文件夹    

    QList<double> earth_thickness;      //土壤分层厚度
    QList<double> earth_resistivity;    //土壤分层电阻率

    bool removeDirWithContent(const QString &dirName);  //删除文件夹以及以下所有内容(公共函数)

    void editItem(QListWidgetItem * item);

public slots:
    void reject();
    
private slots:
    void btnStateChanged();

    void doCurrentProject(QListWidgetItem *item);   //进入当前工程
    void doEditItemName();                          //进入编辑状态
    void doCustomContextMenuRequested(QPoint p);    //右键菜单

    void on_btn_Input_toggled(bool checked);

    void on_btn_Output_toggled(bool checked);

    void on_btn_Compute_clicked();

    void on_btn_ADD_clicked();

    void on_btn_Del_clicked();

    void on_btn_Up_clicked();

    void on_btn_down_clicked();

    void on_toolBtn_GroundCAD_clicked();

    void on_toolBtn_Earth_clicked();

    void on_toolBtn_Plot3D_clicked();

    void on_toolButton_clicked();


    void projectRename(QListWidgetItem *item);
    void doOpenExlorer();       //在系统Explorer中显示

signals:
    void editFinished();

private:
    Ui::WindWalkerEarthingMainDlg *ui;

    GroundingCADWindow *w1;
    Plot3DWindow *w2;



//    QMap<QListWidgetItem* ,QString> projectList;   //当前项目和对应文件目录的映射表

    void iniDir();  //初始化工程文件夹

    void existCheck(QDir *temp);    //执行工程文件夹检查

    void doWidgetHide();
};

#endif // WINDWALKEREARTHINGMAINDLG_H
