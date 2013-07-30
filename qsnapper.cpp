#include "qsnapper.h"
#include <QAction>
#include <QFileDialog>
#include <QPixmap>
#include <QApplication>
#include <QDesktopWidget>

/*!
 * \brief Constructs the snapper, sets image save location.
 * \details Checks the global QSettings for if taking pictures is enabled, and where they should be saved to.
 * If either of these are not in the global QSettings then pictures will not be taken.
 * \param[in] parent The component's parent widget, used for Qt's Memory Management.
 */
QSnapper::QSnapper(QWidget *parent) : Component(parent),
    nextWakeup(QDateTime::currentDateTime().addSecs(60))
{
    QVariant logSetting = settings.value("QSnapper_Enable");
    if(logSetting.isNull() || logSetting.toBool() == false)
        canSnap = false;
    else
        canSnap = true;

    QVariant saveDirSetting = settings.value("QSnapper_Directory");
    if(!saveDirSetting.isNull())
        saveDir = saveDirSetting.toString();

    muteAction->setChecked(true);
    muted = true;

    whenToSpeak.setSingleShot(false);
    whenToSpeak.setInterval(60000);
    connect(&whenToSpeak,SIGNAL(timeout()),this,SLOT(emitSpeak()));
    whenToSpeak.start();
}

/*!
 * \brief Destroys the QSnapper, does nothing interesting.
 */
QSnapper::~QSnapper()
{
}

/*!
 * \brief Gets when the next picture will be taken.
 * \details Returns when the next picture will be taken.
 * If taking pictures is disabled, time 0 is returned.
 * \return A QDateTime representing when the component should be read.
 */
QDateTime QSnapper::nextCheckTime()
{
    return canSnap ? nextWakeup : QDateTime::fromTime_t(0);
}

/*!
 * \brief Gets what to say aloud and notify
 * \return The word "Snap"
 */
QString QSnapper::getText()
{
    return "Snap";
}

/*!
 * \brief Gets the items the user will use to interact with the component.
 *In addition to the default mute, QSnapper also provides the ability to change save locations, and toggle taking pictures.
 * \return A list of actions that will be inserted into the main application's menu.
 */
QList<QAction *> QSnapper::getMenuContents()
{
    QList<QAction*> actions;
    QAction *changeFolderAction = new QAction("Change Save Location",this);
    actions.append(changeFolderAction);
    actions += Component::getMenuContents();

    QAction *enableLogging = new QAction("Enabled", this);
    enableLogging->setCheckable(true);
    enableLogging->setChecked(canSnap);
    actions.append(enableLogging);

    connect(changeFolderAction,SIGNAL(triggered()),this,SLOT(changeSaveFolder()));
    connect(enableLogging,SIGNAL(triggered(bool)),this,SLOT(enableSnapping(bool)));
    return actions;
}

/*!
 * \brief Returns if the logger can take pictures or not
 * \return If the logger is allowed to take pictures.
 */
bool QSnapper::isEnabled()
{
    return canSnap;
}

/*!
 * \brief Prompts the user to pick which folder the screenshots should be logged to.
 *This is saved to settings, and persists between runs.
 */
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

/*!
 * \brief Sets if pictures should be taken.
 * \details Sets if pictures should be taken and logs it in the global QSettings
 * \param[in] enable If taking pictures should be enabled.
 */
void QSnapper::enableSnapping(bool enable)
{
    canSnap = enable;
    settings.setValue("QSnapper_Enable",enable);
}

/*!
 * \brief Takes a picture
 * \details Checks if it is allowed to check pictures, and if the save directory exists, and if so takes a picture.
 * If the picture is different from the last one, it gets a name from getNextFileName() and saves it.
 * It then sets the next time another screen shot should occur.
 */
void QSnapper::snap()
{
    if(canSnap && !saveDir.isNull() && QDir(saveDir).exists())
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

/*!
 * \brief Gets where the image should be saved next.
 * \return A QString comprosed of the path, the current time (yyyyMMddhhmmss) and the extension (.jpg, to save space)
 */
QString QSnapper::getNextFileName()
{
    return saveDir + '/' + QDateTime::currentDateTime().toString("yyyyMMddhhmmss") + ".jpg";
}

/*!
 * \brief Takes a picture, and if unmuted says "Snap".
 */
void QSnapper::emitSpeak()
{
    snap();
    if(!muted)
        emit wantsToSpeak(getText());
}
