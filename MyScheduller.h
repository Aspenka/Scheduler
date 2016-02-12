#ifndef MYSCHEDULLER_H
#define MYSCHEDULLER_H

#include <QObject>
#include <QTimer>
#include <QVector>
#include <QPair>
#include <QDateTime>
#include <QStringList>
#include <QDebug>

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

//класс предназначен для вызова определенных процедур в указанное время, представленное в виде cron-выражения
class MyScheduller : public QObject
{
    Q_OBJECT
private:
    TaskPair task;
    TaskVector taskVect;        //перечень заданий
    TaskTimeVector nextTasks;   //вектор срабатываний, содержащий задание и время, в которое оно должно сработать
    QTimer timeCounter;         //таймер, отслеживающий конкретную дату

    QVector <int>   minute,     //значение минут
                    hour,       //значение часов
                    dayOfMonth, //значение дней месяца
                    month,      //значение месяца
                    dayOfWeek;  //значение дня недели

    void appendNewtTask(TaskPair oneTask);          //метод рассчитывает вектор срабатывания одного задания
    void appendNewtTask(TaskVector taskVector);     //метод рассчитывает вектор срабатывания для перечня заданий
    QVector<QDateTime> calcNewDatetime(QString cronjob);//метод рассчитывает даты срабатывания по cron-выражению
    QVector <int> parseCronJob(QString cronJob, int minLimit, int maxLimit);    //метод обрабатывает cron-выражение и возвращает
                                                                                //значение срабатывания для каждой единицы времени


    QVector <QDateTime> getNextDate();   //расчет новой даты срабатывания
    QVector<QDateTime> calcTime(QVector <int> time, QVector<QDateTime> nextDate, int limit);   //вычисление части даты для нового срабатывания


    //void addNewTask(TaskPair newTask, TaskTimeVector &vect);
    void removeTask(QPair<QDateTime, QPair<QString, QString> > task);  //удалить задание

public:
    explicit MyScheduller(QObject *parent = 0);  //пустой конструктор
    MyScheduller(TaskPair oneTask, QObject *parent = 0); //конструктор, принимающий один экземпляр задания
    MyScheduller(TaskVector taskVector, QObject *parent = 0); //конструктор, принимающий перечень заданий
    ~MyScheduller();        //деструктор

    void append(TaskPair oneTask);          //добавить одно задание
    void append(TaskVector taskVector);     //добавить перечень заданий

    void remove(TaskPair oneTask);  //удалить конкретное задание
    void remove(int index);         //удалить задание с заданным индексом

    void clear();   //очистить очередь заданий

    void start();
    void start(TaskPair oneTask);
    void start(TaskVector taskVector);

    void stop();

signals:
    void timeOut(QString taskName);      //сигнал о начале выполнения нового задания
    void updateTasks(TaskVector variables);
public slots:
    void slotStartTask();            //мониторинг времени и даты для выполнения нвого задания
    void slotUpdateTasks(TaskVector variables);
};

#endif // MYSCHEDULLER_H
