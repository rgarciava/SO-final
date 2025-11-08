#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGridLayout>
#include <QLabel>
#include <QMap>
#include "SeatButton.h"
#include "SeatManager.h"
#include "ReservationWorker.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(int rows = 5, int cols = 8, QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onSeatClicked(int seatId);
    void handleSeatStateChanged(int seatId, int newState);
    void onBuyClicked();
    void onSimulateExternalBuyer();

private:
    QWidget *central;
    QGridLayout *grid;
    QPushButton *btnBuy;
    QPushButton *btnSimulate;
    QLabel *lblSelected;
    QMap<int, SeatButton*> m_buttons;
    SeatManager *m_manager;
    int m_selectedSeat;
    int m_uiOwnerId; // id del "usuario UI", por ejemplo 1
    QList<ReservationWorker*> m_workers;

    void createSeats(int rows, int cols);
    void updateButtonFromState(int seatId);
};

#endif // MAINWINDOW_H
