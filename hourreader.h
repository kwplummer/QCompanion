#ifndef HOURREADER_H
#define HOURREADER_H
#include "component.h"

/*!
 * \brief Reads the time every hour
 */
class HourReader : public Component
{
  Q_OBJECT
  Q_CLASSINFO("D-Bus Interface", "com.coderfrog.qcompanion.hourreader")
private Q_SLOTS:
  void emitSpeak();
  QDateTime nextCheckTime();

public:
  HourReader(QWidget *parent);
  virtual ~HourReader();
  Q_SCRIPTABLE Q_SLOT QString getText();
};

#endif // HOURREADER_H
