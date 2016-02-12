#ifndef MYSCHEDULLER_H
#define MYSCHEDULLER_H

#include <QObject>
#include <QTimer>
#include <QVector>
#include <QPair>
#include <QDateTime>
#include <QStringList>
#include <QDebug>

class MyScheduller : public QObject
{
    Q_OBJECT
private:
    //перечисление предельных значений времен
    enum timeLimits
    {
        MINUTES = 59,       //минуты
        HOURS = 23,         //часы
        DAYS_OF_MONTH = 31, //дни месяца
        MONTHS = 12,        //месяцы
        DAYS_OF_WEEK = 7    //дни недели
    };

    QVector <QPair <QString, QString>> tasks;       //перечень заданий
    QVector <QPair <QDateTime, QPair <QString, QString>>> nextTasks; //перечень будущих заданий
    QTimer timeCounter;         //таймер, отслеживающий конкретную дату
    QVector <int>   minute,     //значение минут
                    hour,       //значение часов
                    dayOfMonth, //значение дней месяца
                    month,      //значение месяца
                    dayOfWeek;  //значение дня недели

    QVector <int> parseCronJob(QString cronJob, int minLimit, int maxLimit); //обработка cron-выражения
    QVector <QDateTime> getNextDate();   //расчет новой даты срабатывания
    QVector<QDateTime> calcTime(QVector <int> time, QVector<QDateTime> nextDate, int limit);   //вычисление части даты для нового срабатывания
public:
    explicit MyScheduller(QVector <QPair<QString, QString> > variables, QObject *parent = 0);   //конструктор
    ~MyScheduller();        //деструктор

    QVector<QDateTime> calcNewCron(QString cronjob);     //подсчитать новую дату выполнения крон-выражения
    void addNewTask(QPair <QString, QString> newTask, QVector<QPair<QDateTime, QPair<QString, QString> > > &vect);
    void removeTask(QPair<QDateTime, QPair<QString, QString> > task);  //удалить задание
signals:
    void timeOut(QString taskName);      //сигнал о начале выполнения нового задания
    void updateTasks(QVector <QPair<QString, QString> > variables);
public slots:
    void slotAlarm();            //мониторинг времени и даты для выполнения нвого задания
    void slotUpdateTasks(QVector <QPair<QString, QString> > variables);
};

#endif // MYSCHEDULLER_H
