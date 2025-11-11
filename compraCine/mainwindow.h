#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMap>
#include <QGridLayout>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include "SeatButton.h"
#include "SeatManager.h"
#include "ReservationWorker.h"
#include <pthread.h>

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
    void simulatePthreadBuyers();

private:
    QWidget *central;
    QGridLayout *grid;
    QPushButton *btnBuy;
    QPushButton *btnSimulate;
    QPushButton *btnPthreadSim;
    QLabel *lblSelected;
    QMap<int, SeatButton*> m_buttons;
    SeatManager *m_manager;
    int m_selectedSeat;
    int m_uiOwnerId;
    QList<ReservationWorker*> m_workers;

    void createSeats(int rows, int cols);
    void updateButtonFromState(int seatId);
};

#endif // MAINWINDOW_H
