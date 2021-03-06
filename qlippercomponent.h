#ifndef QLIPPERCOMPONENT_H
#define QLIPPERCOMPONENT_H
#include <QSettings>
#include "component.h"
#include "qlipperwidget.h"
///\brief Logs the clipboard contents
class QlipperComponent : public Component
{
  Q_OBJECT
  Q_CLASSINFO("D-Bus Interface", "com.coderfrog.qcompanion.qlipper")
  /*!
   *\brief The system settings, used for storing if logging should be enabled,
   *and where it should be done.
   */
  QSettings settings;
  ///\brief the GUI.
  QlipperWidget *dialog;
public Q_SLOTS:
  Q_SCRIPTABLE void showDialog();
  Q_SCRIPTABLE void setLogEnabled(bool enabled);
  Q_SCRIPTABLE void setFileLocation();
  void speakClipboard(QString text);
  Q_SCRIPTABLE void setMuteSettings(bool shouldMute);

public:
  QlipperComponent(QWidget *widget);
  virtual ~QlipperComponent();
  virtual QList<QAction *> getMenuContents() override;
};

#endif // QLIPPERCOMPONENT_H
