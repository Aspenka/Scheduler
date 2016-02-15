#ifndef TIMER_H
#define TIMER_H

#include <QTimer>

class Timer : public QTimer
{
    Q_OBJECT
private:
    int index;
public:
    Timer();
    ~Timer();

    void setIndex(int ind);
    int getIndex();
signals:
    sentIndex(int);
public slots:
    void sentTimerIndex();
};

#endif // TIMER_H
