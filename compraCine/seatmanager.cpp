#include "SeatManager.h"

SeatManager::SeatManager(int totalSeats, QObject *parent)
    : QObject(parent), m_totalSeats(totalSeats)
{
}

bool SeatManager::validSeat(int seatId) const
{
    return seatId >= 1 && seatId <= m_totalSeats;
}

bool SeatManager::tryLockSeat(int seatId, int ownerId)
{
    QMutexLocker locker(&m_mutex);
    if (!validSeat(seatId)) return false;
    if (m_soldSeats.contains(seatId)) return false;
    if (m_lockedBy.contains(seatId)) return false;
    m_lockedBy.insert(seatId, ownerId);
    emit seatStateChanged(seatId, Locked);
    return true;
}

bool SeatManager::releaseLock(int seatId, int ownerId)
{
    QMutexLocker locker(&m_mutex);
    if (!validSeat(seatId)) return false;
    if (!m_lockedBy.contains(seatId)) return false;
    if (m_lockedBy.value(seatId) != ownerId) return false;
    m_lockedBy.remove(seatId);
    emit seatStateChanged(seatId, Available);
    return true;
}

bool SeatManager::confirmPurchase(int seatId, int ownerId)
{
    QMutexLocker locker(&m_mutex);
    if (!validSeat(seatId)) return false;
    if (!m_lockedBy.contains(seatId)) return false;
    if (m_lockedBy.value(seatId) != ownerId) return false;
    m_lockedBy.remove(seatId);
    m_soldSeats.insert(seatId);
    emit seatStateChanged(seatId, Sold);
    return true;
}

SeatManager::State SeatManager::querySeatState(int seatId)
{
    QMutexLocker locker(&m_mutex);
    if (!validSeat(seatId)) return Sold; // invalid -> treat as sold (safe)
    if (m_soldSeats.contains(seatId)) return Sold;
    if (m_lockedBy.contains(seatId)) return Locked;
    return Available;
}
