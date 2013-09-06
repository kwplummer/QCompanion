#ifndef QCOMPANION_H
#define QCOMPANION_H
#include <QDialog>
#include <QSystemTrayIcon>
#include "speaker.h"
#include "qsnapper.h"

namespace Ui {
class QCompanion;
}

/*!
 * \brief The main class of the program.
 * \details This class organizes all the components, schedules when speaking,
 * notifying, and screenshot-logging will occur.
 */
class QCompanion : public QDialog {
  Q_OBJECT
public:
  explicit QCompanion(QWidget *parent = 0);
  virtual ~QCompanion();
public
slots:
  void quit();
  void showingMenu();
  void hidingMenu();
  void updateNextFireText();
  void speakClipboard();
  void toggleNotifications();
  void toggleTTS();
  void sendToSpeaker(QString sayMe);
  void calcuateNextSpeakTime();

private:
  ///\brief The path to where the icon is stored.
  QString iconPath;
  ///\brief The tray that stores our menu, the only way to interface with the
  /// program.
  QSystemTrayIcon *tray;
  QMenu *loadPlugins();
  ///\brief A menu item stating when the next component wants to be read.
  QAction *nextFire;
  ///\brief A list of plugins, consisting of the component and when it wants to
  /// be read.
  std::vector<Component *> plugins;
  ///\brief A timer for when the "Next Speech is in X seconds" dialog should be
  /// updated
  QTimer *updateNextFire;
  ///\brief The class that manages interfacing with the text to speech and
  /// notification systems.
  Speaker speaker;
  ///\brief A pointer to a class that holds the UI elements.
  Ui::QCompanion *ui;
  ///\brief The earliest time for a component that wants to speak.
  QDateTime nextSpeakTime;
  ///\brief A menu option that toggles notifications for every component.
  QAction *toggleNotificationsAction;
  ///\brief A menu option that toggles speech for every component.
  QAction *toggleTTSAction;
  ///\brief The Screenshot Logger.
  QSnapper *snapper;
};

#endif // QCOMPANION_H
