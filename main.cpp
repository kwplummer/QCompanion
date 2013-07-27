/*!
*\mainpage
*\brief A notifier application that alerts you of interesting facts, as well as provides optional logging
*\author    William Plummer
*\date      2013
*\copyright GNU Public License Version 2
*\details
*This application will notify you about upcoming appointments (Using the QWaiter part), hour changes, as well as providing the ability to log your screen or clipboard, at the user's request.
*/

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
