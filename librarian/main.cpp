#include <QtGui/QApplication>
#include "librarian.h"
#include <QTextCodec>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);    
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("utf-8"));
    QSettings sett("Skyrim", "lib");
    sett.setValue("path", a.applicationDirPath());
    librarian w;
    w.show();
    
    return a.exec();
}
