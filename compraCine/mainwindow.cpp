#include "MainWindow.h"
#include "PurchaseDialog.h"
#include <QVBoxLayout>
#include <QMessageBox>
#include <QRandomGenerator>

MainWindow::MainWindow(int rows, int cols, QWidget *parent)
    : QMainWindow(parent),
    central(new QWidget(this)),
    grid(new QGridLayout()),
    btnBuy(new QPushButton("Comprar", this)),
    btnSimulate(new QPushButton("Simular comprador externo", this)),
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
    hl->addWidget(btnBuy);
    vlay->addLayout(hl);

    createSeats(rows, cols);

    connect(btnBuy, &QPushButton::clicked, this, &MainWindow::onBuyClicked);
    connect(btnSimulate, &QPushButton::clicked, this, &MainWindow::onSimulateExternalBuyer);
    connect(m_manager, &SeatManager::seatStateChanged, this, &MainWindow::handleSeatStateChanged);

    setWindowTitle("Seleccionar asientos - Demo");
    resize(900, 600);
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
            // init state from manager
            SeatManager::State s = m_manager->querySeatState(id);
            btn->setState(static_cast<SeatButton::State>(s));
            id++;
        }
    }
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

    // Intentar bloquear lógicamente el asiento para este UI (m_uiOwnerId)
    bool ok = m_manager->tryLockSeat(m_selectedSeat, m_uiOwnerId);
    if (!ok) {
        // no se pudo bloquear -> otro usuario lo está reservando o ya vendido
        SeatManager::State st = m_manager->querySeatState(m_selectedSeat);
        if (st == SeatManager::Sold)
            QMessageBox::warning(this, "No disponible", "El asiento ya fue vendido.");
        else
            QMessageBox::information(this, "Ocupado", "Otro usuario está intentando comprar ese asiento. Intenta otro.");
        return;
    }

    // Abre diálogo de confirmación
    PurchaseDialog dlg(m_selectedSeat, this);
    connect(&dlg, &PurchaseDialog::acceptedPurchase, [this]() {
        bool conf = m_manager->confirmPurchase(m_selectedSeat, m_uiOwnerId);
        if (conf) {
            QMessageBox::information(this, "Compra exitosa", QString("Has comprado el asiento %1").arg(m_selectedSeat));
        } else {
            QMessageBox::warning(this, "Error", "No se pudo confirmar la compra (estado cambió).");
        }
    });
    connect(&dlg, &PurchaseDialog::canceledPurchase, [this]() {
        // liberar el lock
        m_manager->releaseLock(m_selectedSeat, m_uiOwnerId);
    });

    // Ejecutar diálogo (bloqueante)
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

    // crear un worker con ownerId distinto
    int ownerId = 1000 + m_workers.size() + 1;
    ReservationWorker *w = new ReservationWorker(m_manager, ownerId, m_selectedSeat, this);
    m_workers.append(w);
    connect(w, &QThread::finished, [w]() {
        // se podría notificar algo aquí
    });
    w->start();

    QMessageBox::information(this, "Simulación", "Comprador externo simulado: intentará reservar el asiento seleccionado.");
}
