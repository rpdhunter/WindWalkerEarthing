#include "optimizeddesigndlg.h"
#include "ui_optimizeddesigndlg.h"

#include "Compute/computdialog.h"
#include <QtDebug>
#include <QDir>

#include "armadillo"

using namespace arma;

const double pi = 3.1415926;        //圆周率
const double MINVALUE = 0.000001;   //最小值（用于比较实数和零，如果小于这个数明则认为相等）


OptimizedDesignDlg::OptimizedDesignDlg(ComputDialog *computeDlg, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::OptimizedDesignDlg)
{
    ui->setupUi(this);

    dlg = computeDlg;
//    QDir::setCurrent(dlg->currentProjectDir);   //设置当前工作文件夹
    QDir::setCurrent(dlg->currentDir);   //设置当前工作文件夹
}

OptimizedDesignDlg::~OptimizedDesignDlg()
{
    delete ui;
}

void OptimizedDesignDlg::on_pushButton_clicked()
{

    ui->textEdit->setText(QDir::currentPath());

    maxPointFound();



}

//计算接触电压最大值
//使用了比较相邻点电位值得"原始"方法，
//注意这种方法并不能计算陡峭山脊（马鞍）形状的极值
void OptimizedDesignDlg::maxPointFound()
{
    mat P;
    P.load("围墙内接触电压.mat");


    QList<Point> maxlist;
    for(uint i=1;i<P.n_rows-1;i++){
        for(uint j=1;j<P.n_cols-1;j++){
            if(P(i,j)>=P(i-1,j) && P(i,j)>=P(i+1,j) && P(i,j)>=P(i,j-1) && P(i,j)>=P(i,j+1) //注意这个判断条件，避免了平面点的入选
                    && !(P(i,j)==P(i-1,j) && P(i,j)==P(i+1,j) && P(i,j)==P(i,j-1) && P(i,j)==P(i,j+1)))
            {
                maxlist.append(Point(i,j,P(i,j)));
            }
        }
    }

    //类似冒泡排序的算法，完成重复点清查
    for(int i=0;i<maxlist.count()-1;i++){
        for(int j=i+1;j<maxlist.count();j++){
            if(qAbs(maxlist.at(i).x-maxlist.at(j).x)<=2 && qAbs(maxlist.at(i).y-maxlist.at(j).y)<=2){
                if(maxlist.at(i).z>maxlist.at(j).z){
                    maxlist.swap(i,j);     //交换
                }
                maxlist.removeAt(j);      //有相近的点了，无论如何要移除掉一个
                j--;
            }
        }
    }


    QString text;
    text.append(tr("计算完毕，共有%1个极大值点").arg(maxlist.count()));

    ui->textEdit->setText(text);
}
