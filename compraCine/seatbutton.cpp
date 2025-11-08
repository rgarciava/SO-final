#include "seatbutton.h"
#include <QString>

SeatButton::SeatButton(int seatId, QWidget *parent)
    : QPushButton(parent), m_seatId(seatId), m_state(Available)
{
    setText(QString::number(seatId));
    connect(this, &QPushButton::clicked, this, &SeatButton::handleClicked);
    updateAppearance();
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
}

int SeatButton::seatId() const { return m_seatId; }

void SeatButton::handleClicked()
{
    emit clickedSeat(m_seatId);
}

void SeatButton::setState(State s)
{
    m_state = s;
    updateAppearance();
}

SeatButton::State SeatButton::state() const { return m_state; }

void SeatButton::updateAppearance()
{
    switch (m_state) {
    case Available:
        setEnabled(true);
        setStyleSheet("background: #dff0d8;"); // claro para disponible
        setText(QString::number(m_seatId));
        break;
    case Locked:
        setEnabled(true);
        setStyleSheet("background: #fcf8e3;"); // amarillo para bloqueado
        setText(QString("%1\n(BLOQ)").arg(m_seatId));
        break;
    case Sold:
        setEnabled(false);
        setStyleSheet("background: #f2dede;"); // rojo para vendido
        setText(QString("%1\n(VEND)").arg(m_seatId));
        break;
    }
}
