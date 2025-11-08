#ifndef RESERVATIONWORKER_H
#define RESERVATIONWORKER_H

#include <QThread>
#include <QAtomicInt>
#include "SeatManager.h"

/*
 Simula un comprador externo:
 - intenta bloquear un asiento (seatToTry), si lo consigue espera un tiempo (simulate think/buy)
   y luego confirma o suelta (aleatorio).
 - ownerId permite distinguirlo de la UI (usa int distinto).
*/

class ReservationWorker : public QThread
{
    Q_OBJECT
public:
    ReservationWorker(SeatManager* manager, int ownerId, int seatToTry, QObject* parent = nullptr);
    void run() override;
    void stop();

private:
    SeatManager* m_manager;
    int m_ownerId;
    int m_seatToTry;
    QAtomicInt m_running;
};

#endif // RESERVATIONWORKER_H
