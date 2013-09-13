#ifndef QSNAPPER_H
#define QSNAPPER_H
#include "component.h"
#include <QSettings>
#include <QImage>
#include <QAction>
#ifdef Q_OS_LINUX
#include <QtDBus/QDBusInterface>
#include <QtDBus/QDBusReply>
#endif

/*!
 * \brief Provides screenshot logging.
 * \details Takes a picture every minute. If the picture is different from the
 * last one, it saves it with the time taken.
 * This has two enable functions. One which toggles taking pictures, the other
 * toggles speaking.
 * Additionally it defaults to muted, as the user likely doesn't want to hear
 * "snap" every minute.
 * \todo Add signal for "Time changed" which will refresh timer, used for when
 * component wants to speak earlier than normal.
 */
class QSnapper : public Component
{
  Q_OBJECT
  ///\brief The path where the images should be saved.
  QString saveDir;
  QString getNextFileName(bool isDiff);
  bool imagesDiffer(const QImage oldImage, const QImage newImage);
  bool imagesDiffer(const QImage oldImage, const QImage newImage,
                    const QString filename);
  bool screensaverIsActive();
  ///\brief When the next screenshot will occur, if enabled.
  QDateTime nextWakeup;
  /*! \brief Global settings, used to check where images should be saved to,
   * andif they should be saved. */
  QSettings settings;
  /*!
   *\brief the image data of the last screenshot taken.
   *\details Used to check if the screen has changed, to prevent duplicates.
  */
  QImage oldImage;
  ///\brief Indicated whether this component is on and taking pictures.
  bool canSnap;
  /*! \brief If true, tolerates a difference of 1% of the screen size between
   * images. This allows for a blinking cursors/clocks to not signify a "new"
   * image.
   */
  bool lenient;
  /*! \brief If true, a seperate image of the difference between old and new
   * will be saved.
   */
  bool saveDifferenceImage;
  ///\brief The menu option corrisponding to lenient
  QAction *lenientOption;
  /*! \brief A menu option that toggles if a seperate image of the difference
   * should be saved.
   */
  QAction *toggleDiffAction;
#ifdef Q_OS_LINUX
  ///\brief The interface with the screensaver.
  QDBusInterface screensaver;
#endif
private
slots:
  void emitSpeak();
  void changeSaveFolder();
public
slots:
  bool snap();
  void enableSnapping(bool enable);
  void setLenient(bool isLenient);
  void setMuteSettings(bool shouldMute);
  void setDiff(bool enable);

public:
  QSnapper(QWidget *parent);
  virtual ~QSnapper();
  virtual QDateTime nextCheckTime() override;
  QString getText();
  virtual QList<QAction *> getMenuContents() override;
  bool isEnabled();
};

#endif // QSNAPPER_H
