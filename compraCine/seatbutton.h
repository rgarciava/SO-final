#ifndef SEATBUTTON_H
#define SEATBUTTON_H

#include <QPushButton>

class SeatButton : public QPushButton
{
    Q_OBJECT
public:
    explicit SeatButton(int seatId, QWidget *parent = nullptr);
    int seatId() const;

    enum State { Available, Locked, Sold };
    void setState(State s);
    State state() const;

signals:
    void clickedSeat(int seatId);

private slots:
    void handleClicked();

private:
    int m_seatId;
    State m_state;
    void updateAppearance();
};

#endif // SEATBUTTON_H
