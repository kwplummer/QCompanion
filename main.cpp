#ifndef TEST
#include "qcompanion.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QCoreApplication::setOrganizationDomain("coderfrog.com");
    QCoreApplication::setOrganizationName("Coderfrog");
    QCoreApplication::setApplicationName("QCompanion");
    QCompanion w;
    w.show();
    w.hide();
    return a.exec();
}
#endif
