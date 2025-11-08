#ifndef UI_PURCHASEDIALOG_H
#define UI_PURCHASEDIALOG_H

#include <QtWidgets>

namespace Ui {
class PurchaseDialog
{
public:
    QLabel *labelInfo;
    QPushButton *confirmButton;
    QPushButton *cancelButton;
    QWidget *mainWidget;
    void setupUi(QDialog *PurchaseDialog)
    {
        PurchaseDialog->resize(320,140);
        mainWidget = new QWidget(PurchaseDialog);
        QVBoxLayout *vlay = new QVBoxLayout(PurchaseDialog);
        labelInfo = new QLabel(PurchaseDialog);
        labelInfo->setAlignment(Qt::AlignCenter);
        vlay->addWidget(labelInfo);
        QHBoxLayout *hl = new QHBoxLayout();
        confirmButton = new QPushButton("Confirmar", PurchaseDialog);
        cancelButton = new QPushButton("Cancelar", PurchaseDialog);
        hl->addStretch();
        hl->addWidget(confirmButton);
        hl->addWidget(cancelButton);
        hl->addStretch();
        vlay->addLayout(hl);
        QObject::connect(confirmButton, &QPushButton::clicked, PurchaseDialog, &QDialog::accept);
        QObject::connect(cancelButton, &QPushButton::clicked, PurchaseDialog, &QDialog::reject);
    }
};
} // namespace Ui

#endif // UI_PURCHASEDIALOG_H
