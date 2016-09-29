#include "cutcompute.h"
#include "Base/compute.h"
#include "Base/currentsource.h"

CutCompute::CutCompute(QList<Conductor> list, ComputDialog *computeDlg)
    :ParameterComputeBase(list,computeDlg)
{
}

QList<Conductor> CutCompute::cut()
{
    deb(tr("******************************开始一次新的计算******************************"));

    powerProintCut();       //预先进行电源点切割

    Compute *c = new Compute;
    emit(currentName(tr("正在进行交点切割"),tr("1/9")));
    connect(c,SIGNAL(progressValueChanged_cut(int,int)),dlg,SLOT(setValue(int,int)));

    QList<Conductor> cutlist_out;
    cutlist_out = c->cutConductor(this->cutlist);    //交点切割

    emit outputTextAdd( tr("接地导体一次切割分段数：%1 \n").arg(cutlist_out.count()) );

    emit(currentName(tr("正在进行长导体切割"),tr("2/9")));
    cutlist_out = c->secendCut(cutlist_out);        //二次切割

    int thirdCut = dlg->thirdCut();
    cutlist_out = c->thirdCut(cutlist_out,thirdCut);        //三次切割

    emit outputTextAdd( tr("接地导体二次切割分段数：%1 \n").arg(cutlist_out.count()) );

    emit(currentName(tr("正在将切割好的导体保存至文件"),tr("3/9")));
    saveCutlistToFile(cutlist_out);                    //保存到文件

    return cutlist_out;
}

//电源接入点切割（发生在交点切割之前）
//如果没有电源，此函数将不起作用
void CutCompute::powerProintCut()
{
    foreach(CurrentSource cs,dlg->currentSourceList){
        QList<Conductor> list;
        foreach(Conductor c,this->cutlist){
            if(Point::isInline(cs.P,c.P0(),c.P1()) == true){    //说明电源点cs在线段c上，开始切割导体
                Conductor conductor1 = c ,conductor2 = c;
                conductor1.dataNodeInit(c.x0,c.y0,c.z0,cs.P.x,cs.P.y,cs.P.z);
                conductor2.dataNodeInit(cs.P.x,cs.P.y,cs.P.z,c.x1,c.y1,c.z1);
                list.append(conductor1);
                list.append(conductor2);
            }
            else{
                Conductor conductor = c;
                list.append(conductor);
            }
        }
        this->cutlist = list;
    }
}

//保存到文件
void CutCompute::saveCutlistToFile(QList<Conductor> cutlist)
{
    QString fileName = dlg->currentDir + "/" + tr("切割.txt");
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return;
    QTextStream out(&file);
    foreach(Conductor c,cutlist){
        out << c.output() <<endl;
    }
}
