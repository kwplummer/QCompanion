#ifndef QLIPPERCOMPONENT_H
#define QLIPPERCOMPONENT_H
#include <QSettings>
#include "component.h"
#include "qlipperwidget.h"
///\brief Logs the clipboard contents
class QlipperComponent : public Component {
  Q_OBJECT
  /*!
   *\brief The system settings, used for storing if logging should be enabled,
   *and where it should be done.
   */
  QSettings settings;
  ///\brief the GUI.
  QlipperWidget *dialog;
public
slots:
  void showDialog();
  void setLogEnabled(bool enabled);
  void setFileLocation();
  void speakClipboard(QString text);
  void setMuteSettings(bool shouldMute);

public:
  QlipperComponent(QWidget *widget);
  virtual ~QlipperComponent();
  virtual QDateTime nextCheckTime() override;
  virtual QList<QAction *> getMenuContents() override;
};

#endif // QLIPPERCOMPONENT_H
