#include "undocommand.h"
#include <QtDebug>

//删除命令
DelCommand::DelCommand(GraphicsScene *graphicsScene, QUndoCommand *parent)
    : QUndoCommand(parent)
{
    scene=graphicsScene;
    myItems=scene->selectedItems(); //选择的物体
    itemlist=scene->mainitemlist(); //主数据

    setText(QObject::tr("删除"));
}

void DelCommand::undo()
{
    QGraphicsItem *item;
    foreach(item,myItems){
        scene->addItem(item);
    }
    scene->setMainItemList(itemlist);
    scene->doItemChanged();
}

void DelCommand::redo()
{
    scene->deleteSelected();    //删除选择
}


GroupCommand::GroupCommand(GraphicsScene *graphicsScene, QUndoCommand *parent)
    : QUndoCommand(parent)
{
    scene=graphicsScene;    
    myItems=scene->selectedItems();
    group=new QGraphicsItemGroup;
    setText(QObject::tr("组合"));
}

void GroupCommand::undo()
{
//    scene->clearSelection();
//    scene->destroyItemGroup(group);
//    QGraphicsItem *item;
//    foreach(item,myItems){
//        item->setSelected(true);
//    }

    scene->clearSelection();
//    scene->destroyItemGroup(group);

    QGraphicsItem *item;
    foreach(item,myItems){     //可能是形成group的时候经过了格式转换，如不加入以下代码，将产生一些错误
        if(item->type()==6){
            GraphicsLineItem *line=qgraphicsitem_cast<GraphicsLineItem *>(item);
            scene->removeItem(item);
            scene->addItem(line);
            line->setSelected(true);
        }
        if(item->type()==10){
            QGraphicsItemGroup *tempgroup=qgraphicsitem_cast<QGraphicsItemGroup *>(item);
            scene->removeItem(tempgroup);
            scene->addItem(tempgroup);
            tempgroup->setSelected(true);
        }
    }
    scene->removeItem(group);

}

void GroupCommand::redo()
{    
//    group=scene->createItemGroup(myItems);
    QGraphicsItem *item;
    foreach(item,myItems){
        group->addToGroup(item);
    }
    scene->addItem(group);
    group->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemIsMovable
                    |QGraphicsItem::ItemHasNoContents);
    scene->clearSelection();
    group->setSelected(true);
}


CancelGroupCommand::CancelGroupCommand(GraphicsScene *graphicsScene, QUndoCommand *parent)
{
    scene=graphicsScene;
    group=scene->selectedItems().first()->children().first()->group();
    myItems=group->childItems();
    setText(QObject::tr("取消组合"));
}

void CancelGroupCommand::undo()
{
    group=scene->createItemGroup(myItems);
    group->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemIsMovable
                    |QGraphicsItem::ItemHasNoContents);
    group->setSelected(true);
}


void CancelGroupCommand::redo()
{
    scene->destroyItemGroup(group);
}


FixCommand::FixCommand(GraphicsScene *graphicsScene, QUndoCommand *parent)
{
    scene=graphicsScene;
    myItems=scene->selectedItems();
    setText(QObject::tr("固定"));
}

void FixCommand::undo()
{
    QGraphicsItem *item;
    for(int i=0;i<myItems.count();i++){
        item=myItems[i];
        item->setFlags(myFlags[i]);
    }
}


void FixCommand::redo()
{
    QGraphicsItem *item;
    foreach(item,myItems){
        QGraphicsItem::GraphicsItemFlags flags;
        if(item->type()==10){
            flags=QFlags<QGraphicsItem::GraphicsItemFlag>
                    (QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemHasNoContents);
        }else if(item->type()==6){
            flags=QFlags<QGraphicsItem::GraphicsItemFlag>(QGraphicsItem::ItemIsSelectable);
        }
        item->setFlags(flags);
        myFlags.append(flags);
    }
}


UnFixCommand::UnFixCommand(GraphicsScene *graphicsScene, QUndoCommand *parent)
{
    scene=graphicsScene;
    myItems=scene->selectedItems();
    setText(QObject::tr("取消固定"));
}

void UnFixCommand::undo()
{
    QGraphicsItem *item;
    for(int i=0;i<myItems.count();i++){
        item=myItems[i];
        item->setFlags(myFlags[i]);
    }
}

void UnFixCommand::redo()
{
    QGraphicsItem *item;
    foreach(item,myItems){
        myFlags.append(item->flags());
        item->setFlags(item->flags()|QGraphicsItem::ItemIsMovable);
    }
}

MoveCommand::MoveCommand(GraphicsScene *graphicsScene,QList<QGraphicsItem *> ilist, QList<QPointF> posListOld,
                 QList<QPointF> posListNew, QUndoCommand *parent)
    : QUndoCommand(parent)
{
//    myDiagramItem = diagramItem;
//    newPos = diagramItem->pos();
//    myOldPos = oldPos;
    scene=graphicsScene;
    itemList=ilist;
    oldPosList=posListOld;
    newPosList=posListNew;

    setText(QObject::tr("移动"));
}

//bool MoveCommand::mergeWith(const QUndoCommand *command)
//{
//    const MoveCommand *moveCommand = static_cast<const MoveCommand *>(command);
//    QGraphicsItem *item = moveCommand->myDiagramItem;

//    if (myDiagramItem != item)
//    return false;

//    newPos = item->pos();


//    return true;
//}

void MoveCommand::undo()
{
    for(int i=0;i<itemList.count();i++){
        itemList.at(i)->setPos(oldPosList.at(i));
    }
    scene->doItemChanged();
}

void MoveCommand::redo()
{
    for(int i=0;i<itemList.count();i++){
        itemList.at(i)->setPos(newPosList.at(i));
    }
    scene->doItemChanged();
}
