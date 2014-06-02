/*!
*\mainpage
*\brief A notifier application that alerts you of interesting facts, as well as
*provides optional logging
*\author    William Plummer
*\date      2013
*\copyright GNU Public License Version 2
*\details
*This application notifies you about upcoming appointments, hour changes, as
*well as provides the ability to log your screen or clipboard, if instructed to
*do so.
*/
// clang-format command: clang-format -i *.cpp *.h
#ifndef TEST
#include "qcompanion.h"
#include <QApplication>

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);
  QCoreApplication::setOrganizationDomain("coderfrog.com");
  QCoreApplication::setOrganizationName("Coderfrog");
  QCoreApplication::setApplicationName("QCompanion");
  a.setQuitOnLastWindowClosed(false);
  QCompanion w;
  w.show();
  w.hide();
  return a.exec();
}
#endif
