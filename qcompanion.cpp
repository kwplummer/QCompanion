#include <QDir>
#include <QTimer>
#include <QClipboard>
#include <QMenu>
#include "qcompanion.h"
#include "ui_qcompanion.h"
#include "waitercomponent.h"
#include "hourreader.h"
#include "qlippercomponent.h"

/*!
 * \brief Constructs the UI and sets up timers
 * \details Creates the tray, the icons, loads Components, and sets up all the
 * connections.
 * \param parent Used for Qt's memory management.
 */
QCompanion::QCompanion(QWidget *parent)
    : QDialog(parent),
      speaker(QCoreApplication::applicationDirPath() + "/murasaki.png"),
      ui(new Ui::QCompanion)
{
  ui->setupUi(this);
  iconPath = QCoreApplication::applicationDirPath() + "/murasaki.png";
  tray = new QSystemTrayIcon(QIcon(iconPath), this);
  setWindowIcon(tray->icon());
  tray->show();
  QMenu *mainMenu = new QMenu(this);

  QMenu *pluginsMenu = loadPlugins();
  mainMenu->addMenu(pluginsMenu);

  QAction *speakClipboardAction = new QAction("Speak Clipboard", this);
  mainMenu->addAction(speakClipboardAction);

  toggleNotificationsAction = new QAction("Disable Notifications", this);
  mainMenu->addAction(toggleNotificationsAction);

  toggleTTSAction = new QAction("Disable Text To Speech", this);
  mainMenu->addAction(toggleTTSAction);

  QAction *quitAction = new QAction("Quit", this);
  mainMenu->addAction(quitAction);

  tray->setContextMenu(mainMenu);

#if QT_VERSION < 0x050000
  connect(quitAction, SIGNAL(triggered()), this, SLOT(quit()));
  connect(toggleTTSAction, SIGNAL(triggered()), this, SLOT(toggleTTS()));
  connect(toggleNotificationsAction, SIGNAL(triggered()), this,
          SLOT(toggleNotifications()));
  connect(speakClipboardAction, SIGNAL(triggered()), this,
          SLOT(speakClipboard()));
  connect(mainMenu, SIGNAL(aboutToShow()), this, SLOT(showingMenu()));
  connect(mainMenu, SIGNAL(aboutToHide()), this, SLOT(hidingMenu()));
#else
  connect(quitAction, &QAction::triggered, this, &QCompanion::quit);
  connect(toggleTTSAction, &QAction::triggered, this, &QCompanion::toggleTTS);
  connect(toggleNotificationsAction, &QAction::triggered, this,
          &QCompanion::toggleNotifications);
  connect(speakClipboardAction, &QAction::triggered, this,
          &QCompanion::speakClipboard);
  connect(updateNextFire, &QTimer::timeout, this,
          &QCompanion::updateNextFireText);
  connect(mainMenu, &QMenu::aboutToShow, this, &QCompanion::showingMenu);
  connect(mainMenu, &QMenu::aboutToHide, this, &QCompanion::hidingMenu);
#endif
}

/*!
 * \brief Destroys the QCompanion and all its members.
 */
QCompanion::~QCompanion()
{
  delete ui;
}

/*!
 * \brief Exits the application, Must be implemented as hiding the application
 * could cause it to quit.
 */
void QCompanion::quit()
{
  QApplication::exit(0);
}

/*!
 * \brief Loads all the components.
 * \details Loads all the plugins and adds them to the main menu.
 * Additionally it creates the timers for the screenshot logger and components.
 * \return The main menu to be added to the tray.
 */
QMenu *QCompanion::loadPlugins()
{
  QMenu *pluginMenu = new QMenu("Plugins", this);

  snapper = new QSnapper(this);
  QMenu *snapperMenu = new QMenu("QSnapper", this);
  snapperMenu->addActions(snapper->getMenuContents());
  plugins.push_back(snapper);
  pluginMenu->addMenu(snapperMenu);
#if QT_VERSION < 0x050000
  connect(snapper, SIGNAL(wantsToSpeak(QString)), this,
          SLOT(sendToSpeaker(QString)));
#else
  connect(snapper, &QSnapper::wantsToSpeak, this, &QCompanion::sendToSpeaker);
#endif

  HourReader *hr = new HourReader(this);
  QMenu *hourMenu = new QMenu("HourReader", this);
  hourMenu->addActions(hr->getMenuContents());
  plugins.push_back(hr);
  pluginMenu->addMenu(hourMenu);
#if QT_VERSION < 0x050000
  connect(hr, SIGNAL(wantsToSpeak(QString)), this,
          SLOT(sendToSpeaker(QString)));
#else
  connect(hr, &HourReader::wantsToSpeak, this, &QCompanion::sendToSpeaker);
#endif

  WaiterComponent *waiter = new WaiterComponent(this);
  QMenu *waiterMenu = new QMenu("QWaiter", this);
  waiterMenu->addActions(waiter->getMenuContents());
  plugins.push_back(waiter);
  pluginMenu->addMenu(waiterMenu);
#if QT_VERSION < 0x050000
  connect(waiter, SIGNAL(wantsToSpeak(QString)), this,
          SLOT(sendToSpeaker(QString)));
#else
  connect(waiter, &WaiterComponent::wantsToSpeak, this,
          &QCompanion::sendToSpeaker);
#endif

  QlipperComponent *qlipper = new QlipperComponent(this);
  QMenu *qlipperMenu = new QMenu("Qlipper", this);
  qlipperMenu->addActions(qlipper->getMenuContents());
  plugins.push_back(qlipper);
  pluginMenu->addMenu(qlipperMenu);
#if QT_VERSION < 0x050000
  connect(qlipper, SIGNAL(wantsToSpeak(QString)), this,
          SLOT(sendToSpeaker(QString)));
#else
  connect(qlipper, &QlipperComponent::wantsToSpeak, this,
          &QCompanion::sendToSpeaker);
#endif
  return pluginMenu;
}

/*!
 * \brief Feeds the clipboard text into the Speaker.
 */
void QCompanion::speakClipboard()
{
  QClipboard *board = QApplication::clipboard();
  QString text = board->text();
  speaker.speak(text.toUtf8());
}

/*!
 * \brief Toggles if the speaker should show notifications.
 */
void QCompanion::toggleNotifications()
{
  if(speaker.isNotificationsEnabled())
  {
    speaker.setNotificationsEnabled(false);
    toggleNotificationsAction->setText("Enable Notifications");
  }
  else
  {
    speaker.setNotificationsEnabled(true);
    toggleNotificationsAction->setText("Disable Notifications");
  }
}

/*!
 * \brief Toggles if the speaker should show read strings aloud.
 */
void QCompanion::toggleTTS()
{
  if(speaker.isTTSEnabled())
  {
    speaker.setTTSEnabled(false);
    toggleTTSAction->setText("Disable Text To Speech");
  }
  else
  {
    speaker.setTTSEnabled(true);
    toggleTTSAction->setText("Enable Text To Speech");
  }
}

/*!
 * \brief Sends text to the speaker
 * \param sayMe What the speaker should say/notify.
 */
void QCompanion::sendToSpeaker(QString sayMe)
{
  if(!sayMe.isEmpty())
  {
    for(QString s : sayMe.split("\n"))
      speaker.speak(s);
  }
}
