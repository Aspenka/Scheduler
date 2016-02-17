#ifndef MYSCHEDULLER_H
#define MYSCHEDULLER_H

#include <QObject>
//#include <QTimer>
#include <QVector>
#include <QPair>
#include <QDateTime>
#include <QStringList>
#include <QDebug>
#include "timer.h"

#define TaskPair        QPair <QString, QString>    //один экземпляр задания, имеющего формат {"cron-выражение", "имя задания"}
#define TaskVector      QVector<QPair <QString, QString>>           //вектор заданий формата TaskPair
#define TaskTime        QPair<QDateTime, QPair<QString, QString>>   //задание, содержащее в себе время срабатывания и имя задания
#define TaskTimeVector  QVector<QPair<QDateTime, QPair<QString, QString>>>  //вектор заданий, содержащих в себе время срабатывания и имя задания

//перечисление предельных значений времен
enum timeLimits
{
    MINUTES = 59,       //минуты
    HOURS = 23,         //часы
    DAYS_OF_MONTH = 31, //дни месяца
    MONTHS = 12,        //месяцы
    DAYS_OF_WEEK = 7    //дни недели
};

//перечисление типов крон-выражений
enum valueType
{
    ANY = -1,
    ANY_STEP = -2,
    INTERVAL = -3,
    INTERVAL_STEP = -4,
    ENUMERATION = -5,
    START_STEP = -6,
    VALUE = -7
};

//
struct valueProperties
{
    int type;
    QVector<int> startVal;
    int step=0;
    int finish = 0;
};

//класс предназначен для вызова определенных процедур в указанное время, представленное в виде cron-выражения
class MyScheduller : public QObject
{
    Q_OBJECT
private:
    TaskPair task;
    TaskVector taskVect;        //перечень заданий
    TaskTimeVector nextTasks;   //вектор срабатываний, содержащий задание и время, в которое оно должно сработать
    Timer *timer;

    /*QVector <int>   minute,     //значение минут
                    hour,       //значение часов
                    dayOfMonth, //значение дней месяца
                    month,      //значение месяца
                    dayOfWeek;  //значение дня недели*/
    valueProperties minute,     //значение минут
                    hour,       //значение часов
                    dayOfMonth, //значение дней месяца
                    month,      //значение месяца
                    dayOfWeek;


    void appendNewTask(TaskPair oneTask);          //метод рассчитывает вектор срабатывания одного задания
    void appendNewTask(TaskVector taskVector);     //метод рассчитывает вектор срабатывания для перечня заданий

    valueProperties parseCronJob(QString cronJob, int dateType);    //метод обрабатывает cron-выражение и возвращает
                                                                    //значение срабатывания для каждой единицы времени
    QDateTime calcNewDatetime(QString cronjob);//метод рассчитывает даты срабатывания по cron-выражению
    QDateTime getDate(int dateType, valueProperties dateMean, QDateTime date);

    QDateTime calcForAny(int dateType, QDateTime date);
    QDateTime calcForAny(int dateType, int step, QDateTime date);
    QDateTime calcForInterval(int dateType, int start, int finish, QDateTime date);
    QDateTime calcForInterval(int dateType, int start, int finish, int step, QDateTime date);
    QDateTime calcForEnum(int dateType, QVector<int> values, QDateTime date);
    QDateTime calcForValue(int dateType, int start, int step, QDateTime date);
    QDateTime calcForValue(int dateType, int value, QDateTime date);
   // QVector <QDateTime> getNextDate();   //расчет новой даты срабатывания
    //QVector<QDateTime> calcTimeUnit(QVector <int> time, QVector<QDateTime> nextDate, int limit);    //вычисление нового срабатывания
                                                                                                    //в зависимости от единицы времени
    //QDateTime calcTimeUnit(int dateType, int dateMean, QDateTime date);
    void removeOne(TaskTime task);  //удалить задание
    int calcTimeout(TaskTime oneTask);   //высчитать время срабатывания для одного задания

public:
    explicit MyScheduller(QObject *parent = 0);  //пустой конструктор
    MyScheduller(TaskPair oneTask, QObject *parent = 0); //конструктор, принимающий один экземпляр задания
    MyScheduller(TaskVector taskVector, QObject *parent = 0); //конструктор, принимающий перечень заданий
    ~MyScheduller();        //деструктор

    void append(TaskPair oneTask);          //добавить одно задание
    void append(TaskVector taskVector);     //добавить перечень заданий

    void remove(TaskPair oneTask);  //удалить из вектора срабатывания все записи о конкретном задании и его cron-выражении

    void clear();   //очистить очередь заданий



signals:
    void timeOut(QString taskName);      //сигнал о начале выполнения нового задания
    void updateTasks(int index);
public slots:
    void slotUpdateTasks(int index);

    void startSheduller();
    void slotReaction(int ind);
    //void start(TaskPair oneTask);
    //void start(TaskVector taskVector);

    //void stop();
};

#endif // MYSCHEDULLER_H
