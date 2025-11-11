#include "MainWindow.h"
#include "PurchaseDialog.h"
#include <QVBoxLayout>
#include <QMessageBox>
#include <QRandomGenerator>
#include <QThread>
#include <iostream>
#include <pthread.h>
using namespace std;


struct ThreadData {
    SeatManager *manager;
    int seatId;
    int ownerId;
};


void* pthreadWorker(void* arg) {
    ThreadData* data = static_cast<ThreadData*>(arg);
    SeatManager* manager = data->manager;
    int seatId = data->seatId;
    int ownerId = data->ownerId;

    bool locked = manager->tryLockSeat(seatId, ownerId);
    if (locked) {

        QThread::msleep(500 + QRandomGenerator::global()->bounded(800));
        bool confirmed = manager->confirmPurchase(seatId, ownerId);
        if (confirmed) {
            printf("[Hilo %lu] Asiento %d comprado con exito.\n",
                   (unsigned long)pthread_self(), seatId);
        } else {
            printf("[Hilo %lu] Asiento %d no pudo confirmarse.\n",
                   (unsigned long)pthread_self(), seatId);
        }
    } else {
        printf("[Hilo %lu] Fallo al intentar comprar asiento %d (ocupado o vendido).\n",
               (unsigned long)pthread_self(), seatId);
    }
    delete data;
    return nullptr;
}


MainWindow::MainWindow(int rows, int cols, QWidget *parent)
    : QMainWindow(parent),
    central(new QWidget(this)),
    grid(new QGridLayout()),
    btnBuy(new QPushButton("Comprar", this)),
    btnSimulate(new QPushButton("Simular comprador externo", this)),
    btnPthreadSim(new QPushButton("Simular con pthreads", this)),
    lblSelected(new QLabel("Asiento seleccionado: ninguno", this)),
    m_manager(new SeatManager(rows*cols, this)),
    m_selectedSeat(-1),
    m_uiOwnerId(1)
{
    setCentralWidget(central);
    QVBoxLayout *vlay = new QVBoxLayout(central);

    QWidget *gridWidget = new QWidget(central);
    gridWidget->setLayout(grid);
    vlay->addWidget(gridWidget);

    QHBoxLayout *hl = new QHBoxLayout();
    hl->addWidget(lblSelected);
    hl->addStretch();
    hl->addWidget(btnSimulate);
    hl->addWidget(btnPthreadSim);
    hl->addWidget(btnBuy);
    vlay->addLayout(hl);

    createSeats(rows, cols);

    connect(btnBuy, &QPushButton::clicked, this, &MainWindow::onBuyClicked);
    connect(btnSimulate, &QPushButton::clicked, this, &MainWindow::onSimulateExternalBuyer);
    connect(btnPthreadSim, &QPushButton::clicked, this, &MainWindow::simulatePthreadBuyers);
    connect(m_manager, &SeatManager::seatStateChanged, this, &MainWindow::handleSeatStateChanged);

    setWindowTitle("Simulador de Cine con Exclusion Mutua");
    resize(950, 600);
}


MainWindow::~MainWindow()
{
    for (ReservationWorker* w : m_workers) {
        w->stop();
        w->wait();
        delete w;
    }
}


void MainWindow::createSeats(int rows, int cols)
{
    int id = 1;
    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < cols; ++c) {
            SeatButton *btn = new SeatButton(id, this);
            m_buttons.insert(id, btn);
            grid->addWidget(btn, r, c);
            connect(btn, &SeatButton::clickedSeat, this, &MainWindow::onSeatClicked);
            SeatManager::State s = m_manager->querySeatState(id);
            btn->setState(static_cast<SeatButton::State>(s));
            id++;
        }
    }
}


void MainWindow::simulatePthreadBuyers()
{
    const int numThreads = 20;
    pthread_t threads[numThreads];

    for (int i = 0; i < numThreads; ++i) {
        int seat = 1 + QRandomGenerator::global()->bounded(m_buttons.size());
        ThreadData* data = new ThreadData{ m_manager, seat, 1000 + i };
        pthread_create(&threads[i], nullptr, pthreadWorker, data);
        printf("Creado hilo %lu intentando asiento %d\n", (unsigned long)threads[i], seat);
    }

    for (int i = 0; i < numThreads; ++i) {
        pthread_join(threads[i], nullptr);
    }

    printf(">>> Todos los hilos terminaron.\n");
}


void MainWindow::onSeatClicked(int seatId)
{
    m_selectedSeat = seatId;
    lblSelected->setText(QString("Asiento seleccionado: %1").arg(seatId));
}

void MainWindow::onBuyClicked()
{
    if (m_selectedSeat < 1) {
        QMessageBox::information(this, "Selecciona", "Selecciona primero un asiento.");
        return;
    }

    bool ok = m_manager->tryLockSeat(m_selectedSeat, m_uiOwnerId);
    if (!ok) {
        SeatManager::State st = m_manager->querySeatState(m_selectedSeat);
        if (st == SeatManager::Sold)
            QMessageBox::warning(this, "No disponible", "El asiento ya fue vendido.");
        else
            QMessageBox::information(this, "Ocupado", "Otro usuario está intentando comprar ese asiento.");
        return;
    }

    PurchaseDialog dlg(m_selectedSeat, this);
    connect(&dlg, &PurchaseDialog::acceptedPurchase, [this]() {
        bool conf = m_manager->confirmPurchase(m_selectedSeat, m_uiOwnerId);
        if (conf)
            QMessageBox::information(this, "Compra exitosa", QString("Has comprado el asiento %1").arg(m_selectedSeat));
        else
            QMessageBox::warning(this, "Error", "No se pudo confirmar la compra.");
    });
    connect(&dlg, &PurchaseDialog::canceledPurchase, [this]() {
        m_manager->releaseLock(m_selectedSeat, m_uiOwnerId);
    });

    dlg.exec();
}

void MainWindow::handleSeatStateChanged(int seatId, int newState)
{
    QMetaObject::invokeMethod(this, [this, seatId, newState]() {
        updateButtonFromState(seatId);
    }, Qt::QueuedConnection);
}

void MainWindow::updateButtonFromState(int seatId)
{
    SeatButton *btn = m_buttons.value(seatId, nullptr);
    if (!btn) return;
    SeatManager::State s = m_manager->querySeatState(seatId);
    btn->setState(static_cast<SeatButton::State>(s));
}

void MainWindow::onSimulateExternalBuyer()
{
    if (m_selectedSeat < 1) {
        QMessageBox::information(this, "Selecciona", "Selecciona un asiento para que el comprador externo lo intente.");
        return;
    }

    int ownerId = 1000 + m_workers.size() + 1;
    ReservationWorker *w = new ReservationWorker(m_manager, ownerId, m_selectedSeat, this);
    m_workers.append(w);
    w->start();
}
