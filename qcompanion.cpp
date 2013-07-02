#include "qcompanion.h"
#include "ui_qcompanion.h"

QCompanion::QCompanion(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QCompanion)
{
    ui->setupUi(this);
}

QCompanion::~QCompanion()
{
    delete ui;
}
