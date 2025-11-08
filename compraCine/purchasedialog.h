#ifndef PURCHASEDIALOG_H
#define PURCHASEDIALOG_H

#include <QDialog>

namespace Ui { class PurchaseDialog; }

class PurchaseDialog : public QDialog
{
    Q_OBJECT
public:
    explicit PurchaseDialog(int seatId, QWidget *parent = nullptr);
    ~PurchaseDialog();

private slots:
    void on_confirmButton_clicked();
    void on_cancelButton_clicked();

signals:
    void acceptedPurchase();
    void canceledPurchase();

private:
    Ui::PurchaseDialog *ui;
    int m_seatId;
};

#endif // PURCHASEDIALOG_H
