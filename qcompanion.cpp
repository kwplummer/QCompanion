#include <QDir>
#include <QTimer>
#include <QClipboard>
#include <QMenu>
#include "qcompanion.h"
#include "ui_qcompanion.h"

QCompanion::QCompanion(QWidget *parent) :
    QDialog(parent),
    nextFire(nullptr),
    speaker(QCoreApplication::applicationDirPath()+"/murasaki.png"),
    ui(new Ui::QCompanion)
{
    ui->setupUi(this);
    whenToSpeak = new QTimer(this);
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
    connect(whenToSpeak,SIGNAL(timeout()),this,SLOT(speak()));
    connect(updateNextFire,SIGNAL(timeout()),this,SLOT(updateNextFireText()));
    connect(mainMenu,SIGNAL(aboutToShow()),this,SLOT(showingMenu()));
    connect(mainMenu,SIGNAL(aboutToHide()),this,SLOT(hidingMenu()));
#else
#endif
}

QCompanion::~QCompanion()
{
    delete ui;
}

void QCompanion::quit()
{
    exit(0);
}

void QCompanion::speak()
{
    const QDateTime zero = QDateTime::fromTime_t(0);
    const QDateTime now = QDateTime::currentDateTime();
    nextSpeakTime = zero;
    try
    {
        for(std::pair<Component*,QDateTime> &p: plugins)
        {
            if(p.second <= now && !p.first->isMuted())
                speaker.speak(p.first->getText().toUtf8());
            QDateTime cTime = p.second = p.first->nextCheckTime();
            if(!p.first->isMuted() && (nextSpeakTime == zero || (cTime < nextSpeakTime && cTime > now)))
                nextSpeakTime = cTime;
        }
    }
    catch(std::exception e)
    {
        speaker.speak(e.what());
    }
    if(nextSpeakTime == zero)
    {
        nextSpeakTime.setTime_t(time(NULL)+60);
    }
    whenToSpeak->setInterval(now.msecsTo(nextSpeakTime));
    whenToSpeak->start();
    updateNextFireText();
}

QMenu *QCompanion::loadPlugins()
{
    QMenu *pluginMenu = new QMenu("Plugins",this);

    snapper = new QSnapper(this);
    QMenu *snapperMenu = new QMenu("QSnapper",this);
    snapperMenu->addActions(snapper->getMenuContents());
    plugins.push_back(std::make_pair(snapper,snapper->nextCheckTime()));
    pluginMenu->addMenu(snapperMenu);

    QTimer *snapTimer = new QTimer(this);
    connect(snapTimer,SIGNAL(timeout()),snapper,SLOT(snap()));
    snapTimer->setInterval(1000*60);
    snapTimer->setSingleShot(false);
    snapTimer->start();

    HourReader *hr = new HourReader(this);
    QMenu *hourMenu = new QMenu("HourReader",this);
    hourMenu->addActions(hr->getMenuContents());
    plugins.push_back(std::make_pair(hr,hr->nextCheckTime()));
    pluginMenu->addMenu(hourMenu);


    const QDateTime zero = QDateTime::fromTime_t(0);
    const QDateTime now = QDateTime::currentDateTime();
    nextSpeakTime = zero;
    for(std::pair<Component*,QDateTime> plugin: plugins)
    {
        if(!plugin.first->isMuted() && (nextSpeakTime == zero || (plugin.second < nextSpeakTime && plugin.second > now)))
            nextSpeakTime = plugin.second;
    }
    if(nextSpeakTime == zero)
        nextSpeakTime.setTime_t(time(NULL)+60);
    whenToSpeak->setInterval(now.msecsTo(nextSpeakTime));
    whenToSpeak->start();
    return pluginMenu;
}

void QCompanion::showingMenu()
{
    updateNextFire->setInterval(1000);
    updateNextFire->start();
}

void QCompanion::hidingMenu()
{
    updateNextFire->stop();
}

void QCompanion::updateNextFireText()
{
    if(nextFire)
        nextFire->setText("The next speech is in: " + QString::number(QDateTime::currentDateTime().secsTo(nextSpeakTime)) + " seconds");
}

void QCompanion::speakClipboard()
{
    QClipboard *board = QApplication::clipboard();
    QString text = board->text();
    speaker.speak(text.toUtf8());
}

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
