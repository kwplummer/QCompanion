#include <QDir>
#include <QTimer>
#include <libnotify/notify.h>
#include "qcompanion.h"
#include "ui_qcompanion.h"
QCompanion::QCompanion(QWidget *parent) :
    QDialog(parent),
    nextFire(nullptr),
    ui(new Ui::QCompanion)
{
    notify_init("QCompanion");
    ui->setupUi(this);
    whenToSpeak = new QTimer(this);
    updateNextFire = new QTimer(this);
    tray = new QSystemTrayIcon(QIcon(":icons/murasaki.png"),this);
    setWindowIcon(tray->icon());
    tray->show();
    QMenu *mainMenu = new QMenu(this);

    QMenu *pluginsMenu = loadPlugins();
    mainMenu->addMenu(pluginsMenu);
    if(!plugins.empty())
    {
        nextFire = new QAction(this);
        updateNextFireText();
        mainMenu->addAction(nextFire);
        NotifyNotification *hello = notify_notification_new ("Hello There!", nextFire->text().toUtf8(), "dialog-information");
        notify_notification_show(hello,NULL);
    }

    QAction *quitAction = new QAction("Quit",this);
    mainMenu->addAction(quitAction);

    tray->setContextMenu(mainMenu);


#if QT_VERSION < 0x050000
    connect(quitAction,SIGNAL(triggered()),this,SLOT(quit()));
    connect(whenToSpeak,SIGNAL(timeout()),this,SLOT(speak()));
    connect(mainMenu,SIGNAL(aboutToShow()),this,SLOT(showingMenu()));
    connect(mainMenu,SIGNAL(aboutToHide()),this,SLOT(hidingMenu()));
    connect(updateNextFire,SIGNAL(timeout()),this,SLOT(updateNextFireText()));
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
                NotifyNotification *notification = notify_notification_new("QCompanion",text.c_str(),"dialog-information");
                notify_notification_show(notification,NULL);
            }
            time_t cTime = p.second = p.first->nextCheckTime();
            if(!nextSpeakTime || (cTime < nextSpeakTime && cTime > time(NULL)))
                nextSpeakTime = cTime;
        }
    }
    catch(std::exception e)
    {
        NotifyNotification *warn = notify_notification_new ("QCompanion Error", e.what(), "dialog-error");
        notify_notification_show(warn,NULL);
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
            NotifyNotification *warn = notify_notification_new ("QCompanion Error",
                                                                ("Unable to load plugin " + list.at(i).fileName() + '\n'
                                                                 + QString(e.what())).toUtf8(), "dialog-error");
            notify_notification_show(warn,NULL);
        }
    }
    if(pluginMenu->isEmpty())
    {
        NotifyNotification *warn = notify_notification_new ("QCompanion Error", "Unable to load any plugins.","dialog-error");
        notify_notification_show(warn,NULL);
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
