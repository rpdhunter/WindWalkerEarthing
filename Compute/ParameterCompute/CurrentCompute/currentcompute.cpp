#include "currentcompute.h"
#include "equal.h"
#include "unequal_r.h"
#include "unequal_z_v1.h"
#include "unequal_z_v2.h"


CurrentCompute::CurrentCompute(QList<Conductor> list, ComputDialog *computeDlg) :
    ParameterComputeBase(list,computeDlg)
{
    QDir::setCurrent(dlg->currentDir);
}

void CurrentCompute::run()
{
    if(model() == equal){
        Equal *c = new Equal(cutlist,dlg);
        c->run();
        delete c;
    }
    else if(model() == unequal_r){
        UnEqual_R *c = new UnEqual_R(cutlist,dlg);
        c->run();
        delete c;
    }
    else if(model() == unequal_z_v1){
        UnEqual_Z_V1 *c = new UnEqual_Z_V1(cutlist,dlg);
        c->run();
        delete c;
    }
    else if(model() == unequal_z_v2){
        UnEqual_Z_V2 *c = new UnEqual_Z_V2(cutlist,dlg);
        c->run();
        delete c;
    }
}
