#ifndef COMPONENT_H
#define COMPONENT_H
#include <qwidget.h>
#include <QTimer>
#include <QDateTime>

/*!
 * \brief Everything that has something to say derives from this
 * \details This abstract class provides an interface for all parts of the
 * program that speak or notify.
 * Also it allows for it to be muted so that they will be excluded from
 * speaking.
 * It provides a list of menu options, that can be overridden as well as \link
 * nextCheckTime() \endlink which indicates when the component wants to speak
 * next, as well as getText(), which returns the message to speak/show.
 * \todo Add Qlipper component.
 * \todo Add QWikiReader to read random wiki "Did you know"s.
 * \todo Add signal for "Time changed" when muted/unmuted.
 */
class Component : public QWidget
{
  Q_OBJECT
private
slots:
  void setMute(bool mute);

protected:
  ///\brief A timer used by deriving classes for scheduled events.
  QTimer whenToSpeak;
  /*!
   *\brief Indicates if this component will be checked for getText, or if it
   *will be skipped over.
   */
  bool muted;
  /// The action used to toggle muted.
  QAction *muteAction;
signals:
  /// Emitted when the component wants to send text to the Speaker
  void wantsToSpeak(QString);

public:
  Component(QWidget *parent);
  virtual ~Component();
  /*!
  * \note While nextCheckTime() is not const (you can change state), it may be
  * called many times before \link Speaker::speak\endlink is called.
  * You should not do something like increment and return a time by 60 seconds,
  * as you'll never get called.
  * However, the last value returned is always checked before each call of
  * nextCheckTime().
  */
  virtual QDateTime nextCheckTime() = 0;
  virtual QList<QAction *> getMenuContents();
  bool isMuted();
};
#endif // COMPONENT_H
