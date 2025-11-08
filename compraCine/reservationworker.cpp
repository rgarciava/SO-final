#include "ReservationWorker.h"
#include <QRandomGenerator>
#include <QThread>

ReservationWorker::ReservationWorker(SeatManager* manager, int ownerId, int seatToTry, QObject* parent)
    : QThread(parent), m_manager(manager), m_ownerId(ownerId), m_seatToTry(seatToTry), m_running(1)
{
}

void ReservationWorker::stop()
{
    m_running.storeRelease(0);
}

void ReservationWorker::run()
{
    // Intentar bloquear repetidamente durante unos segundos
    int attempts = 0;
    while (m_running.loadAcquire() && attempts < 6) {
        attempts++;
        bool locked = m_manager->tryLockSeat(m_seatToTry, m_ownerId);
        if (locked) {
            // Simula pensar/completar pago entre 1 y 4 segundos
            int ms = 1000 + QRandomGenerator::global()->bounded(3000);
            ms = qMax(1000, ms);
            ms = qMin(5000, ms);
            QThread::msleep(ms);

            // Decide aleatoriamente confirmar o soltar
            bool confirm = (QRandomGenerator::global()->bounded(100) < 70); // 70% confirma
            if (confirm) {
                m_manager->confirmPurchase(m_seatToTry, m_ownerId);
            } else {
                m_manager->releaseLock(m_seatToTry, m_ownerId);
            }
            return;
        } else {
            // no conseguido; espera un poco antes de reintentar
            QThread::msleep(300 + QRandomGenerator::global()->bounded(700));
        }
    }
    // terminó sin conseguir o detenido
}
