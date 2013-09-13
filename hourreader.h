#ifndef HOURREADER_H
#define HOURREADER_H
#include "component.h"

/*!
 * \brief Reads the time every hour
 */
class HourReader : public Component
{
  Q_OBJECT
private
slots:
  void emitSpeak();

public:
  HourReader(QWidget *parent);
  virtual ~HourReader();
  virtual QDateTime nextCheckTime() override;
  QString getText();
};

#endif // HOURREADER_H
