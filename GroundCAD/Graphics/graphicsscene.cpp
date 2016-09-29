#include "graphicsscene.h"
#include "graphicslineitem.h"

#include <QPainter>
#include <QtDebug>
#include <QGraphicsSceneWheelEvent>
#include <QApplication>
#include <QClipboard>

GraphicsScene::GraphicsScene(QObject *parent) :
    QGraphicsScene(parent)
{
    setSceneRect(-800,-600,1600,1200);
   // movingItem=0;
    connect(this,SIGNAL(selectionChanged()),this,SLOT(doSelectionChanged()));   //自身通知自身发射选择变化信号
}

//简单的主数据存取函数
QList<GraphicsLineItem *> GraphicsScene::mainitemlist()
{
    return itemlist;
}

void GraphicsScene::setMainItemList(QList<GraphicsLineItem *> list)
{
    itemlist=list;
}

//取得主数据中存储的导体信息
QList<Conductor> GraphicsScene::datalist()
{
    QList<Conductor> datalist;
    GraphicsLineItem *line;
    foreach(line,itemlist){
        datalist.append(line->node());
    }
    return datalist;
}

QList<Conductor> GraphicsScene::selectedList()
{
    QList<Conductor> datalist;
    GraphicsLineItem *line;
    foreach(line,itemlist){
        if(line->isSelected()){
            datalist.append(line->node());
        }
    }
    return datalist;
}

//添加导体
//步骤：1.主视图添加 2.主数据添加
void GraphicsScene::addConductor(QList<Conductor> list)
{
    disconnect(this,SIGNAL(selectionChanged()),this,SLOT(doSelectionChanged()));   //保证性能，暂时停止连接
    this->clearSelection();
    for(int i=0;i<list.count();i++){
        GraphicsLineItem *line=new GraphicsLineItem(list[i]);
        addItem(line);              //视图操作
        line->setSelected(true);
        itemlist.append(line);    //主数据结构操作        
    }
    this->update();
    connect(this,SIGNAL(selectionChanged()),this,SLOT(doSelectionChanged()));   //自身通知自身发射选择变化信号

//    doSelectionChanged();   //发送数据变化信号，用于及时更新主视图导体数据表格
}

//删除选中导体
//步骤：1.主数据删除 2.主视图删除
void GraphicsScene::deleteSelected()
{
    disconnect(this,SIGNAL(selectionChanged()),this,SLOT(doSelectionChanged()));   //保证性能，暂时停止连接
    if(this->selectedItems().count()>0){
        for(int i=0;i<itemlist.count();i++){    //主数据删除
            GraphicsLineItem *line=itemlist[i];
            if(line->isSelected()){
                itemlist.removeAt(i);
                i--;
            }
        }
        QGraphicsItem *item;        //视图删除（群组自动删除子对象）
        foreach(item,this->selectedItems()){
            this->removeItem(item);
        }
    }
    doItemChanged();
    connect(this,SIGNAL(selectionChanged()),this,SLOT(doSelectionChanged()));   //自身通知自身发射选择变化信号
}

//全选函数
//步骤1.改变视图
//主数据不动
void GraphicsScene::selectAll()
{
    disconnect(this,SIGNAL(selectionChanged()),this,SLOT(doSelectionChanged()));   //保证性能，暂时停止连接
    QList<QGraphicsItem *> list=this->items();
    QGraphicsItem *item;
    foreach(item,list){
        item->setSelected(true);
    }
    doSelectionChanged();
    connect(this,SIGNAL(selectionChanged()),this,SLOT(doSelectionChanged()));   //自身通知自身发射选择变化信号
}

//编辑一个节点
void GraphicsScene::editConductor(Conductor c)
{
    if(this->selectedItems().count()==1&&this->selectedItems().first()->type()==6){
        GraphicsLineItem *line=qgraphicsitem_cast<GraphicsLineItem *>(this->selectedItems().first());
        line->setNode(c);
        this->doItemChanged();
    }
}

//批量设置选择导体的特性值
void GraphicsScene::setSelectedConductorFeatures(double r,double rou,double u,QString remarks)
{
    if(this->selectedItems().count()>0){
        for(int i=0;i<this->selectedItems().count();i++){
            GraphicsLineItem *line=qgraphicsitem_cast<GraphicsLineItem *>(this->selectedItems().at(i));
            line->setFeatures(r,rou,u,remarks);
        }
        this->doItemChanged();
    }
}


//结成群组
//纯场景操作，无需修改主数据，也无需主动发出信号
void GraphicsScene::setGroup()
{
    if(this->selectedItems().count()>1){
        QGraphicsItemGroup *group=this->createItemGroup(this->selectedItems());
        group->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemIsMovable
                        |QGraphicsItem::ItemHasNoContents);
        group->setSelected(true);
    }
}

//取消群组
void GraphicsScene::cancelGroup()
{
    if(this->selectedItems().count()==1){
        QList<QGraphicsItem *> list=this->selectedItems()[0]->childItems();
        if(list.count()>0){
            this->destroyItemGroup(list[0]->group());
            QGraphicsItem *item;
            foreach(item,list){     //可能是形成group的时候经过了格式转换，如不加入以下代码，将产生一些错误
                if(item->type()==6){
                    GraphicsLineItem *line=qgraphicsitem_cast<GraphicsLineItem *>(item);
                    this->removeItem(item);
                    this->addItem(line);
                }
                if(item->type()==10){
                    QGraphicsItemGroup *group=qgraphicsitem_cast<QGraphicsItemGroup *>(item);
                    this->removeItem(group);
                    this->addItem(group);
                }
            }
        }
    }
}

//固定
void GraphicsScene::setFixed()
{
    if(this->selectedItems().count()>0){
        QList<QGraphicsItem *> list=this->selectedItems();
        QGraphicsItem *item;
        foreach(item,list){
            if(item->type()==10){
                item->setFlags(QFlags<QGraphicsItem::GraphicsItemFlag>(QGraphicsItem::ItemIsSelectable|
                                                                       QGraphicsItem::ItemHasNoContents));
            }else if(item->type()==6){
                item->setFlags(QFlags<QGraphicsItem::GraphicsItemFlag>(QGraphicsItem::ItemIsSelectable));
            }
        }
    }
}

//取消固顶
void GraphicsScene::cancelFix()
{
    if(this->selectedItems().count()>0){
        QList<QGraphicsItem *> list=this->selectedItems();
        QGraphicsItem *item;
        foreach(item,list){
            item->setFlags(item->flags()|QGraphicsItem::ItemIsMovable);
        }
    }
}

//剪切
void GraphicsScene::cut()
{
    if(this->selectedItems().count()>0){
        copy();
        deleteSelected();
    }

}

//复制
void GraphicsScene::copy()
{
    if(this->selectedItems().count()>0){
        copylist.clear();
        GraphicsLineItem *line;
        foreach(line,itemlist){
            if(line->isSelected()){
                Conductor c=line->node();
                c.x0=line->mapToScene(line->line().p1()).x()+100;
                c.y0=line->mapToScene(line->line().p1()).y()+100;
                c.x1=line->mapToScene(line->line().p2()).x()+100;
                c.y1=line->mapToScene(line->line().p2()).y()+100;
                GraphicsLineItem *newline= new GraphicsLineItem(c);
                copylist.append(newline);
            }
        }
    }
    //以下为对剪切板的操作
    QClipboard *clipboard = QApplication::clipboard();

    QList<Conductor> list = this->selectedList();   //获得当前选择的导体组

    QString newText;
    QStringList newlist;
    Conductor c;
    foreach(c,list){
        newlist.clear();
        newlist<<tr("%1").arg(c.x0)<<tr("%1").arg(c.y0)<<tr("%1").arg(c.z0)
              <<tr("%1").arg(c.x1)<<tr("%1").arg(c.y1)<<tr("%1").arg(c.z1)<<tr("%1\n").arg(c.r);

        newText += newlist.join("\t");
    }
    clipboard->setText(newText);

}

//粘贴
void GraphicsScene::paste()
{
    if(copylist.count()>0){
        GraphicsLineItem *line;
        foreach(line,copylist){
            this->addItem(line);
            itemlist.append(line);
        }
    }
    doItemChanged();
}

//平移
void GraphicsScene::translate(double x, double y, double z)
{
    if(this->selectedItems().count()>0){
        GraphicsLineItem *line;
        foreach(line,itemlist){
            if(line->isSelected()){
                Conductor c=line->node();
                c.x0+=x;
                c.y0+=y;
                c.z0+=z;
                c.x1+=x;
                c.y1+=y;
                c.z1+=z;
                line->setNode(c);
                line->update();
            }
        }
        doItemChanged();
    }
}

//平移到
void GraphicsScene::translateTo(double x, double y, double z)
{
    if(this->selectedItems().count()>0){
        QGraphicsItem *item;
        foreach(item,this->selectedItems()){
            item->setPos(x,y);
        }
        GraphicsLineItem *line;
        foreach(line,itemlist){
            if(line->isSelected()){
                Conductor c=line->node();
                c.z0=z;
                c.z1=z;
                line->setNode(c);
            }
        }
        doItemChanged();
    }
}

//旋转
void GraphicsScene::rotate(double x)
{ if(this->selectedItems().count()>0){
        QGraphicsItem *item;
        foreach(item,this->selectedItems()){
            item->setRotation(item->rotation()+x);
        }
        doItemChanged();
    }
}

//缩放
void GraphicsScene::scale(double x)
{
    if(this->selectedItems().count()>0){
        QGraphicsItem *item;
        foreach(item,this->selectedItems()){
            item->setScale(item->scale()*x);
        }
        doItemChanged();
    }
}

//选择变化后，发出相应信号的槽函数
void GraphicsScene::doSelectionChanged()
{
    emit selectionChanged(itemlist);
}

//数据变化后，发出相应信号的槽函数
void GraphicsScene::doItemChanged()
{
    QList<Conductor> list=this->datalist();
    emit dataChanged(list);
    emit selectionChanged(itemlist);
}

//接受主视图表格选择变化，改变场景选择状态
void GraphicsScene::doTableSelectionChanged(QItemSelection s1, QItemSelection s2)
{
    disconnect(this,SIGNAL(selectionChanged()),this,SLOT(doSelectionChanged()));   //保证性能，暂时停止连接
    QList<QModelIndex> list1=s1.indexes();
    QModelIndex index;
    foreach(index,list1){
        itemlist.at(index.row())->setSelected(true);
    }
    QList<QModelIndex> list2=s2.indexes();
    foreach(index,list2){
        itemlist.at(index.row())->setSelected(false);
    }
    connect(this,SIGNAL(selectionChanged()),this,SLOT(doSelectionChanged()));   //自身通知自身发射选择变化信号
}

void GraphicsScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    QPointF mousePos(event->buttonDownScenePos(Qt::LeftButton).x(),
                     event->buttonDownScenePos(Qt::LeftButton).y());
    QGraphicsItem *movingItem = itemAt(mousePos.x(), mousePos.y());

    if (movingItem != 0 && event->button() == Qt::LeftButton) {
        movingItemList=this->selectedItems();
        oldPosList.clear();
        for(int i=0;i<movingItemList.count();i++){
            oldPosList.append(movingItemList.at(i)->pos());
        }

//        oldPos = movingItem->pos();
    }
    QGraphicsScene::mousePressEvent(event);
}

void GraphicsScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
   // QPointF mousePos(event->scenePos());
    QGraphicsItem *movingItem = itemAt(event->scenePos());

    if (movingItem != 0 && event->button() == Qt::LeftButton && oldPosList.count() != 0) {
        QList<QPointF> newPosList;
        for(int i=0;i<this->selectedItems().count();i++){
            newPosList.append(this->selectedItems().at(i)->pos());
        }
        if (newPosList != oldPosList){
            emit itemMoved(movingItemList,oldPosList,newPosList);
        }
        movingItemList.clear();
    }
    doItemChanged();
    QGraphicsScene::mouseReleaseEvent(event);
}





