#ifndef QSNAPPER_H
#define QSNAPPER_H
#include "component.h"
#include <QSettings>
#include <QImage>

/*!
 * \brief Provides screenshot logging.
 * \details Takes a picture every minute. If the picture is different from the last one, it saves it with the time taken.
 * This has two enable functions. One which toggles taking pictures, the other toggles speaking.
 * Additionally it defaults to muted, as the user likely doesn't want to hear "snap" every minute.
 * \todo Add signal for "Time changed" which will refresh timer, used for when component wants to speak earlier than normal.
 */
class QSnapper : public Component
{
    Q_OBJECT
    ///\brief The path where the images should be saved.
    QString saveDir;
    QString getNextFileName();
    ///\brief When the next screenshot will occur, if enabled.
    QDateTime nextWakeup;
    ///\brief Global settings, used to check where images should be saved to, and if they should be saved.
    QSettings settings;
    /*!
     *\brief the image data of the last screenshot taken.
     *\details Used to check if the screen has changed, to prevent duplicates.
    */
    QImage oldImage;
    ///\brief Indicated whether this component is on and taking pictures.
    bool canSnap;
private slots:
    void emitSpeak();
    void changeSaveFolder();
public slots:
    void snap();
    void enableSnapping(bool enable);
public:
    QSnapper(QWidget *parent);
    virtual ~QSnapper();
    virtual QDateTime nextCheckTime() override;
    QString getText();
    virtual QList<QAction*> getMenuContents() override;
    bool isEnabled();
};

#endif // QSNAPPER_H
