#ifndef WAITERWIDGET_H
#define WAITERWIDGET_H
#include <QWidget>
#include <QDateTime>
#include <QLabel>
#include <QProgressBar>
#include "waitercronoccurance.h"
/*!
 * \brief A class representing an event that is being waited for. It notifies
 * and shows UI elements related to that time.
 */
class WaiterWidget : public QWidget
{
  Q_OBJECT
  void init();
  ///\brief A bit of text saying when the waiter is waiting for
  QLabel *time;
  ///\brief Text saying how long is left until the waiter reaches its time.
  QLabel *timeLeft;
  ///\brief A progress bar representing the amount of time from start until the
  /// timer is reached.
  QProgressBar *progress;
  ///\brief When the waiter is waiting for.
  const QDateTime datetime;
  ///\brief What the waiter is called.
  QString title;
  /*!
   * \brief Used to represent if the Widget should be updated, or if it has
   * already reached its end time.
   */
  bool quit;
  /*!
   * \brief The time the waiter is created
   * \details As the seconds since the UNIX epoch is reather large, both the end
   * date and current time have this subtracted by them
   * This leads to a much more relevant percentage, as otherwise the progress
   * bar would start at 99% and end at 100%.
   * This does, however, lead to every progress bar starting from 0, even if one
   * is due in a day and one is due in ten minutes.
   * However the timer that's due in ten minutes will grow faster than that of
   * the day, so in the end it works out.
   */
  const double initMs;
  ///\brief When the Waiter should repeat.
  WaiterCronOccurance repeat;

public:
  explicit WaiterWidget(QWidget *parent, QDate date, QTime time, QString title,
                        const WaiterCronOccurance &repeat);
  explicit WaiterWidget(QWidget *parent, qint64 msec, QString title,
                        const WaiterCronOccurance &repeat);
  QString toLoggableString();
  qint64 getMsecs() const;
  static QString intToMonth(int month);
signals:
  ///\brief Emitted to tell the dialog to remove this widget
  void removeAt(WaiterWidget *);
  /*!
   *\brief Emitted to tell the dialog to remove this widget, and set its input
   *fields to what were used to create it.
   */
  void replaceAt(WaiterWidget *, QString, QDateTime, const WaiterCronOccurance);
  ///\brief Emitted to tell the dialog to speak this string
  void speakThis(QString);
  /*!
   * \brief Emitted to tell the dialog that the time until the next speech has
   * changed.
   */
  void resetTimer();
  ///\brief Tells the dialog to create a waiter with the same title and new
  ///time, and delete the old one.
  void repeatAt(WaiterWidget *, QString, QDateTime, const WaiterCronOccurance);
public
slots:
  void update(QDateTime t);
  void removeThis();
};

#endif // WAITERWIDGET_H
