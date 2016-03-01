#ifndef MYSCHEDULLER_H
#define MYSCHEDULLER_H

#include <QObject>
#include <QVector>
#include <QPair>
#include <QDateTime>
#include <QStringList>
#include <QDebug>
#include "Timer.h"
#include "CronParser.h"

#define TaskPair        QPair <QString, QString>    //один экземпляр задания, имеющего формат {"cron-выражение", "имя задания"}
#define TaskVector      QVector<QPair <QString, QString>>           //вектор заданий формата TaskPair

//класс предназначен для вызова определенных процедур в указанное время, представленное в виде cron-выражения
class MyScheduller : public QObject
{
    Q_OBJECT
private:
    TaskPair task;              //одно задание
    TaskVector taskVect;        //перечень заданий
    Timer *timer;               //таймер

public:
    explicit MyScheduller(QObject *parent = 0);  //пустой конструктор
    MyScheduller(TaskPair oneTask, QObject *parent = 0); //конструктор, принимающий один экземпляр задания
    MyScheduller(TaskVector taskVector, QObject *parent = 0); //конструктор, принимающий перечень заданий
    ~MyScheduller();        //деструктор

    void append(TaskPair oneTask);          //добавить одно задание
    void append(TaskVector taskVector);     //добавить перечень заданий

    void remove(TaskPair oneTask);  //метод удаляет из вектора заданий задание, равное oneTask
    void remove(int index); //метод удаляет из вектора заданий значение с индексом index

    void clear();   //очистить очередь заданий
signals:
    void callTask(QString taskName);     //сигнал о начале выполнения нового задания
public slots:
    void start();   //метод-слот запускает планировщик
    void slotReaction(TaskPair task);   //метод-слот, реагируюшщий на срабатывание таймера
};

#endif // MYSCHEDULLER_H
