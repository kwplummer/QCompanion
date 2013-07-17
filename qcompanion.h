#ifndef QCOMPANION_H
#define QCOMPANION_H
#include <QDialog>
#include <QSystemTrayIcon>
#include "hourreader.h"
#include "qsnapper.h"
#include "speaker.h"
namespace Ui {
class QCompanion;
}

class QCompanion : public QDialog
{
    Q_OBJECT
public:
    explicit QCompanion(QWidget *parent = 0);
    virtual ~QCompanion();
public slots:
    void quit();
    void speak();
    void showingMenu();
    void hidingMenu();
    void updateNextFireText();
    void speakClipboard();
    void toggleNotifications();
    void toggleTTS();
private:
    QString iconPath;
    QSystemTrayIcon *tray;
    QMenu *loadPlugins();
    QMenu *findPluginMenu(Component *c, QString path);
    QAction *nextFire;
    std::vector<std::pair<Component*,QDateTime>> plugins;
    QTimer *whenToSpeak;
    QTimer *updateNextFire;
    Speaker speaker;
    Ui::QCompanion *ui;
    QDateTime nextSpeakTime;
    QAction *toggleNotificationsAction;
    QAction *toggleTTSAction;
    QSnapper *snapper;
};

#endif // QCOMPANION_H
