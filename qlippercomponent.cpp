#include "qlippercomponent.h"
#include <QAction>
#include <QFileDialog>

/*!
 * \brief Creates the GUI. Loads if logging is enabled, and where to store it.
 * \param widget The owning widget, used for Qt's memory management.
 */
QlipperComponent::QlipperComponent(QWidget *widget) : Component(widget) {
  QVariant logFile = settings.value("Qlipper_Location", "");
  dialog = new QlipperWidget(logFile.toString(), this);
  QVariant logSetting = settings.value("Qlipper_Enable", false);
  dialog->setLogEnabled(logSetting.toBool());
  QVariant muteSetting = settings.value("Qlipper_Muted", true);
  muteAction->setChecked(muteSetting.toBool());
  muted = muteSetting.toBool();
  dialog->setWindowFlags(Qt::Window);
#if QT_VERSION < 0x050000
  connect(dialog, SIGNAL(speakThis(QString)), this,
          SLOT(speakClipboard(QString)));
  connect(muteAction, SIGNAL(triggered(bool)), this,
          SLOT(setMuteSettings(bool)));
#else
  connect(dialog, &QlipperWidget::speakThis, this,
          &QlipperComponent::speakClipboard);
  connect(muteAction, &QAction::triggered, this,
          &QlipperComponent::setMuteSettings);
#endif
}

///\brief Destroys the GUI
QlipperComponent::~QlipperComponent() { delete dialog; }

QDateTime QlipperComponent::nextCheckTime() { return QDateTime::fromTime_t(0); }

/*!
 * \brief Stores the value of the muted checkbox to settings
 * \param shouldMute if it should be muted.
 */
void QlipperComponent::setMuteSettings(bool shouldMute) {
  settings.setValue("Qlipper_Muted", shouldMute);
}

/*!
 * \brief Creates a specialized menu for the component.
 * \details Creates a menu that can show the GUI as well as automatically read
 * the clipboard upon changes
 * \return A QList of QActions that can be used to interface with the component.
 */
QList<QAction *> QlipperComponent::getMenuContents() {
  QList<QAction *> actions;

  QAction *showAction = new QAction("Show", this);
  connect(showAction, SIGNAL(triggered()), this, SLOT(showDialog()));

  actions.append(showAction);

  QAction *setLocationAction = new QAction("Set save location", this);
  connect(setLocationAction, SIGNAL(triggered()), this,
          SLOT(setFileLocation()));

  actions.append(setLocationAction);

  QAction *enableLogAction = new QAction("Enable Clipboard Logging", this);
  enableLogAction->setCheckable(true);
  QVariant enabledLogging = settings.value("Qlipper_Enable");
  if (enabledLogging.isValid())
    enableLogAction->setChecked(enabledLogging.toBool());
  connect(enableLogAction, SIGNAL(triggered(bool)), this,
          SLOT(setLogEnabled(bool)));

  actions.append(enableLogAction);
  actions.append(Component::getMenuContents());

  return actions;
}

/*!
 * \brief Displays the GUI.
 */
void QlipperComponent::showDialog() { dialog->setVisible(true); }

/*!
 * \brief Sets if the clipboard should be logged, by default is off.
 * \param enabled if the logs should written
 */
void QlipperComponent::setLogEnabled(bool enabled) {
  settings.setValue("Qlipper_Enable", enabled);
  dialog->setLogEnabled(enabled);
}

/*!
 * \brief Sets where to store the log of the clipboard.
 */
void QlipperComponent::setFileLocation() {
  QFileDialog dlg(this, "Select Folder");
  QString path = dlg.getOpenFileName(this, "Select state file", "~/");
  if (!path.isNull()) {
    settings.setValue("Qlipper_Location", path);
    dialog->setStatePath(path);
  }
}

/*!
 * \brief Sends the text from the GUI to the QCompanion
 * \param text what to read.
 */
void QlipperComponent::speakClipboard(QString text) {
  if (!muted)
    emit wantsToSpeak(text);
}
