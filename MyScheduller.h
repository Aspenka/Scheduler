#ifndef MYSCHEDULLER_H
#define MYSCHEDULLER_H

#include <QObject>
//#include <QTimer>
#include <QVector>
#include <QPair>
#include <QDateTime>
#include <QStringList>
#include <QDebug>
#include "Timer.h"
#include "CronParser.h"

#define TaskPair        QPair <QString, QString>    //один экземпляр задания, имеющего формат {"cron-выражение", "имя задания"}
#define TaskVector      QVector<QPair <QString, QString>>           //вектор заданий формата TaskPair
#define TaskTime        QPair<QDateTime, QPair<QString, QString>>   //задание, содержащее в себе время срабатывания и имя задания
#define TaskTimeVector  QVector<QPair<QDateTime, QPair<QString, QString>>>  //вектор заданий, содержащих в себе время срабатывания и имя задания

//класс предназначен для вызова определенных процедур в указанное время, представленное в виде cron-выражения
class MyScheduller : public QObject
{
    Q_OBJECT
private:
    TaskPair task;
    TaskVector taskVect;        //перечень заданий
    TaskTimeVector nextTasks;   //вектор срабатываний, содержащий задание и время, в которое оно должно сработать
    Timer *timer;
    CronParser parser;

    void appendNewTask(TaskPair oneTask);          //метод рассчитывает вектор срабатывания одного задания

    void removeOne(TaskTime task);  //удалить задание
    double calcTimeout(TaskTime oneTask);   //высчитать время срабатывания для одного задания

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
    void timeOut(QString taskName);      //сигнал о начале выполнения нового задания
    void updateTasks();
public slots:
    void slotUpdateTasks();

    void startSheduller();
    void slotReaction(int ind);
    //void start(TaskPair oneTask);
    //void start(TaskVector taskVector);

    //void stop();
};

#endif // MYSCHEDULLER_H
