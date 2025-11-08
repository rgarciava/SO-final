#ifndef SEATMANAGER_H
#define SEATMANAGER_H

#include <QObject>
#include <QMutex>
#include <QSet>
#include <QMap>

/*
 SeatManager:
 - tryLockSeat(seatId, ownerId) => true si se consiguió el bloqueo lógico
 - releaseLock(seatId, ownerId)
 - confirmPurchase(seatId, ownerId) => marca vendido
 - querySeatState(seatId)
 Señales para notificar cambios.
*/

class SeatManager : public QObject
{
    Q_OBJECT
public:
    explicit SeatManager(int totalSeats, QObject *parent = nullptr);

    bool tryLockSeat(int seatId, int ownerId);
    bool releaseLock(int seatId, int ownerId);
    bool confirmPurchase(int seatId, int ownerId);

    enum State { Available, Locked, Sold };
    State querySeatState(int seatId);

signals:
    void seatStateChanged(int seatId, int newState); // newState: 0 Available,1 Locked,2 Sold

private:
    QMutex m_mutex; // protege las estructuras
    QSet<int> m_soldSeats;
    QMap<int,int> m_lockedBy; // seatId -> ownerId
    int m_totalSeats;
    bool validSeat(int seatId) const;
};

#endif // SEATMANAGER_H
