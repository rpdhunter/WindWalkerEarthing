#include "GroundingCADWindow.h"
#include "ui_GroundingCADWindow.h"

#include <QStandardItemModel>
#include <QStandardItem>
#include <QClipboard>
#include <QLabel>
#include <QGraphicsTextItem>
#include <QFileDialog>
#include <QMessageBox>

#include "Base/compute.h"
#include "undocommand.h"
#include "Graphics/oblinelistmodle.h"
#include "Graphics/powerlistmodel.h"
#include "OtherDlg/featuresdialog.h"
#include "OtherDlg/newgrid.h"
#include "OtherDlg/translatedialog.h"
#include "OtherDlg/check.h"
#include "Power/powerdialog.h"

GroundingCADWindow::GroundingCADWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::GroundingCADWindow)
{
    ui->setupUi(this);

    //父窗口指针，用于参数传递
    windWalkerDlg=qobject_cast<WindWalkerEarthingMainDlg *>(parent);

    //设置标题
    QStringList list = windWalkerDlg->currentProjectDir.split("/");
    this->setWindowTitle(list.last() + " - GroundCAD");

    menuSetup();
    modelViewSetup();
    statusBarSetup();

    //设定数据检测状态为未检测
    isChecked=uncheck;
    ob_z = 0;
    ob_xmesh = 100;
    ob_ymesh = 100;

    readFile();     //读取文件数据

    defultR = 0.0067056;        //设置默认导体半径（设置这个数值是为了与CDEGS相一致，便于验证）
    defultRou = 1.7e-08;        //铜电阻率
    defultU = 1;                //铜相对磁导率
    defultRemarks = tr("铜");

    current = 1000;     //默认故障电流

}

GroundingCADWindow::~GroundingCADWindow()
{
    delete ui;
}


QPolygonF GroundingCADWindow::weiqiang()
{
    return weiqiangPolygon;
}

void GroundingCADWindow::setWeiqiang(QPolygonF p)
{
    weiqiangPolygon = p;
}

QList<Conductor> GroundingCADWindow::datalist()
{
    return scene->datalist();
}

//初始化菜单相关，构造函数调用
void GroundingCADWindow::menuSetup()
{
    pointerGroup=new QActionGroup(this);
    pointerGroup->addAction(ui->atn_Hand);
    pointerGroup->addAction(ui->atn_Point);
    pointerGroup->addAction(ui->atn_Pen);

    //布置浮动窗口初始位置
    tabifyDockWidget(ui->dockWidget_datalist,ui->dockWidget_cut);
    tabifyDockWidget(ui->dockWidget_datalist,ui->dockWidget_ObLine);
    tabifyDockWidget(ui->dockWidget_datalist,ui->dockWidget_Power);
    ui->dockWidget_datalist->raise();
    ui->dockWidget_cut->setHidden(true);    //切割窗口隐藏

    //撤销/重做的菜单
    undoStack = new QUndoStack(this);
    undoAction = undoStack->createUndoAction(this, tr("撤销"));
    undoAction->setShortcuts(QKeySequence::Undo);
    undoAction->setIcon(QIcon(":/GroundingCAD/images/GroundingCAD/undo.png"));
    ui->menu_E->addAction(undoAction);
    ui->editlBar->addAction(undoAction);
    redoAction = undoStack->createRedoAction(this, tr("重做"));
    redoAction->setShortcuts(QKeySequence::Redo);
    redoAction->setIcon(QIcon(":/GroundingCAD/images/GroundingCAD/redo.png"));
    ui->menu_E->addAction(redoAction);
    ui->editlBar->addAction(redoAction);


}

//初始化模型视图，构造函数调用
void GroundingCADWindow::modelViewSetup()
{
    //设定表格视图和数据模型的关系
    datamodel=new DataModel;
    ui->tableView_data->setModel(datamodel);
    cutmodel=new DataModel;
    ui->tableView_cut->setModel(cutmodel);
    selectionModel=new QItemSelectionModel(datamodel);
    ui->tableView_data->setSelectionModel(selectionModel);

    observedLineModel = new ObLineListModle;
    ui->tableView_Ob->setModel(observedLineModel);

    powerListModel = new PowerListModel;
    ui->tableView_Power->setModel(powerListModel);

    //设定场景和中心视图的关系
    scene =new GraphicsScene;
    view=new GraphicsView(this);
    view->setScene(scene);
    setCentralWidget(view);
    doMenuStateChanged();

    connect(ui->atn_Pen,SIGNAL(toggled(bool)),view,SLOT(setPenMode(bool)));

    connect(ui->tableView_data,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(on_atn_EditConductor_triggered()));

    connect(selectionModel,SIGNAL(selectionChanged(QItemSelection,QItemSelection)), //保证scene的选择和表格的选择同步
            scene,SLOT(doTableSelectionChanged(QItemSelection,QItemSelection)));
    connect(selectionModel,SIGNAL(selectionChanged(QItemSelection,QItemSelection)), //表格选择变化，也会影响到菜单的状态
            this,SLOT(doMenuStateChanged()));

    connect(scene,SIGNAL(itemMoved(QList<QGraphicsItem*>,QList<QPointF>,QList<QPointF>)),
            this, SLOT(itemMoved(QList<QGraphicsItem*>,QList<QPointF>,QList<QPointF>)));

    //根据scene给出的信号，更新表格
    connect(scene,SIGNAL(dataChanged(QList<Conductor>)),datamodel,SLOT(modelInit(QList<Conductor>)));
    connect(scene,SIGNAL(dataChanged(QList<Conductor>)),this,SLOT(doDatalistChanged()));
    connect(scene,SIGNAL(selectionChanged(QList<GraphicsLineItem*>)),this,SLOT(doSelectionChanged(QList<GraphicsLineItem*>)));
    connect(scene,SIGNAL(selectionChanged(QList<GraphicsLineItem*>)),this,SLOT(doMenuStateChanged()));

    //右键菜单
    connect(view,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(doCustomContextMenu(QPoint)));
    connect(ui->tableView_data,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(doCustomContextMenu(QPoint)));
    connect(ui->tableView_Ob,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(doObCustomContextMenu(QPoint)));
    connect(ui->tableView_Power,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(doPowerCustomContextMenu(QPoint)));
}

//初始化状态栏，构造函数调用
void GroundingCADWindow::statusBarSetup()
{
    //设定状态栏
    pointPos=new QLabel;
    progressBar=new QProgressBar;
    progressBar->setMaximumWidth(200);

    statusBar()->addPermanentWidget(progressBar);
    statusBar()->addPermanentWidget(pointPos);
    statusBar()->setMaximumHeight(15);

    progressBar->setHidden(true);
    statusBar()->showMessage("ready");
    connect(view,SIGNAL(mousePosChanged(QString)),pointPos,SLOT(setText(QString)));
}



//设定观测面信息
void GroundingCADWindow::setObRect(QRectF ob_r, int mesh_x, int mesh_y, double z)
{
    ui->atn_OBShow->setChecked(true);
    ob_rect = ob_r;
    //    conductor_rect = con_r;
    ob_z = z;
    ob_xmesh = mesh_x;
    ob_ymesh = mesh_y;
    ui->atn_OBShow->setChecked(false); //加入这两行，才能显示观测面，原因暂时未知
    ui->atn_OBShow->setChecked(true);
}

//添加观测线
void GroundingCADWindow::addObservedLine(ObservedLine line)
{
    this->observedLineList.append(line);
    this->doObservedLineChanged();

}

//添加对象
void GroundingCADWindow::on_atn_AddObject_triggered()
{
    NewGrid grid(scene,this);
    connect(&grid,SIGNAL(dataAdded(QList<Conductor> )),scene,SLOT(addConductor(QList<Conductor>)));
    connect(&grid,SIGNAL(obAdded(QRectF,int,int,double)),view,SLOT(setObRect(QRectF,int,int,double)));
    connect(&grid,SIGNAL(obAdded(QRectF,int,int,double)),this,SLOT(setObRect(QRectF,int,int,double)));
    connect(&grid,SIGNAL(observedLineAdded(ObservedLine)),this,SLOT(addObservedLine(ObservedLine)));
    grid.exec();

}

//切割导体
void GroundingCADWindow::on_atn_dataCut_triggered()
{
    if(isChecked!=passed){
        on_atn_check_triggered();
    }
    if(isChecked==passed){
        progressBar->setValue(0);
        progressBar->setHidden(false);
        statusBar()->showMessage(tr("正在切割导体"));

        Compute a;
        connect(&a,SIGNAL(progressValueChanged_cut(int)),progressBar,SLOT(setValue(int)));
        cutlist=a.cutConductor(scene->datalist());    //一次切割（交点切割）
        cutlist=a.secendCut(cutlist);       //二次切割（长度切割）


        progressBar->setValue(100);
        progressBar->setHidden(true);
        statusBar()->showMessage("ready");
        ui->dockWidget_cut->setHidden(false);   //显示切割窗口
        doCutlistChanged();
    }
}


void GroundingCADWindow::doDatalistChanged()
{
    //   datamodel->modelInit(datalist);
    ui->dockWidget_datalist->raise();
    ui->tableView_data->resizeColumnsToContents();

    isChecked=uncheck;

}

void GroundingCADWindow::doCutlistChanged()
{
    cutmodel->modelInit(cutlist);
    ui->dockWidget_cut->raise();
    ui->tableView_cut->resizeColumnsToContents();
}


void GroundingCADWindow::doObservedLineChanged()
{
    this->observedLineModel->modelInit(observedLineList);
    ui->tableView_Ob->resizeColumnsToContents();

}

void GroundingCADWindow::doPowerListChanged()
{
    this->powerListModel->modelInit(currentSourceList);
    ui->tableView_Power->resizeColumnsToContents();
}


//右键菜单
void GroundingCADWindow::doCustomContextMenu(const QPoint pos)
{
    doMenuStateChanged();
    QMenu menu(this);
    menu.addAction(ui->atn_EditConductor);
    menu.addAction(ui->atn_features);
    menu.addAction(ui->atn_AddObject);
    menu.addAction(ui->atn_Del);
    menu.addSeparator();
    menu.addAction(ui->atn_Cut);
    menu.addAction(ui->atn_Copy);
    menu.addAction(ui->atn_Paste);
    menu.addSeparator();
    menu.addAction(ui->atn_Group);
    menu.addAction(ui->atn_CancelGroup);
    menu.addAction(ui->atn_fixed);
    menu.addAction(ui->atn_CancelFix);
    menu.exec(QCursor::pos());
}

void GroundingCADWindow::doObCustomContextMenu(const QPoint pos)
{
    doMenuStateChanged();
    QMenu menu(this);
    menu.addAction(ui->atn_EditObLine);
    menu.addAction(ui->atn_DelObLine);
    menu.addAction(ui->atn_AddObject);
    menu.exec(QCursor::pos());
}

void GroundingCADWindow::doPowerCustomContextMenu(const QPoint pos)
{
    doMenuStateChanged();
    QMenu menu(this);
    menu.addAction(ui->atn_Power);
    menu.exec(QCursor::pos());
}

void GroundingCADWindow::itemMoved(QList<QGraphicsItem *> itemList, QList<QPointF> newPosList, QList<QPointF> oldPosList)
{
    undoStack->push(new MoveCommand(scene,itemList, newPosList, oldPosList));

    qDebug()<<undoStack->count();
}



void GroundingCADWindow::on_atn_zoomin_triggered()
{
    view->zoom_in();
}

void GroundingCADWindow::on_atn_zoomout_triggered()
{
    view->zoom_out();
}


void GroundingCADWindow::on_acn_Zoom1_triggered()
{
    view->zoom1();
}

void GroundingCADWindow::on_atn_fit_triggered()
{
    view->zoom_fit();
}

void GroundingCADWindow::on_atn_check_triggered()
{
    QList<Conductor> datalist=scene->datalist();
    Check *dlg=new Check(datalist);

    dlg->exec();
    if(dlg->isChecked==Check::passed){
        isChecked=passed;
    }else if(dlg->isChecked==Check::fail){
        isChecked=fail;
    }
}

//指针工具
void GroundingCADWindow::on_atn_Point_triggered()
{
    view->setDragMode(QGraphicsView::RubberBandDrag);
}

//手形工具
void GroundingCADWindow::on_atn_Hand_triggered()
{
    view->setDragMode(QGraphicsView::ScrollHandDrag);
}

//铅笔工具
void GroundingCADWindow::on_atn_Pen_toggled(bool arg1)
{
    if(arg1==true){
        setCursor(Qt::CrossCursor);
        view->setDragMode(QGraphicsView::NoDrag);
    }else{
        setCursor(Qt::ArrowCursor);
    }

}


//群组
void GroundingCADWindow::on_atn_Group_triggered()
{
    if (scene->selectedItems().count()<2)
        return;
    //  scene->setGroup();
    undoStack->push(new GroupCommand(scene));
}

//取消群组
void GroundingCADWindow::on_atn_CancelGroup_triggered()
{
    if(scene->selectedItems().count()!=1 && scene->selectedItems().first()->type()!=10)
        return;
    scene->cancelGroup();
    //    undoStack->push(new CancelGroupCommand(scene));

}

//固定
void GroundingCADWindow::on_atn_fixed_triggered()
{
    if (scene->selectedItems().count()==0)
        return;
    scene->setFixed();
    //    undoStack->push( new FixCommand(scene));
}

//取消固定
void GroundingCADWindow::on_atn_CancelFix_triggered()
{
    if (scene->selectedItems().count()==0)
        return;
    scene->cancelFix();
    //    undoStack->push(new UnFixCommand(scene));
}

//根据scene传来的选择状态，改变菜单的状态
void GroundingCADWindow::doMenuStateChanged()
{
    if(scene->selectedItems().count()>0){
        ui->atn_Copy->setEnabled(true);
        ui->atn_Cut->setEnabled(true);
        ui->atn_fixed->setEnabled(true);
        ui->atn_CancelFix->setEnabled(true);
        ui->atn_Del->setEnabled(true);
        ui->atn_rotate->setEnabled(true);
        ui->atn_scale->setEnabled(true);
        ui->atn_translate->setEnabled(true);
    }else{
        ui->atn_Copy->setEnabled(false);
        ui->atn_Cut->setEnabled(false);
        ui->atn_fixed->setEnabled(false);
        ui->atn_CancelFix->setEnabled(false);
        ui->atn_Del->setEnabled(false);
        ui->atn_rotate->setEnabled(false);
        ui->atn_scale->setEnabled(false);
        ui->atn_translate->setEnabled(false);
    }

    if(scene->selectedItems().count()>1){
        ui->atn_Group->setEnabled(true);
    }else{
        ui->atn_Group->setEnabled(false);
    }

    if(scene->selectedItems().count()==1 && scene->selectedItems().first()->type()==10 ){
        ui->atn_CancelGroup->setEnabled(true);
    }else{
        ui->atn_CancelGroup->setEnabled(false);
    }

    if(scene->selectedItems().count()==1 && scene->selectedItems().first()->type()==6){
        ui->atn_EditConductor->setEnabled(true);
    }else{
        ui->atn_EditConductor->setEnabled(false);
    }

    if(scene->copylist.count()==0)
        ui->atn_Paste->setEnabled(false);
    else
        ui->atn_Paste->setEnabled(true);
}

//void GroundingCADWindow::itemMoved(QGraphicsItem *movedItem, QPointF oldPosition)
//{
//    undoStack->push(new MoveCommand(movedItem, oldPosition));
//}

//全选
void GroundingCADWindow::on_atn_All_triggered()
{
    scene->selectAll();
}

//删除
void GroundingCADWindow::on_atn_Del_triggered()
{
    if (scene->selectedItems().isEmpty())
        return;
    //   scene->deleteSelected();
    undoStack->push(new DelCommand(scene));
}

// scene选择变化时，同步列表选择
void GroundingCADWindow::doSelectionChanged(QList<GraphicsLineItem *> list)
{
    disconnect(selectionModel,SIGNAL(selectionChanged(QItemSelection,QItemSelection)), //暂停scene的选择和表格的选择同步
               scene,SLOT(doTableSelectionChanged(QItemSelection,QItemSelection)));
    ui->tableView_data->clearSelection();
    for(int i=0;i<list.count();i++){
        if(list[i]->isSelected()==true){
            selectionModel->select(datamodel->index(i,0),QItemSelectionModel::Rows|QItemSelectionModel::Toggle);
        }
    }
    connect(selectionModel,SIGNAL(selectionChanged(QItemSelection,QItemSelection)), //恢复scene的选择和表格的选择同步
            scene,SLOT(doTableSelectionChanged(QItemSelection,QItemSelection)));
}

//剪切
void GroundingCADWindow::on_atn_Cut_triggered()
{
    scene->cut();
}

//复制
void GroundingCADWindow::on_atn_Copy_triggered()
{
    scene->copy();
}

//粘贴
void GroundingCADWindow::on_atn_Paste_triggered()
{
    scene->paste();
}

//平移导体
void GroundingCADWindow::on_atn_translate_triggered()
{
    TranslateDialog dlg(this);
    if(dlg.exec()==1){
        double x=dlg.getX();
        double y=dlg.getY();
        double z=dlg.getZ();
        if(dlg.flag()==0)
            scene->translate(x,y,z);
        else
            scene->translateTo(x,y,z);
    }
}

//缩放导体
void GroundingCADWindow::on_atn_scale_triggered()
{
    ScaleDialog dlg(this);
    if(dlg.exec()==1){
        scene->scale(dlg.scale());
    }
}

//旋转导体
void GroundingCADWindow::on_atn_rotate_triggered()
{
    RotateDialog dlg(this);
    if(dlg.exec()==1){
        scene->rotate(dlg.angle());
    }
}

//编辑导体
void GroundingCADWindow::on_atn_EditConductor_triggered()
{
    EditConductorDialog dlg(this);
    QGraphicsItem *item=scene->selectedItems().first();
    GraphicsLineItem *line=qgraphicsitem_cast<GraphicsLineItem *>(item);
    dlg.setConductor(line->node());
    if(dlg.exec()==1){
        scene->editConductor(dlg.getConductor());
    }
}

//编辑观测线
void GroundingCADWindow::on_atn_EditObLine_triggered()
{
    int row = ui->tableView_Ob->currentIndex().row();

    ObLineEditDialog dlg(this);
    dlg.setObLine(observedLineList.at(row));
    if(dlg.exec()==1){
        observedLineList.replace(row,dlg.getObLine());
        doObservedLineChanged();
    }
}

//删除观测线
void GroundingCADWindow::on_atn_DelObLine_triggered()
{
    int row = ui->tableView_Ob->currentIndex().row();
    int ret = QMessageBox::question(this,tr("删除"),tr("是否删除观测线%1 ?").arg(observedLineList.at(row).name()),
                                    QMessageBox::Ok|QMessageBox::Cancel);
    if(ret == QMessageBox::Ok){
        this->observedLineList.removeAt(row);
        doObservedLineChanged();
    }
}

//保存文件
void GroundingCADWindow::on_atn_Save_triggered()
{
    on_atn_SaveAs_triggered();
}

//打开文件
void GroundingCADWindow::on_atn_Open_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("打开文件"),QDir::currentPath(),
                                                    tr("Excel Files (*.csv);;txt Files (*.txt)"));
    if(fileName.isEmpty())
        return;

    read(fileName);     //调用读取的核心功能
}

//新建
void GroundingCADWindow::on_atn_New_triggered()
{
    scene->selectAll();
    scene->deleteSelected();

    //    view->setObRect(QRect(),0,0,0);     //清除观测面数据
}

//另存为
void GroundingCADWindow::on_atn_SaveAs_triggered()
{
    QString fileName = QFileDialog::getSaveFileName(this,tr("保存文件"),this->windWalkerDlg->currentProjectDir,
                                                    tr("Excel Files (*.csv);;txt Files (*.txt)"));
    if(fileName.isEmpty())
        return;

    save(fileName);     //调用保存的核心功能
}


//是否显示观测面
void GroundingCADWindow::on_atn_OBShow_toggled(bool arg1)
{
    this->view->setShowOb(arg1);
}

//关闭事件
//完成数据输入对应文件夹的文件
void GroundingCADWindow::closeEvent(QCloseEvent *event)
{
    int n=QMessageBox::question(this,tr("保存"),tr("是否保存至主工程？"),
                                QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel);
    if(n==QMessageBox::Yes){
        //传递datalist,ob_rect,ob_xmesh,ob_ymesh,ob_z几个参数到主窗口（父窗口）
        //        this->windWalkerDlg->datalist=scene->datalist();

        //        this->windWalkerDlg->ob_rect = this->ob_rect;
        //        this->windWalkerDlg->ob_xmesh=this->ob_xmesh;
        //        this->windWalkerDlg->ob_ymesh=this->ob_ymesh;
        //        this->windWalkerDlg->ob_z=this->ob_z;


        //保存文件至标准位置
        saveFile();

        emit datalistChangeSaved();
    }else if(n==QMessageBox::Cancel){
        event->ignore();
    }
}

//在标准位置读取文件
void GroundingCADWindow::readFile()
{
    QString fileName = this->windWalkerDlg->currentProjectDir + "/" + tr("输入数据.txt");
    read(fileName);
}

//读取核心功能
void GroundingCADWindow::read(QString fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;
    QTextStream in(&file);    // read the data serialized from the file

    QString string;
    int ret=QMessageBox::No ;

    while(!in.atEnd())
    {
        string=in.readLine();
        //导体
        if(string.contains(tr("导体数据"))){
            QList<Conductor> datalist;
            string = string.remove(tr("导体数据：（"));
            string = string.remove(tr("组）"));
            int n = string.toInt(); //得到导体组数

            for(int i=0;i<n;i++){   //
                string=in.readLine();
                Conductor c;
                c.input(string);
                datalist.append(c);
            }
            scene->addConductor(datalist);
            scene->doItemChanged();
        }
        //观测面数据
        else if(string.contains(tr("观测面"))){
            string=in.readLine();
            QStringList list = string.split(",");
            if(list.count()>=4){
                ob_rect.setRect(list.at(0).toDouble(),list.at(1).toDouble(),list.at(2).toDouble(),list.at(3).toDouble());
            }
            //            string=in.readLine();   //第二行没用
            string=in.readLine();   //第二行是ob_z,ob_xmesh,ob_ymesh
            list = string.split(",");
            if(list.count()>=3){
                ob_z = list.at(0).toDouble();
                ob_xmesh = list.at(1).toDouble();
                ob_ymesh = list.at(2).toDouble();
            }
            //            view->setObRect(ob_rect,ob_z,ob_xmesh,ob_ymesh);
            ui->atn_OBShow->setChecked(false); //加入这两行，才能显示观测面，原因暂时未知
            ui->atn_OBShow->setChecked(true);
        }
        //观测线数据
        else if(string.contains(tr("观测线"))){
            this->observedLineList.clear();
            string = string.remove(tr("观测线数据：（"));
            string = string.remove(tr("组）"));
            int n = string.toInt(); //得到导体组数

            for(int i=0;i<n;i++){   //
                string=in.readLine();
                QStringList list = string.split(",");
                if(list.count()!=8){
                    if(ret != QMessageBox::YesToAll){
                        ret = QMessageBox::warning(this, tr("警告"),tr("输入文件格式有误.\n" "是否任然继续?"),
                                                   QMessageBox::Yes | QMessageBox::YesToAll | QMessageBox::No,
                                                   QMessageBox::Yes);
                    }
                    if(ret == QMessageBox::No){
                        return;
                    }
                    if(list.count()<8){             //一行不够8个信息就补零
                        int fill=8-list.count();
                        for(int i=0;i<fill;i++){
                            list.append("0");
                        }
                    }
                }
                ObservedLine line;
                line.setPoints(Point(list.at(0).toDouble(),list.at(1).toDouble(),list.at(2).toDouble()),
                               Point(list.at(3).toDouble(),list.at(4).toDouble(),list.at(5).toDouble()));
                line.setName(list.at(6));
                line.setSubsection(list.at(7).toInt());
                this->observedLineList.append(line);
                this->doObservedLineChanged();
            }
        }
        //围墙数据
        else if(string.contains(tr("围墙"))){
            QPolygonF polygon;
            string=in.readLine();
            QStringList list = string.split(",");
            for(int i=0;i<list.count();i++){
                double x = list.at(i).split(" ").at(0).toDouble();
                double y = list.at(i).split(" ").at(1).toDouble();
                QPointF point(x,y);
                polygon << point;
            }
            //            qDebug()<<polygon;
            setWeiqiang(polygon);
        }
        //电源数据
        else if(string.contains(tr("电源"))){
            this->currentSourceList.clear();    //清空
            string = string.remove(tr("电源数据：（"));
            string = string.remove(tr("组）"));
            int n = string.toInt(); //得到电流源组数

            string=in.readLine();
            this->current = string.toDouble();  //得到等电位电流
            if(this->current == 0)
                this->current = 1000;

            for(int i=0;i<n;i++){   //得到不等电位电流源信息
                string=in.readLine();
                CurrentSource cs;
                cs.input(string);
                this->currentSourceList.append(cs);
                this->doPowerListChanged();
            }
        }

    };

}

//保存文件至标准位置
void GroundingCADWindow::saveFile()
{
    QString fileName = this->windWalkerDlg->currentProjectDir + "/" + tr("输入数据.txt");
    save(fileName);    //调用保存的核心功能
}


//保存核心功能
void GroundingCADWindow::save(QString fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return;
    QTextStream out(&file);

    //输出导体数据
    out << tr("导体数据：（%1组）").arg(scene->datalist().count()) << endl;
    foreach(Conductor c,scene->datalist()){     //保存10个导体信息
        out << c.output() <<endl;
    }

    //输出观测面数据
    out << tr("观测面数据：") << endl;
    if(ob_rect.isValid()){    //当前观测面存在
        out << ob_rect.x() << "," << ob_rect.y() << ","
            << ob_rect.width() << "," << ob_rect.height() <<endl;
    }
    else{       //当前观测面不存在，设置默认
        ob_rect = this->scene->itemsBoundingRect();
        double k = 0.25;
        ob_rect.adjust(-ob_rect.width()*k,-ob_rect.height()*k,ob_rect.width()*k,ob_rect.height()*k);
    }
    out << ob_z << "," << ob_xmesh << "," << ob_ymesh << endl;

    //输出观测线数据
    out << tr("观测线数据：（%1组）").arg(this->observedLineList.count()) << endl;
    foreach(ObservedLine line,this->observedLineList){
        out << line.P1().x << "," << line.P1().y << "," << line.P1().z << ","
            << line.P2().x << "," << line.P2().y << "," << line.P2().z << ","
            << line.name() << "," << line.subsection() << endl;
    }

    //输出围墙数据
    out << tr("围墙数据：") << endl;
    if(weiqiang().isEmpty()){     //如果为空，则用导体外边界代替
        QPolygonF polygon;
        polygon << scene->itemsBoundingRect().topLeft() << scene->itemsBoundingRect().topRight()
                << scene->itemsBoundingRect().bottomRight() << scene->itemsBoundingRect().bottomLeft();
        setWeiqiang(polygon);
    }
    for(int i=0;i<weiqiang().count();i++){
        out << weiqiang().at(i).x() << " " << weiqiang().at(i).y();
        if(i == weiqiang().count()-1){
            out << endl;    //最后一个输出换行符
        }
        else{
            out << ",";     //非最后一个用逗号隔开
        }
    }

    //输出电源数据
    out << tr("电源数据：（%1组）").arg(this->currentSourceList.count()) << endl;
    out << this->current << endl;       //第一组存放等电位电流值
    foreach(CurrentSource c,currentSourceList){     //保存不等电位电流源信息
        out << c.output() <<endl;       //具体存放过程做到了类结构中（应当是一种编程思路的进化吧，有精力可以讲其他几种IO都按此方式简化）
    }
}


//导体特性对话框(批量修改导体特性)
void GroundingCADWindow::on_atn_features_triggered()
{
    FeaturesDialog dlg(this);
    if(dlg.exec() == 1){
        dlg.getDefult();
        scene->setSelectedConductorFeatures(defultR,defultRou,defultU,defultRemarks);
    }
}


//输入电源
void GroundingCADWindow::on_atn_Power_triggered()
{
    PowerDialog dlg(this);
    dlg.exec();
    this->doPowerListChanged();
}
