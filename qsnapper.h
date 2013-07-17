#ifndef QSNAPPER_H
#define QSNAPPER_H
#include "component.h"
#include <QSettings>
#include <QImage>
class QSnapper : public Component
{
    Q_OBJECT
    QString saveDir;
    QString getNextFileName();
    QDateTime nextWakeup;
    QSettings settings;
    QImage oldImage;
private slots:
    void changeSaveFolder();
public slots:
    void snap();
public:
    QSnapper(QWidget *parent);
    virtual ~QSnapper();
    virtual QDateTime nextCheckTime() override;
    virtual QString getText() override;
    virtual QList<QAction*> getMenuContents() override;
};

#endif // QSNAPPER_H
