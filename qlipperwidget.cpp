#include "qlipperwidget.h"
#include "ui_qlipperwidget.h"
#include <QMimeData>
#include <QFile>
#include <QTextStream>
#include <QCloseEvent>
#include <map>
/*!
 * \brief Creates the Qlipper widget
 * \param savePath Where to store the clipboard log
 * \param parent The owning widget, used for Qt's memory management.
 */
QlipperWidget::QlipperWidget(QString savePath, QWidget *parent)
    : QDialog(parent), ui(new Ui::QlipperWidget), path(savePath),
      isLogEnabled(false)
{
  ui->setupUi(this);
  clipboard = QApplication::clipboard();

  nodeDate = QDate::currentDate();

  model = new QStandardItemModel(this);

  if(!load())
  {
    model->clear();
    if(yearNode)
      delete yearNode;
    yearNode = new QStandardItem(nodeDate.toString("yyyy"));
    if(monthNode)
      delete monthNode;
    monthNode = new QStandardItem(nodeDate.toString("MM - MMMM"));
    if(dayNode)
      delete dayNode;
    dayNode = new QStandardItem(nodeDate.toString("dd - dddd"));

    monthNode->appendRow(dayNode);
    yearNode->appendRow(monthNode);
    model->invisibleRootItem()->appendRow(yearNode);
  }
  ui->clipboardTree->setModel(model);
  ui->clipboardTree->setHeaderHidden(true);
  ui->clipboardTree->expandAll();
  delayedSave.setSingleShot(true);
#if QT_VERSION < 0x050000
  connect(clipboard, SIGNAL(dataChanged()), this, SLOT(clipboardChanges()));
  connect(ui->removeButton, SIGNAL(clicked()), this,
          SLOT(removeButtonClicked()));
  connect(&delayedSave, SIGNAL(timeout()), this, SLOT(save()));
  connect(ui->clipboardTree, SIGNAL(activated(QModelIndex)), this,
          SLOT(toClipboard(QModelIndex)));
#else
  connect(clipboard, &QClipboard::dataChanged, this,
          &QlipperWidget::clipboardChanges);
  connect(ui->removeButton, &QPushButton::clicked, this,
          &QlipperWidget::removeButtonClicked);
  connect(&delayedSave, QTimer::timeout, this, QlipperWidget::save);
#endif
}

/*!
 * \brief Saves the clipboard history, and deletes the GUI.
 */
QlipperWidget::~QlipperWidget()
{
  save();
  delete ui;
}

/*!
 * \brief Sets if logging should be enabled.
 * \param enabled If logging should be enabled
 */
void QlipperWidget::setLogEnabled(bool enabled)
{
  isLogEnabled = enabled;
}

/*!
 * \brief Sets where logs should be stored.
 * \param newPath Where logs should be stored.
 */
void QlipperWidget::setStatePath(QString newPath)
{
  path = newPath;
  load();
}

/*!
 * \brief Hides the UI, rather than actually closing it.
 * \param event used to tell the program not to close the window.
 */
void QlipperWidget::closeEvent(QCloseEvent *event)
{
  setVisible(false);
  event->ignore();
}

/*!
 * \brief Saves the clipboard history to file.
 * \details Saves the history to a file. It is compressed using qCompress(),
 * which is based on zlib, but may not be compatible with it. The string "/|\"
 * is used as a delimiter, which should help prevent daily usage from including
 * delimiters. `TIME is used to mark a change in time in the file.
 */
void QlipperWidget::save()
{
  if(!path.isEmpty())
  {

    std::map<QString, bool> usedStrings;

    QByteArray compressMe;
    QTextStream stream(&compressMe, QIODevice::WriteOnly);
    stream << nodeDate.toString("yyyy-MM-dd") << '\n';
    QStandardItem *root = model->invisibleRootItem();
    for(int i = 0; i < root->rowCount(); ++i)
    {
      QStandardItem *year = root->child(i);
      stream << "`YEAR:" << year->text() << "/|\\";
      for(int j = 0; j < year->rowCount(); ++j)
      {
        QStandardItem *month = year->child(j);
        stream << "`MONTH:" << month->text() << "/|\\";
        for(int k = 0; k < month->rowCount(); ++k)
        {
          QStandardItem *day = month->child(k);
          stream << "`DAY:" << day->text() << "/|\\";
          for(int l = 0; l < day->rowCount(); ++l)
          {
            QStandardItem *node = day->child(l);
            if(!usedStrings[node->text()])
            {
              stream << node->text() << "/|\\";
              usedStrings[node->text()] = true;
            }
          }
        }
      }
    }
    stream.flush();
    QFile file(path);
    file.open(QIODevice::WriteOnly);
    file.write(qCompress(compressMe, 9));
  }
}

/*!
 * \brief Sets selected text to the current clipboard.
 * \param index What to set the clipboard too.
 */
void QlipperWidget::toClipboard(QModelIndex index)
{
  clipboard->setText(index.data().toString());
}

/*!
 * \brief Loads the clipboard history from file.
 * \details Saves the history to a file. It is compressed using qUncompress(),
 * which is based on zlib, but may not be compatible with it. The string "/|\"
 * is used as a delimiter, which should help prevent daily usage from including
 * delimiters. `TIME is used to mark a change in time in the file.
 */
bool QlipperWidget::load()
{
  if(!path.isEmpty())
  {
    QFile file(path);
    if(file.exists())
    {
      file.open(QIODevice::ReadOnly);
      QByteArray compressed = file.readAll();

      QByteArray uncompressed = qUncompress(compressed);
      QTextStream stream(&uncompressed, QIODevice::ReadOnly);

      QString dateString = stream.readLine();
      nodeDate = QDate::fromString(dateString, "yyyy-MM-dd");

      QStringList strings = stream.readAll().split("/|\\");
      QStandardItem *year = nullptr, *month = nullptr, *day = nullptr;
      yearNode = monthNode = dayNode = nullptr;
      for(QString string : strings)
      {
        if(string.startsWith("`YEAR:"))
        {
          QString yearText = string.mid(6);
          year = new QStandardItem(string.mid(6));
          if(!yearNode)
          {
            yearNode = year;
          }
          model->invisibleRootItem()->appendRow(year);
        }
        else if(string.startsWith("`MONTH:"))
        {
          QString yearText = string.mid(6);
          month = new QStandardItem(string.mid(7));
          if(!monthNode)
          {
            monthNode = month;
          }
          if(!year)
            return false;
          year->appendRow(month);
        }
        else if(string.startsWith("`DAY:"))
        {
          QString yearText = string.mid(5);
          day = new QStandardItem(string.mid(5));
          if(!dayNode)
          {
            dayNode = day;
          }
          if(!month)
            return false;
          month->appendRow(day);
        }
        else if(!string.isEmpty())
        {
          QStringList entries = string.split("\n");
          for(const QString &entry : entries)
          {
            if(!day)
              return false;
            day->appendRow(new QStandardItem(entry));
          }
        }
      }
      return true;
    }
  }
  return false;
}

/*!
 * \brief Called when the clipboard changes. It checks if the day has changed
 * since the last clipboard change, and if it has it creates a new "day",
 * "month", and potentially "year" node, depending on how the date has changed.
 */
void QlipperWidget::clipboardChanges()
{
  if(isLogEnabled && clipboard->mimeData()->hasText())
  {
    if(nodeDate != QDate::currentDate())
    {
      QDate newDate = QDate::currentDate();
      if(newDate.year() != nodeDate.year())
      {
        yearNode = new QStandardItem(newDate.toString("yyyy"));
        monthNode = new QStandardItem(newDate.toString("MM - MMMM"));
        dayNode = new QStandardItem(newDate.toString("dd - dddd"));

        monthNode->appendRow(dayNode);
        yearNode->appendRow(monthNode);
        model->invisibleRootItem()->insertRow(0, yearNode);
      }
      else if(newDate.month() != nodeDate.month())
      {
        monthNode = new QStandardItem(newDate.toString("MM - MMMM"));
        dayNode = new QStandardItem(newDate.toString("dd - dddd"));

        monthNode->appendRow(dayNode);
        yearNode->insertRow(0, monthNode);
      }
      else if(newDate.day() != nodeDate.day())
      {
        dayNode = new QStandardItem(newDate.toString("dd - dddd"));

        monthNode->insertRow(0, dayNode);
      }
      nodeDate = newDate;
    }
    dayNode->insertRow(0, new QStandardItem(clipboard->text()));
    emit speakThis(clipboard->text());
    delayedSave.start(60000);
  }
}

/*!
 * \brief Removes the selected item from the list of items in the clipboard, as
 * well as the history.
 */
void QlipperWidget::removeButtonClicked()
{
  QModelIndexList list = ui->clipboardTree->selectionModel()->selectedIndexes();
  for(QModelIndex i : list)
  {
    if(!model->hasChildren(i))
    {
      model->removeRow(i.row(), i.parent());
    }
  }
  ui->clipboardTree->clearSelection();
  delayedSave.start(60000);
}
