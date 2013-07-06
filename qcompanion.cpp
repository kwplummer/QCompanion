#include <QDir>
#include <QTimer>
#include <QClipboard>
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

    toggleMuteAction = new QAction("Disable Text To Speech",this);
    mainMenu->addAction(toggleMuteAction);

    QAction *quitAction = new QAction("Quit",this);
    mainMenu->addAction(quitAction);

    tray->setContextMenu(mainMenu);

#if QT_VERSION < 0x050000
    connect(quitAction,SIGNAL(triggered()),this,SLOT(quit()));
    connect(toggleMuteAction,SIGNAL(triggered()),this,SLOT(toggleTTS()));
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
    for(std::pair<Component*,time_t> &p: plugins)
        delete p.first;
}

void QCompanion::quit()
{
    exit(0);
}

void QCompanion::speak()
{
    nextSpeakTime = 0;
    try
    {
        for(std::pair<Component*,time_t> &p: plugins)
        {
            if(p.second <= time(NULL))
            {
                std::string text = p.first->getText();
                speaker.speak(text.c_str());
            }
            time_t cTime = p.second = p.first->nextCheckTime();
            if(!nextSpeakTime || (cTime < nextSpeakTime && cTime > time(NULL)))
                nextSpeakTime = cTime;
        }
    }
    catch(std::exception e)
    {
        speaker.speak(e.what());
    }
    if(nextSpeakTime == 0)
    {
        nextSpeakTime = time(NULL) + 60;
    }
    whenToSpeak->setInterval((nextSpeakTime - time(NULL))*1000);
    whenToSpeak->start();
    updateNextFireText();
}

QMenu *QCompanion::loadPlugins()
{
    QDir d("plugins/");
    QFileInfoList list = d.entryInfoList(QStringList("*.so"),QDir::Files);
    QMenu *pluginMenu = new QMenu("Plugins",this);
    nextSpeakTime = 0;
    for(int i=0;i<list.size();++i)
    {
        try
        {
            Component *c = loadComponent(("plugins/" + list.at(i).fileName()).toUtf8());
            time_t cTime = c->nextCheckTime();
            if(!nextSpeakTime || (cTime < nextSpeakTime && cTime > time(NULL)))
                nextSpeakTime = cTime;
            plugins.push_back(std::make_pair(c,c->nextCheckTime()));
            pluginMenu->addAction(list.at(i).fileName());
        }
        catch(ComponentException e)
        {
            speaker.speak(("Unable to load plugin " + list.at(i).fileName() + '\n' + e.what()).toUtf8());
        }
    }
    if(pluginMenu->isEmpty())
    {
        speaker.speak("Unable to load any plugins.");
    }
    else
    {
        if(nextSpeakTime == 0)
        {
            nextSpeakTime = time(NULL) + 60;
        }
        whenToSpeak->setInterval((nextSpeakTime - time(NULL))*1000);
        whenToSpeak->start();
    }
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
        nextFire->setText("The next speech is in: " + QString::number(nextSpeakTime - time(NULL)) + " seconds");
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
        toggleMuteAction->setText("Disable Text To Speech");
    }
    else
    {
        speaker.setTTSEnabled(true);
        toggleMuteAction->setText("Enable Text To Speech");
    }
}
