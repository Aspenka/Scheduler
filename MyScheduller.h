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
    TaskPair task;
    TaskVector taskVect;        //перечень заданий
    Timer *timer;

public:
    explicit MyScheduller(QObject *parent = 0);  //пустой конструктор
    MyScheduller(TaskPair oneTask, QObject *parent = 0); //конструктор, принимающий один экземпляр задания
    MyScheduller(TaskVector taskVector, QObject *parent = 0); //конструктор, принимающий перечень заданий
    ~MyScheduller();        //деструктор

    void append(TaskPair oneTask);          //добавить одно задание
    void append(TaskVector taskVector);     //добавить перечень заданий

    void remove(TaskPair oneTask);  //удалить из вектора срабатывания все записи о конкретном задании и его cron-выражении
    void remove(int index);

    void clear();   //очистить очередь заданий
signals:
    void callTask(QString taskName);     //сигнал о начале выполнения нового задания
public slots:
    void start();
    void slotReaction(TaskPair task);
};

#endif // MYSCHEDULLER_H
