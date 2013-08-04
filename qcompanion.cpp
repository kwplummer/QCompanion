#include <QDir>
#include <QTimer>
#include <QClipboard>
#include <QMenu>
#include "qcompanion.h"
#include "ui_qcompanion.h"
#include "waitercomponent.h"
#include "hourreader.h"

/*!
 * \brief Constructs the UI and sets up timers
 * \details Creates the tray, the icons, loads Components, and sets up all the connections.
 * \param parent Used for Qt's memory management.
 */
QCompanion::QCompanion(QWidget *parent) :
    QDialog(parent),
    nextFire(nullptr),
    speaker(QCoreApplication::applicationDirPath()+"/murasaki.png"),
    ui(new Ui::QCompanion)
{
    ui->setupUi(this);
    updateNextFire = new QTimer(this);
    iconPath = QCoreApplication::applicationDirPath()+"/murasaki.png";
    tray = new QSystemTrayIcon(QIcon(iconPath),this);
    setWindowIcon(tray->icon());
    tray->show();
    QMenu *mainMenu = new QMenu(this);

    QMenu *pluginsMenu = loadPlugins();
    mainMenu->addMenu(pluginsMenu);

    QAction *speakClipboardAction = new QAction("Speak Clipboard",this);
    mainMenu->addAction(speakClipboardAction);
    if(!plugins.empty())
    {
        nextFire = new QAction(this);
        updateNextFireText();
        mainMenu->addAction(nextFire);
        speaker.speak(("Hello There, " + nextFire->text()).toUtf8());
    }

    toggleNotificationsAction = new QAction("Disable Notifications",this);
    mainMenu->addAction(toggleNotificationsAction);

    toggleTTSAction = new QAction("Disable Text To Speech",this);
    mainMenu->addAction(toggleTTSAction);

    QAction *quitAction = new QAction("Quit",this);
    mainMenu->addAction(quitAction);

    tray->setContextMenu(mainMenu);

#if QT_VERSION < 0x050000
    connect(quitAction,SIGNAL(triggered()),this,SLOT(quit()));
    connect(toggleTTSAction,SIGNAL(triggered()),this,SLOT(toggleTTS()));
    connect(toggleNotificationsAction,SIGNAL(triggered()),this,SLOT(toggleNotifications()));
    connect(speakClipboardAction,SIGNAL(triggered()),this,SLOT(speakClipboard()));
    connect(updateNextFire,SIGNAL(timeout()),this,SLOT(updateNextFireText()));
    connect(mainMenu,SIGNAL(aboutToShow()),this,SLOT(showingMenu()));
    connect(mainMenu,SIGNAL(aboutToHide()),this,SLOT(hidingMenu()));
#else
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
 * \brief Exits the application, Must be implemented as hiding the application could cause it to quit.
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
    QMenu *pluginMenu = new QMenu("Plugins",this);

    snapper = new QSnapper(this);
    QMenu *snapperMenu = new QMenu("QSnapper",this);
    snapperMenu->addActions(snapper->getMenuContents());
    plugins.push_back(snapper);
    pluginMenu->addMenu(snapperMenu);
    connect(snapper,SIGNAL(wantsToSpeak(QString)),this,SLOT(sendToSpeaker(QString)));

    HourReader *hr = new HourReader(this);
    QMenu *hourMenu = new QMenu("HourReader",this);
    hourMenu->addActions(hr->getMenuContents());
    plugins.push_back(hr);
    pluginMenu->addMenu(hourMenu);
    connect(hr,SIGNAL(wantsToSpeak(QString)),this,SLOT(sendToSpeaker(QString)));

    WaiterComponent *waiter = new WaiterComponent(this);
    QMenu *waiterMenu = new QMenu("QWaiter",this);
    waiterMenu->addActions(waiter->getMenuContents());
    plugins.push_back(waiter);
    pluginMenu->addMenu(waiterMenu);
    connect(waiter,SIGNAL(changeTimers()),this,SLOT(calcuateNextSpeakTime()));
    connect(waiter,SIGNAL(wantsToSpeak(QString)),this,SLOT(sendToSpeaker(QString)));

    calcuateNextSpeakTime();
    return pluginMenu;
}

/*!
 * \brief Checks each component for when it will want to speak next, and then updates the menu timer to signify when that is.
 */
void QCompanion::calcuateNextSpeakTime()
{
    const QDateTime zero = QDateTime::fromTime_t(0);
    const QDateTime now = QDateTime::currentDateTime();
    nextSpeakTime = zero;
    for(Component *plugin: plugins)
    {
        const QDateTime pluginTime = plugin->nextCheckTime();
        if(!plugin->isMuted() && (nextSpeakTime == zero || (pluginTime < nextSpeakTime && pluginTime > now)))
        {
            nextSpeakTime = pluginTime;
        }
    }
    if(nextSpeakTime == zero)
        nextSpeakTime.setTime_t(time(NULL)+60);
    updateNextFireText();
}

/*!
 * \brief A trigger that starts updating the "Next Speech in X" dialog, as it would waste CPU to constantly update it.
 */
void QCompanion::showingMenu()
{
    updateNextFire->setInterval(1000);
    updateNextFire->start();
}

/*!
 * \brief A trigger that stops updating the "Next Speech in X" dialog, as it would waste CPU to constantly update it.
 */
void QCompanion::hidingMenu()
{
    updateNextFire->stop();
}

/*!
 * \brief Sets the menu item to indicate when the next speech will occur.
 */
void QCompanion::updateNextFireText()
{
    if(nextFire)
        nextFire->setText("The next speech is in: " + QString::number(QDateTime::currentDateTime().secsTo(nextSpeakTime)) + " seconds");
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
 * \param[in] sayMe What the speaker should say/notify.
 */
void QCompanion::sendToSpeaker(QString sayMe)
{
    if(!sayMe.isEmpty())
        speaker.speak(sayMe.toUtf8());
    calcuateNextSpeakTime();
}
