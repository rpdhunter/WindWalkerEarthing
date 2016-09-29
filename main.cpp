#include <QtGui/QApplication>
#include "windwalkerearthingmaindlg.h"

#include <QTextCodec>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTextCodec::setCodecForTr(QTextCodec::codecForLocale() );
    QTextCodec::setCodecForCStrings(QTextCodec::codecForLocale());
    WindWalkerEarthingMainDlg w;
    w.show();

    return a.exec();
}
