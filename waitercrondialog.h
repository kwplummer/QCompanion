#ifndef WAITERCRONDIALOG_H
#define WAITERCRONDIALOG_H

#include <QDialog>

namespace Ui {
class WaiterCronDialog;
}

class WaiterCronDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit WaiterCronDialog(QWidget *parent = 0);
    ~WaiterCronDialog();
    
private:
    Ui::WaiterCronDialog *ui;
};

#endif // WAITERCRONDIALOG_H
