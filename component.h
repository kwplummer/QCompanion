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
 * It provides a list of menu options, that can be overridden, as well as
 * getText(), which returns the message to speak/show.
 * \todo Add QWikiReader to read random wiki "Did you know"s.
 */
class Component : public QWidget
{
  Q_OBJECT
private Q_SLOTS:
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
Q_SIGNALS:
  /// Emitted when the component wants to send text to the Speaker
  void wantsToSpeak(QString);

public:
  Component(QWidget *parent);
  virtual ~Component();
  virtual QList<QAction *> getMenuContents();
  bool isMuted();
};
#endif // COMPONENT_H
