#include "PurchaseDialog.h"
#include "ui_PurchaseDialog.h"
#include <QString>

PurchaseDialog::PurchaseDialog(int seatId, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PurchaseDialog),
    m_seatId(seatId)
{
    ui->setupUi(this);
    setWindowTitle(QString("Confirmar compra - Asiento %1").arg(seatId));
    ui->labelInfo->setText(QString("¿Confirmar la compra del asiento %1 ?").arg(seatId));
}

PurchaseDialog::~PurchaseDialog()
{
    delete ui;
}

void PurchaseDialog::on_confirmButton_clicked()
{
    emit acceptedPurchase();
    accept();
}

void PurchaseDialog::on_cancelButton_clicked()
{
    emit canceledPurchase();
    reject();
}
