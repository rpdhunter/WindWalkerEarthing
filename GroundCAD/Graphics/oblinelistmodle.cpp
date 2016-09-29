#include "oblinelistmodle.h"
#include "Base/point.h"

ObLineListModle::ObLineListModle(QObject *parent) :
    QAbstractTableModel(parent)
{
    connect(this,SIGNAL(doSomeChange()),this,SIGNAL(modelReset()));
}

int ObLineListModle::rowCount(const QModelIndex &parent) const
{
    return obLineList.count();
}

int ObLineListModle::columnCount(const QModelIndex &parent) const
{
    return 9;
}

void ObLineListModle::modelInit(QList<ObservedLine> list)
{
    this->obLineList = list;
    emit doSomeChange();
}

QVariant ObLineListModle::data(const QModelIndex &index, int role) const
{
    if(!index.isValid())
        return QVariant();
    if (role != Qt::DisplayRole)
        return QVariant();
    int row=index.row();
    int column=index.column();
    if(row>=obLineList.count())
        return QVariant();

    switch(column){
    case 0:
        return this->obLineList.at(row).name();
    case 1:
        return obLineList.at(row).P1().x;
    case 2:
        return obLineList.at(row).P1().y;
    case 3:
        return obLineList.at(row).P1().z;
    case 4:
        return obLineList.at(row).P2().x;
    case 5:
        return obLineList.at(row).P2().y;
    case 6:
        return obLineList.at(row).P2().z;
    case 7:
        return obLineList.at(row).length();
    case 8:
        return obLineList.at(row).subsection();
    default:
        return QVariant();
    }
}

QVariant ObLineListModle::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(role!=Qt::DisplayRole)
        return QVariant();
    if(orientation == Qt::Vertical)
        return QVariant();
    switch(section){
    case 0:
        return tr("名称");
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
        return tr("长度");
    case 8:
        return tr("分段数");
    default:
        return QVariant();
    }
}
