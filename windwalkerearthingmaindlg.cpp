#include "windwalkerearthingmaindlg.h"
#include "ui_windwalkerearthingmaindlg.h"
#include <QMessageBox>
#include <QMenu>
#include <QProcess>
#include <QtDebug>

#include "GroundCAD/GroundingCADWindow.h"
#include "Output/ObservedRect/plot3dwindow.h"
#include "Base/compute.h"
#include "Compute/computdialog.h"
#include "Compute/cutthread.h"
#include "earthdialog.h"

#include "Output/oblinemainwindow.h"
//#include "Plot3D/tvplot.h"
//#include "Qwt/qwtex.cpp"

WindWalkerEarthingMainDlg::WindWalkerEarthingMainDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::WindWalkerEarthingMainDlg)
{
    ui->setupUi(this);
    ui->widget->setHidden(true);
    ui->widget_2->setHidden(true);

    currentDir = QDir::currentPath();


    connect(ui->listWidget,SIGNAL(itemPressed(QListWidgetItem*)),this,SLOT(doCurrentProject(QListWidgetItem*)));
    connect(ui->listWidget,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(doCustomContextMenuRequested(QPoint)));
    connect(ui->listWidget,SIGNAL(itemChanged(QListWidgetItem*)),this,SLOT(projectRename(QListWidgetItem*)));

    iniDir();
    doWidgetHide();
    btnStateChanged(); //按钮状态变化
}

WindWalkerEarthingMainDlg::~WindWalkerEarthingMainDlg()
{
    delete ui;
}

//初始化文件夹
void WindWalkerEarthingMainDlg::iniDir()
{
    QDir *temp = new QDir;
    existCheck(temp);

    //读入工程，按建立时间倒序排序
//    QFileInfoList infoList = temp->entryInfoList(QDir::Dirs|QDir::NoDotAndDotDot,QDir::Time|QDir::Reversed);
    QFileInfoList infoList = temp->entryInfoList(QDir::Dirs|QDir::NoDotAndDotDot);
    foreach(QFileInfo info,infoList){
        QListWidgetItem *item = new QListWidgetItem(info.fileName());
        item->setFlags(Qt::ItemIsEnabled|Qt::ItemIsEditable|Qt::ItemIsSelectable);
        item->setData(Qt::UserRole,info.absoluteFilePath());    //在用户数据里面保存绝对路径
        ui->listWidget->addItem(item);
    }

}

//为了代码重用设立的便捷函数
//实现了“工程”文件夹的检查，并定位到工程文件夹中
void WindWalkerEarthingMainDlg::existCheck(QDir *temp)
{
    //检测工程文件夹（避免错误）
    bool ok = temp->exists(currentDir + "/" + tr("工程") );
    if(!ok){
        //不存在工程文件夹，重新建立
        temp->mkdir(currentDir+"/" + tr("工程"));
    }
    temp->cd(tr("工程"));
}

//进入当前工程(做包括改名一类的工作)
void WindWalkerEarthingMainDlg::doCurrentProject(QListWidgetItem* item)
{
//qDebug()<<"cao ni ma11111111";
    QDir *temp = new QDir;
    existCheck(temp);

    //开始处理当前功能
    currentProjectDir = item->data(Qt::UserRole).toString();
    btnStateChanged();
    ui->widget->setHidden(true);
    ui->btn_Input->setChecked(false);
    ui->btn_Output->setChecked(false);




//    for(int row=0;row<ui->listWidget->count();row++){
//        item = ui->listWidget->item(row);
//        if(projectList.value(item) == temp->absolutePath()+"/"+item->text()){
//            ok = temp->exists(projectList.value(item));
//            if(ok){
//                //说明三位（listWidget,projectList,实际文件夹）一体，完全正常
//                if(ui->listWidget->currentRow() == row){
//                    //说明这是当前显示的工程，插入工程显示处理代码
//                    currentProjectDir = projectList.value(item);
////                        qDebug()<<"cao ni ma222222222222222";
//                    btnStateChanged();
//                    //隐藏右部窗口
//                    ui->widget->setHidden(true);
//                    ui->btn_Input->setChecked(false);
//                    ui->btn_Output->setChecked(false);

//                }
//            }else{
//                //说明程序数据和实际文件夹不一致（可能文件夹在程序外被修改（重命名或者删除））
//                temp->mkdir(projectList.value(item));
//            }
//        }else{
//            //说明三位listWidget和projectList数据不同步，可能有程序内重命名发生
//            if(temp->exists(projectList.value(item))){
//                //这种情况确定发生了程序内的重命名
//                ok = temp->rename(projectList.value(item),item->text());
//                if(ok){ //重命名成功
//                    projectList[item] = temp->absolutePath()+ "/" + item->text();
//                }else{  //重命名失败
////                    QMessageBox::warning(this,tr("重命名失败"),tr("重命名的文件夹名字不合规范，或者已存在相同名字的文件夹！/n请重新操作！"));
////                    temp->cd(projectList.value(item));
////                    item->setText(temp->dirName());
////                    ui->listWidget->editItem(item);
//                    return;
//                }
//            }else{//不存在，说明老文件夹已没有，建立新文件夹
//                temp->mkdir(item->text());
//                projectList[item] = temp->absolutePath() + "/" + item->text();
//            }
//        }
//    }
}

//进入编辑状态
void WindWalkerEarthingMainDlg::doEditItemName()
{
    ui->listWidget->editItem(ui->listWidget->currentItem());
}


//右键菜单
void WindWalkerEarthingMainDlg::doCustomContextMenuRequested(QPoint p)
{
    QMenu menu;
    menu.addAction(QIcon(":/GroundingCAD/images/GroundingCAD/delete.png"),tr("删除"),this,SLOT(on_btn_Del_clicked()));
    menu.addAction(tr("重命名"),this,SLOT(doEditItemName()));
    menu.addAction(QIcon(":/GroundingCAD/images/GroundingCAD/arrow_up.png"),tr("上移"),this,SLOT(on_btn_Up_clicked()));
    menu.addAction(QIcon(":/GroundingCAD/images/GroundingCAD/arrow_down.png"),tr("下移"),this,SLOT(on_btn_down_clicked()));
    menu.addAction(tr("在Exprorer中显示"),this,SLOT(doOpenExlorer()));

    QPoint point = ui->listWidget->mapToGlobal(p);
    menu.exec(point);
}

//编辑
//void WindWalkerEarthingMainDlg::setDatalist(QList<Conductor> list)
//{
//    datalist=list;
//}

//重新实现程序的退出函数
void WindWalkerEarthingMainDlg::reject()
{
//    int flag=QMessageBox::question(this,tr("WWE"),tr("退出前需要保存吗？"),
//             QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel,QMessageBox::Yes);
//    if(flag == QMessageBox::Yes){
//        saveProject();
//        QDialog::reject();
//    }
//    if(flag == QMessageBox::No)
    QDialog::reject();
}




//开启/关闭输入面板
void WindWalkerEarthingMainDlg::on_btn_Input_toggled(bool checked)
{
    if(ui->btn_Input->isChecked()){
        if(ui->btn_Output->isChecked()){
            ui->btn_Output->setChecked(false);
        }
        ui->widget->setVisible(true);
        ui->stackedWidget->setCurrentIndex(0);
    }else{
        ui->widget->setVisible(false);
    }
}

//开启/关闭输出面板
void WindWalkerEarthingMainDlg::on_btn_Output_toggled(bool checked)
{
    if(ui->btn_Output->isChecked()){
        if(ui->btn_Input->isChecked()){
            ui->btn_Input->setChecked(false);
        }
        ui->widget->setVisible(true);
        ui->stackedWidget->setCurrentIndex(1);
    }else{
        ui->widget->setVisible(false);
    }
}

//计算
void WindWalkerEarthingMainDlg::on_btn_Compute_clicked()
{
    ui->widget->setHidden(true);
    ui->btn_Input->setChecked(false);
    ui->btn_Output->setChecked(false);

    //为计算生成新的显示面板(带有基本的时间显示功能)
    ComputDialog dlg(this);
    dlg.show();




    dlg.exec();

}


//适当时候改变几个按钮的状态(完全根据对应工程目录下的文件)
void WindWalkerEarthingMainDlg::btnStateChanged()
{
    if(ui->listWidget->currentRow()>=0){

        QListWidgetItem *item = ui->listWidget->currentItem();  //为了书写方便
        ui->btn_Input->setEnabled(true);
        //        if(projectList.contains(ui->listWidget->currentItem())){
        QDir *temp = new QDir;
        bool ok1 = temp->exists(item->data(Qt::UserRole).toString() + "/" + tr("输入数据.txt"));
        bool ok2 = temp->exists(item->data(Qt::UserRole).toString() + "/" + tr("土壤数据.txt"));
        ui->btn_Compute->setEnabled(ok1 && ok2);
        //根据输入文件状态，改变图标颜色
        if(ok1)
            ui->toolBtn_GroundCAD->setIcon(QIcon(":/Other/images/Other/grid.png"));
        else
            ui->toolBtn_GroundCAD->setIcon(QIcon(":/Other/images/Other/grid_black.png"));
        if(ok2)
            ui->toolBtn_Earth->setIcon(QIcon(":/Other/images/Other/earth.png"));
        else
            ui->toolBtn_Earth->setIcon(QIcon(":/Other/images/Other/earth_black.png"));

        //        }
        ui->btn_Output->setEnabled(true);

    }
    else{
        ui->btn_Input->setEnabled(false);
        ui->btn_Compute->setEnabled(false);
        ui->btn_Output->setEnabled(false);
    }
}



//listWidget的添加按钮
void WindWalkerEarthingMainDlg::on_btn_ADD_clicked()
{    
    //文件操作
    QDir *temp = new QDir;
    existCheck(temp);

    int i=0;
    bool ok;
    do{
        i++;
        ok = temp->mkdir(currentDir+tr("//工程//新项目%1").arg(i));
    }while(!ok);

    temp->cd(currentDir+tr("//工程//新项目%1").arg(i));

    //ui操作
    int row=ui->listWidget->currentRow();
    ui->listWidget->insertItem(row+1,tr("新项目%1").arg(i));
    ui->listWidget->setCurrentRow(row+1);
    //建立文件夹和绝对路径的联系
//    projectList[ui->listWidget->currentItem()] = temp->absolutePath();
    ui->listWidget->currentItem()->setData(Qt::UserRole,temp->absolutePath());
    ui->listWidget->currentItem()->setFlags(Qt::ItemIsEnabled|Qt::ItemIsEditable|Qt::ItemIsSelectable);
    ui->listWidget->editItem(ui->listWidget->currentItem());

    doWidgetHide();
    doCurrentProject(ui->listWidget->currentItem());
}

//listWidget的删除按钮
void WindWalkerEarthingMainDlg::on_btn_Del_clicked()
{
    if(ui->listWidget->currentRow() >= 0){
        int flag=QMessageBox::question(this,tr("WWE"),
                                       tr("将会删除工程【%1】中所有数据\n\n是否确定删除？").arg(ui->listWidget->currentItem()->text()),
                                       QMessageBox::Yes|QMessageBox::No,QMessageBox::No);
        if(flag == QMessageBox::Yes){
            //文件操作(调用删除文件夹的函数)
            bool b;
            b=removeDirWithContent(currentDir+tr("//工程//")+ui->listWidget->currentItem()->text());

            //ui操作
            ui->listWidget->takeItem(ui->listWidget->currentRow());
            doWidgetHide();
        }
    }
}

/*递归删除文件夹,即使里面有内容
**
**dir :文件夹的名字,绝对路径和相对路径均可
**
**返回值 :成功后返回true;否则返回false
*/
bool WindWalkerEarthingMainDlg::removeDirWithContent(const QString &dirName)
{
    static QVector<QString> dirNames;
    static QString funcErrMsg="删除[%1]失败.";
    static QString funcInfFndMsg="发现[%1].";
    static QString funcInfDelMsg="删除[%1]成功.";
    QDir dir;
    QFileInfoList filst;
    QFileInfoList::iterator curFi;
    //初始化
    dirNames.clear();
    if(dir.exists()){
        dirNames<<dirName;
    }
    else{
        return true;
    }
    //遍历各级文件夹，并将这些文件夹中的文件删除
    for(int i=0;i<dirNames.size();++i){
        dir.setPath(dirNames[i]);
        filst=dir.entryInfoList(QDir::Dirs|QDir::Files
                                |QDir::Readable|QDir::Writable
                                |QDir::Hidden|QDir::NoDotAndDotDot
                                ,QDir::Name);
        if(filst.size()>0){
            curFi=filst.begin();
            while(curFi!=filst.end()){
                 //遇到文件夹,则添加至文件夹列表dirs尾部
                if(curFi->isDir()){
                    dirNames.push_back(curFi->filePath());
                }else if(curFi->isFile()){
                    //遇到文件,则删除之
                    if(!dir.remove(curFi->fileName())){
                        return false;
                    }
                }
                curFi++;
            }//end of while
        }
    }
    //删除文件夹
    for(int i=dirNames.size()-1;i>=0;--i){
        dir.setPath(dirNames[i]);
        if(!dir.rmdir(".")){
            return false;
        }
    }
    return true;
}

//工程文件夹重命名
void WindWalkerEarthingMainDlg::projectRename(QListWidgetItem* item)
{
//    qDebug()<<"item changed!"<<item->text();
    QString path = item->data(Qt::UserRole).toString();

    QString oldName = path.split('/').last();
    if(item->text() == oldName){
//         qDebug()<<tr("新建工程!");
    }
    else{
        qDebug()<<tr("重命名工程!");

        QDir *temp = new QDir;
        existCheck(temp);
        bool ok = temp->rename(path,item->text());
        if(ok){ //重命名成功
             qDebug()<<tr("重命名成功!");
             temp->cd(item->text());
             item->setData(Qt::UserRole,temp->absolutePath());
        }else{  //重命名失败
            QMessageBox::warning(this,tr("重命名失败"),tr("重命名的文件夹名字不合规范，或者已存在相同名字的文件夹！\n请重新操作！"));
            item->setText(oldName);
            ui->listWidget->setCurrentItem(item);
            ui->listWidget->editItem(item);
        }
    }
}

void WindWalkerEarthingMainDlg::doOpenExlorer()
{
    QString path=ui->listWidget->currentItem()->data(Qt::UserRole).toString();
//    QString path=QDir::currentPath();       //获取程序当前目录
    path.replace("/","\\");                 //将地址中的"/"替换为"\"，因为在Windows下使用的是"\"。
    QProcess::startDetached("explorer "+path);//打开上面获取的目录
}

//listWidget的上移按钮
void WindWalkerEarthingMainDlg::on_btn_Up_clicked()
{
    int row=ui->listWidget->currentRow();
    if(row>0){
        QListWidgetItem *item=ui->listWidget->takeItem(row);
        ui->listWidget->insertItem(row-1,item);
        ui->listWidget->setCurrentRow(row-1);
    }
}

//listWidget的下降按钮
void WindWalkerEarthingMainDlg::on_btn_down_clicked()
{
    int row=ui->listWidget->currentRow();
    if(row<ui->listWidget->count()-1){
        QListWidgetItem *item=ui->listWidget->takeItem(row);
        ui->listWidget->insertItem(row+1,item);
        ui->listWidget->setCurrentRow(row+1);
    }
}

//判断主widget是否隐藏
void WindWalkerEarthingMainDlg::doWidgetHide()
{
    if(ui->listWidget->count() == 0){
        ui->widget_2->setVisible(false);
    }else{
        ui->widget_2->setVisible(true);
    }
}



//接地网结构输入,调用GroundingCAD
void WindWalkerEarthingMainDlg::on_toolBtn_GroundCAD_clicked()
{
    w1 = new GroundingCADWindow(this);
    w1->show();

    //如果成功保存，改变主窗口按钮状态
    connect(w1,SIGNAL(datalistChangeSaved()),this,SLOT(btnStateChanged()));
}

//土壤结构输入
void WindWalkerEarthingMainDlg::on_toolBtn_Earth_clicked()
{
    EarthDialog dlg(this);
    dlg.exec();
    btnStateChanged();
}


//调用Plot3D
void WindWalkerEarthingMainDlg::on_toolBtn_Plot3D_clicked()
{
    w2 = new Plot3DWindow(this);
    w2->show();
}

//观测线
void WindWalkerEarthingMainDlg::on_toolButton_clicked()
{
//    TVPlot *plot = new TVPlot;

//    plot->resize(600,400);
//    plot->show();
    ObLineMainWindow *m = new ObLineMainWindow(this);
    m->show();
}


