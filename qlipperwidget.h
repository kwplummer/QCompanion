#ifndef QLIPPERWIDGET_H
#define QLIPPERWIDGET_H

#include <QDialog>
#include <QClipboard>
#include <QStandardItemModel>
#include <QDateTime>
#include <QAction>
#include <QTimer>
namespace Ui
{
class QlipperWidget;
}

/*!
 * \brief The GUI used for showing and storing the clipboard history.
 * \todo Add ability to search clipboard history, present user with modal
 * result.
 * In modal ask "continue" and "Stop", if continue, keep looking. Else return.
 */
class QlipperWidget : public QDialog
{
  Q_OBJECT

public:
  explicit QlipperWidget(QString savePath, QWidget *parent = 0);
  ~QlipperWidget();
public
slots:
  void setLogEnabled(bool enabled);
  void setStatePath(QString newPath);
  void save();
  void toClipboard(QModelIndex index);
signals:
  ///\brief Send the text up to the speaker.
  void speakThis(QString text);

private:
  ///\brief A pointer to the UI widgets.
  Ui::QlipperWidget *ui;
  ///\brief The system's clipboard.
  QClipboard *clipboard;
  ///\brief The date (not time) that the clipboard was last changed.
  QDate nodeDate;
  ///\brief A tree storing all the clipboard entries.
  QStandardItemModel *model;
  ///\brief A node storing this year's entries
  QStandardItem *yearNode;
  ///\brief A node storing this month's entries
  QStandardItem *monthNode;
  ///\brief A node storing this day's entries
  QStandardItem *dayNode;
  ///\brief Where the compressed clipboard history will be stored.
  QString path;
  ///\brief If the clipboard changes should be logged.
  bool isLogEnabled;
  /*!
   * \brief A timer used to delay commiting to file, so multiple
   * successivecopy+paste operations won't constantly compress and save the
   * entire history.
   */
  QTimer delayedSave;
  bool load();

protected:
  void closeEvent(QCloseEvent *event) override;
private
slots:
  void clipboardChanges();
  void removeButtonClicked();
  void on_searchButton_clicked();
};

#endif // QLIPPERWIDGET_H
