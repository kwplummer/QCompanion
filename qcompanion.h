#ifndef QCOMPANION_H
#define QCOMPANION_H

#include <QDialog>

namespace Ui {
class QCompanion;
}

class QCompanion : public QDialog
{
    Q_OBJECT
    
public:
    explicit QCompanion(QWidget *parent = 0);
    ~QCompanion();
    
private:
    Ui::QCompanion *ui;
};

#endif // QCOMPANION_H
