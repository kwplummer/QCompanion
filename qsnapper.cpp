#include "qsnapper.h"
#include <QFileDialog>
#include <QPixmap>
#include <QApplication>
#include <QDesktopWidget>
#include <tbb/parallel_for.h>
#include <atomic>
#include <mutex>
#include <iostream>
#if QT_VERSION >= 0x050000
#include <QScreen>
#endif

/*!
 * \brief Constructs the snapper, sets image save location.
 * \details Checks the global QSettings for if taking pictures is enabled, and where they should be saved to.
 * If either of these are not in the global QSettings then pictures will not be taken.
 * \param[in] parent The component's parent widget, used for Qt's Memory Management.
 */
QSnapper::QSnapper(QWidget *parent) : Component(parent),
    nextWakeup(QDateTime::currentDateTime().addSecs(60))
    #ifdef Q_OS_LINUX
    ,screensaver("org.freedesktop.ScreenSaver","/ScreenSaver")
    #endif
{
    QVariant logSetting = settings.value("QSnapper_Enable");
    if(logSetting.isNull() || logSetting.toBool() == false)
        canSnap = false;
    else
        canSnap = true;

    QVariant saveDirSetting = settings.value("QSnapper_Directory");
    if(saveDirSetting.isValid())
        saveDir = saveDirSetting.toString();

    QVariant diffSetting = settings.value("QSnapper_Diff");
    toggleDiffAction = new QAction("Save image of differences",this);
    toggleDiffAction->setCheckable(true);
    if(diffSetting.isValid() && diffSetting.toBool())
    {
        saveDifferenceImage = true;
    }
    else
        saveDifferenceImage = false;
    toggleDiffAction->setChecked(saveDifferenceImage);

    QVariant lenientSetting = settings.value("QSnapper_Lenient");
    lenientOption = new QAction("Allow minor differences",this);
    lenientOption->setCheckable(true);
    if(lenientSetting.isValid() && lenientSetting.toBool())
    {
        lenient = true;
    }
    else
        lenient = false;
    lenientOption->setChecked(lenient);
    toggleDiffAction->setEnabled(lenient);


    QVariant mutedSetting = settings.value("QSnapper_Muted");
    if(mutedSetting.isValid() && !mutedSetting.toBool())
    {
        muteAction->setChecked(false);
        muted = false;
    }
    else
    {
        muteAction->setChecked(true);
        muted = true;
    }

    whenToSpeak.setSingleShot(false);
    whenToSpeak.setInterval(60000);
#if QT_VERSION < 0x050000
    connect(&whenToSpeak,SIGNAL(timeout()),this,SLOT(emitSpeak()));
    connect(muteAction,SIGNAL(triggered(bool)),this,SLOT(setMuteSettings(bool)));
    connect(toggleDiffAction,SIGNAL(triggered(bool)),this,SLOT(setDiff(bool)));
#else
    connect(&whenToSpeak,&QTimer::timeout,this,&QSnapper::emitSpeak);
    connect(muteAction,&QAction::triggered,this,&QSnapper::setMuteSettings);
    connect(toggleDiffAction,&QAction::triggered,this,&QSnapper::setDiff);
#endif
    emitSpeak();
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

    actions.append(lenientOption);
    actions.append(toggleDiffAction);

#if QT_VERSION < 0x050000
    connect(changeFolderAction,SIGNAL(triggered()),this,SLOT(changeSaveFolder()));
    connect(enableLogging,SIGNAL(triggered(bool)),this,SLOT(enableSnapping(bool)));
    connect(lenientOption,SIGNAL(triggered(bool)),this,SLOT(setLenient(bool)));
#else
    connect(changeFolderAction,&QAction::triggered,this,&QSnapper::changeSaveFolder);
    connect(enableLogging,&QAction::triggered,this,&QSnapper::enableSnapping);
    connect(lenientOption,&QAction::triggered,this,&QSnapper::setLenient);
#endif
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
 * \brief Sets lenient to true or false, based on the checkbox
 * \param isLenient if it should be lenient or not
 */
void QSnapper::setLenient(bool isLenient)
{
    lenient = isLenient;
    settings.setValue("QSnapper_Lenient",isLenient);
    toggleDiffAction->setEnabled(lenient);
}

/*!
 * \brief Stores the value of the muted checkbox to settings
 * \param shouldMute if it should be muted.
 */
void QSnapper::setMuteSettings(bool shouldMute)
{
    settings.setValue("QSnapper_Muted",shouldMute);
}

/*!
 * \brief Sets if "Difference" images should be generated, and stores it into settings.
 * \param enable If difference files should be made.
 */
void QSnapper::setDiff(bool enable)
{
    settings.setValue("QSnapper_Diff",enable);
    saveDifferenceImage = enable;
}

/*!
 * \brief Compares two images, and if they are different enough(1% of the screen), returns true
 * \details Uses a loop through all the x pixels and the y pixels, and compares them. The "X loop" is done in parallel using TBB.
 * \param oldImage the old image
 * \param newImage the new image, if this returns true, it will be saved.
 * \return True if the images differ.
 */
bool QSnapper::imagesDiffer(const QImage oldImage, const QImage newImage)
{
    bool exceedsDiffenceLimit = true;
    if(oldImage.width() == newImage.width() && oldImage.height() == newImage.height())
    {
        exceedsDiffenceLimit = false;
        std::atomic_int difference(0);
        const int height = oldImage.height();
        const int width = oldImage.width();
        const int differenceLimit = (height*width)/100;
        tbb::parallel_for(tbb::blocked_range<int>(0,width),[&](const tbb::blocked_range<int> &range)
        {
            for(int i=range.begin(); i != range.end(); ++i)
            {
                for(int j=0;j<height;++j)
                {
                    if(exceedsDiffenceLimit) return;
                    if(oldImage.pixel(i,j) != newImage.pixel(i,j))
                        ++difference;
                    if(difference > differenceLimit)
                    {
                        exceedsDiffenceLimit = true;
                    }
                }
            }
        });
        if(!muted)
            std::cout << difference << std::endl;
    }
    else if(!muted)
        std::cout << "Different sizes" << std::endl;
    return exceedsDiffenceLimit;
}

/*!
 * \brief Compares two images, and if they are different enough(1% of the screen), returns true. Also saves a new image of where they differ.
 * \param oldImage the old image
 * \param newImage the new image, if this returns true, it will be saved.
 * \param filename the name of the generated difference file
 * \return True if the images differ.
 */
bool QSnapper::imagesDiffer(const QImage oldImage, const QImage newImage, const QString filename)
{
    bool exceedsDiffenceLimit = true;
    const int height = oldImage.height();
    const int width = oldImage.width();
    QImage diff(width,height,oldImage.format());
    if(width == newImage.width() && height == newImage.height())
    {
        exceedsDiffenceLimit = false;
        std::atomic_int difference(0);
        const int differenceLimit = (height*width)/100;
        std::mutex diffMutex;
        diff.fill(QColor(00,0xF2,0xFF));
        tbb::parallel_for(tbb::blocked_range<int>(0,width),[&](const tbb::blocked_range<int> &range)
        {
            for(int i=range.begin(); i != range.end(); ++i)
            {
                for(int j=0;j<height;++j)
                {
                    if(oldImage.pixel(i,j) != newImage.pixel(i,j))
                    {
                        ++difference;
                        QRgb pixel = newImage.pixel(i,j);
                        if(pixel != qRgb(0xFF,0xFF,0xFF))
                        {
                            std::lock_guard<std::mutex> l(diffMutex);
                            diff.setPixel(i,j,pixel);
                        }
                    }
                    if(difference > differenceLimit)
                    {
                        exceedsDiffenceLimit = true;
                    }
                }
            }
        });
        if(!muted)
            std::cout << difference << std::endl;
    }
    else
    {
        if(!muted)
            std::cout << "Different sizes" << std::endl;
        diff = newImage;
    }
    diff.save(filename);
    return exceedsDiffenceLimit;
}

/*!
 * \brief Returns if the screensaver is running. This is OS-dependent.
 * \return If the screensaver is running.
 */
bool QSnapper::screensaverIsActive()
{
#ifdef Q_OS_LINUX
    QDBusReply<bool> reply =  screensaver.call("GetActive");
    bool dbg1 = reply.isValid(), dbg2 = reply.value();
    return reply.isValid() && reply.value();
#elif Q_OS_WIN
    //Untested!
    bool isActive = false;
    SystemParametersInfo(SPI_GETSCREENSAVERACTIVE, 0, &isActive, 0));
    return isActive;
#endif
}

/*!
 * \brief Takes a picture
 * \details Checks if it is allowed to check pictures, and if the save directory exists, and if so takes a picture.
 * If the picture is different from the last one, it gets a name from getNextFileName() and saves it.
 * It then sets the next time another screen shot should occur.
 * \return If a picture was taken.
 */
bool QSnapper::snap()
{
    if(canSnap && !saveDir.isNull() && QDir(saveDir).exists() && !screensaverIsActive())
    {
        QString saveFileName = getNextFileName(false);
#if QT_VERSION < 0x050000
        QPixmap desktop = QPixmap::grabWindow(QApplication::desktop()->winId());
#else
        QPixmap desktop = QGuiApplication::primaryScreen()->grabWindow(QApplication::desktop()->winId());
#endif
        QImage newImage = desktop.toImage();
        nextWakeup = QDateTime::currentDateTime().addSecs(60);
        if( (!lenient && oldImage != newImage) ||
                (lenient && !saveDifferenceImage && imagesDiffer(oldImage,newImage)) ||
                (lenient && saveDifferenceImage && imagesDiffer(oldImage,newImage,getNextFileName(true))))
        {
            oldImage = newImage;
            oldImage.save(saveFileName);
            saveFileName = getNextFileName(false);
            return true;
        }
    }
    return false;
}

/*!
 * \brief Gets where the image should be saved next.
 * \param[in] isDiff determines if the "Diff" should be added to the path or not.
 * \return A QString comprosed of the path, the current time (yyyyMMddhhmmss) and the extension (.jpg, to save space)
 */
QString QSnapper::getNextFileName(bool isDiff)
{
    return saveDir + (isDiff ? "/Diff/" : "/") + QDateTime::currentDateTime().toString("yyyyMMddhhmmss") + ".jpg";
}

/*!
 * \brief Takes a picture, and if unmuted says "Snap".
 */
void QSnapper::emitSpeak()
{
    if(snap() && !muted)
        emit wantsToSpeak(getText());
    else
        emit wantsToSpeak("");
}
