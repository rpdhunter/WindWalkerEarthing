#ifndef UNDOCOMMAND_H
#define UNDOCOMMAND_H

#include <QUndoCommand>
#include "Graphics/graphicsscene.h"
#include <QGraphicsItem>

//删除
class DelCommand : public QUndoCommand
{
public:
    DelCommand(GraphicsScene *graphicsScene,QUndoCommand *parent = 0);

    void undo();
    void redo();

private:
    GraphicsScene *scene;
    QList<QGraphicsItem *> myItems;
    QList<GraphicsLineItem *> itemlist;

};

//组合
class GroupCommand : public QUndoCommand
{
public:
    GroupCommand(GraphicsScene *graphicsScene,QUndoCommand *parent = 0);

    void undo();
    void redo();

private:
    GraphicsScene *scene;
    QList<QGraphicsItem *> myItems;
    QGraphicsItemGroup *group;

};

//取消组合
class CancelGroupCommand : public QUndoCommand
{
public:
    CancelGroupCommand(GraphicsScene *graphicsScene,QUndoCommand *parent = 0);

    void undo();
    void redo();

private:
    GraphicsScene *scene;
    QList<QGraphicsItem *> myItems;
    QGraphicsItemGroup *group;

};

//固定
class FixCommand : public QUndoCommand
{
public:
    FixCommand(GraphicsScene *graphicsScene,QUndoCommand *parent = 0);

    void undo();
    void redo();

private:
    GraphicsScene *scene;
    QList<QGraphicsItem *> myItems;
    QList<QGraphicsItem::GraphicsItemFlags> myFlags;

};

//取消固定
class UnFixCommand : public QUndoCommand
{
public:
    UnFixCommand(GraphicsScene *graphicsScene,QUndoCommand *parent = 0);

    void undo();
    void redo();

private:
    GraphicsScene *scene;
    QList<QGraphicsItem *> myItems;
    QList<QGraphicsItem::GraphicsItemFlags> myFlags;

};

//添加对象
class AddCommand : public QUndoCommand
{
public:
    AddCommand(GraphicsScene *graphicsScene,QList<Conductor> &datalist,
               QList<Conductor> &newlist, QUndoCommand *parent = 0);

    void undo();
    void redo();

private:
    GraphicsScene *scene;
    QList<QGraphicsItem *> myItems;
    QList<QGraphicsItem::GraphicsItemFlags> myFlags;

};

class MoveCommand : public QUndoCommand
{
public:
    enum { Id = 1234 };

    MoveCommand(GraphicsScene *graphicsScene,QList<QGraphicsItem *> ilist, QList<QPointF> posListOld,QList<QPointF> posListNew,
                QUndoCommand *parent = 0);

    void undo();
    void redo();
//    bool mergeWith(const QUndoCommand *command);
    int id() const { return Id; }

private:
    GraphicsScene *scene;
    QList<QGraphicsItem *> itemList;
    QList<QPointF> oldPosList;
    QList<QPointF> newPosList;

//    QGraphicsItem *myDiagramItem;
//    QPointF myOldPos;
//    QPointF newPos;
};

#endif // UNDOCOMMAND_H
