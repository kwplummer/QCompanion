#ifndef QCOMPANION_H
#define QCOMPANION_H

#include <QDialog>
#include <QSystemTrayIcon>
#include <QMenu>
#include <vector>
#include "component.h"
#include "speaker.h"
namespace Ui {
class QCompanion;
}

class QCompanion : public QDialog
{
    Q_OBJECT
public:
    explicit QCompanion(QWidget *parent = 0);
    ~QCompanion();
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
    QAction *nextFire;
    std::vector<std::pair<Component*,time_t>> plugins;
    QTimer *whenToSpeak;
    QTimer *updateNextFire;
    Speaker speaker;
    Ui::QCompanion *ui;
    time_t nextSpeakTime;
    QAction *toggleNotificationsAction;
    QAction *toggleTTSAction;
};

#endif // QCOMPANION_H
