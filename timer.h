#ifndef TIMER_H
#define TIMER_H

#include <QTimer>
#include <QObject>
#include <QPair>
#include <QString>
#include <QObject>
#include <mutex>
#include "CronParser.h"

#define TaskPair        QPair <QString, QString>

class Timer : public QObject//public QTimer
{
    Q_OBJECT
private:
    CronParser parser;      //обработчик cron-выражений
    QString cronJob;        //cron-выражение
    QString taskName;       //имя таска
    int timerId = 0;        //время срабатывания таймера
    bool singShot = false;  //синглшот
    time_t nextExec = -1;   //время следующего срабатывания
    std::mutex mtxNextExec; //мьтекс для корректного срабатывания таймера

    void timerEvent(QTimerEvent *event) override;    //обработка событий таймера
    time_t calcDiffTime();  //подсчет разницы времени
public:
    explicit Timer(QObject *parent = 0); //конструктор
    ~Timer();   //деструктор

    void setSingleShot(bool singleShot);    //метод станавливает переменную singShot в нужное состояние
signals:
    void timeout(TaskPair);  //сигнал о том, что время таймера истекло
public slots:
    void start(TaskPair pair);     //запуск таймера
    void stop();                   //остановка таймера
};

#endif // TIMER_H
