#include "qsnapper.h"
#include <QAction>
#include <QFileDialog>
#include <QPixmap>
#include <QApplication>
#include <QDesktopWidget>
QSnapper::QSnapper(QWidget *parent) : Component(parent),
    nextWakeup(QDateTime::currentDateTime().addSecs(60))
{
    QVariant saveDirSetting = settings.value("QSnapper_Directory");
    if(!saveDirSetting.isNull())
        saveDir = saveDirSetting.toString();
    muteAction->setChecked(true);
    muted = true;
}

QSnapper::~QSnapper()
{
}

QDateTime QSnapper::nextCheckTime()
{
    return nextWakeup;
}

QString QSnapper::getText()
{
    return "Snap";
}

QList<QAction *> QSnapper::getMenuContents()
{
    QList<QAction*> actions;
    QAction *changeFolderAction = new QAction("Change Save Location",this);
    actions.append(changeFolderAction);
    actions += Component::getMenuContents();
    connect(changeFolderAction,SIGNAL(triggered()),this,SLOT(changeSaveFolder()));
    return actions;
}

void QSnapper::changeSaveFolder()
{
    QFileDialog dialog(this,"Select Folder");
    QString dir = dialog.getExistingDirectory(this,"Open Directory","~/Pictures",QFileDialog::ShowDirsOnly);
    if(!dir.isNull())
    {
        saveDir = dir;
        settings.setValue("QSnapper_Directory",dir);
    }
}

void QSnapper::snap()
{
    if(!saveDir.isNull() && QDir(saveDir).exists())
    {
        QString saveFileName = getNextFileName();
        QPixmap desktop = QPixmap::grabWindow(QApplication::desktop()->winId());
        QImage newImage = desktop.toImage();
        if(oldImage != newImage)
        {
            oldImage = newImage;
            oldImage.save(saveFileName);
            saveFileName = getNextFileName();
        }
        nextWakeup = QDateTime::currentDateTime().addSecs(60);
    }
}

QString QSnapper::getNextFileName()
{
    return saveDir + '/' + QDateTime::currentDateTime().toString("yyyyMMddhhmmss") + ".jpg";
}
