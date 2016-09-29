#include "datamodel.h"
#include <QtDebug>
#include <qmath.h>

DataModel::DataModel(QObject *parent) :
    QAbstractTableModel(parent)
{
    connect(this,SIGNAL(doSomeChange()),this,SIGNAL(modelReset()));
}

void DataModel::modelInit(QList<Conductor> list)
{
    dataList=list;
    emit doSomeChange();
}

int DataModel::rowCount(const QModelIndex &parent) const
{
    return dataList.count();
}

int DataModel::columnCount(const QModelIndex &parent) const
{
    return 12;
}

QVariant DataModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid())
        return QVariant();
    if (role != Qt::DisplayRole)
        return QVariant();
    int row=index.row();
    int column=index.column();
    if(row>=dataList.count())
        return QVariant();

    switch(column){
    case 0:
        return row+1;
    case 1:
        return dataList[row].x0;
    case 2:
        return dataList[row].y0;
    case 3:
        return dataList[row].z0;
    case 4:
        return dataList[row].x1;
    case 5:
        return dataList[row].y1;
    case 6:
        return dataList[row].z1;
    case 7:
        return dataList.value(row).length();
    case 8:
        return dataList[row].r;
    case 9:
        return dataList[row].rou;
    case 10:
        return dataList[row].u;
    case 11:
        return dataList[row].remarks;

    default:
        return QVariant();
    }

}

QVariant DataModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(role!=Qt::DisplayRole)
        return QVariant();
    if(orientation == Qt::Vertical)
        return QVariant();
    switch(section){
    case 0:
        return tr("编号");
    case 1:
        return "x0";
    case 2:
        return "y0";
    case 3:
        return "z0";
    case 4:
        return "x1";
    case 5:
        return "y1";
    case 6:
        return "z1";
    case 7:
        return tr("长度(m)");
    case 8:
        return tr("半径(m)");
    case 9:
        return tr("电阻率(Ω·m)");
    case 10:
        return tr("相对磁导率");
    case 11:
        return tr("标注");
    default:
        return QVariant();
    }
}



