#include "MyScheduller.h"

//конструктор
MyScheduller::MyScheduller(QVector <QPair <QString, QString>> variables, QObject *parent) : QObject(parent)
{
    qDebug() << "Starting MyScheduller...";
    for(int i=0; i<variables.length(); i++)
        addNewTask(variables.at(i), nextTasks);
    timeCounter.start(1000);              //забить в QSettings
    connect(&timeCounter, SIGNAL(timeout()), this, SLOT(slotAlarm()));
    connect(this, SIGNAL(updateTasks(QVector<QPair<QString,QString> >)), this, SLOT(slotUpdateTasks(QVector<QPair<QString,QString> >)));
}

//добавить новое задание в список
void MyScheduller::addNewTask(QPair <QString, QString> newTask, QVector <QPair <QDateTime, QPair <QString, QString>>> &vect)
{
    qDebug() << "Adding new task = " << newTask;
    QVector <QDateTime> nextDT = calcNewCron(newTask.first);
    for(int i=0; i<nextDT.size(); i++)
    {
        QPair <QDateTime, QPair <QString, QString>> pair(nextDT.at(i), newTask);
        vect.append(pair);
    }
}

//удалить задание из списка
void MyScheduller::removeTask(QPair<QDateTime, QPair<QString, QString> > task)
{
    qDebug() << "Removing new task = " << task;
    nextTasks.removeOne(task);
}

//вычислить дату выполнения следующего задания
QVector <QDateTime> MyScheduller::calcNewCron(QString cronjob)
{
    QVector <QDateTime> nextDate;
    QStringList crons = cronjob.split(" ");                         //делим строку на части по пробелам
    if(crons.size() == 5)                                           //============================
    {                                                               //
        minute = parseCronJob(crons.at(0), 0, MINUTES);             //
        hour = parseCronJob(crons.at(1), 0, HOURS);                 //вычисляем массив значений
        dayOfMonth = parseCronJob(crons.at(2), 1, DAYS_OF_MONTH);   //для каждой единицы даты
        month = parseCronJob(crons.at(3), 1, MONTHS);               //
        dayOfWeek = parseCronJob(crons.at(4), 1, DAYS_OF_WEEK);     //=============================
        nextDate = getNextDate();                                   //получаем ветор новых времен срабатываний таска
    }
    else
    {
        qDebug() << "Error cronjob!";
    }
    return nextDate;
}

//распарсить крон-выражение
QVector<int> MyScheduller::parseCronJob(QString cronJob, int minLimit, int maxLimit)
{
    QVector <int> res;
    if(cronJob.contains("*"))
    {
        if(cronJob.contains("/"))
        {
            //символ "*/step", где step - интервал
            int step = cronJob.section("/", 1, 1).toInt();
            if(step >= minLimit && step<maxLimit)
                for(int i=0; i<maxLimit; i = i+step)
                    res.append(i);
        }
        else
        {
            //символ "*"
            res.append(-1);
        }
     }
     else if(cronJob.contains("-"))
        {
            if(cronJob.contains("/"))
            {
                QString tmp = cronJob.section("-", 1, 1);
                tmp = tmp.section("/", 0, 0);
                int finish = tmp.toInt();
                tmp.clear();

                tmp = cronJob.section("-", 0, 0);
                int start = tmp.toInt();

                int step = cronJob.section("/", 1, 1).toInt();

                for(int i=start; i<=finish; i+=step)
                    //значения типа start-finish/step
                    res.append(i);
            }
            else
            {
                int start = cronJob.section("-", 0, 0).toInt();
                int finish = cronJob.section("-", 1, 1).toInt();
                if(start > minLimit && finish < maxLimit)
                    for(int j=start; j<=finish; j++)
                    {
                        //интервал "start"-"finish"
                        res.append(j);
                    }
                else res.append(0);
            }
        }
        else if(cronJob.contains(","))
        {
            QStringList result;
            result = cronJob.split(",");
            //перечень значений через ","
            for(int j=0; j<result.size(); j++)
                if(result.at(j).toInt() > minLimit && result.at(j).toInt() < maxLimit)
                    res.append(result.at(j).toInt());
                else res.append(0);
        }
        else if(cronJob.contains("/"))
        {
            int start = cronJob.section("/", 0, 0).toInt();
            int step = cronJob.section("/", 1, 1).toInt();
            if(start >= minLimit && start < maxLimit)
                for(int i=start; i<maxLimit; i=i+step)
                    //значения типа start/step
                    res.append(i);
        }
        else if((cronJob.toInt() < maxLimit) && (cronJob.toInt() > minLimit))
        {
            res.append(cronJob.toInt());
        }
        else
        {
            res.append(0);
        }
    return res;
}

//вычисление даты следующего срабатывания выражения
QVector<QDateTime> MyScheduller::getNextDate()
{
    QVector <QDateTime> nextDate;
    QVector <QDateTime> tmp;

    tmp = calcTime(minute, nextDate, MINUTES);
    nextDate = tmp;
    tmp.clear();

    tmp = calcTime(hour, nextDate, HOURS);
    nextDate.clear();
    nextDate = tmp;
    tmp.clear();

    tmp = calcTime(dayOfMonth, nextDate, DAYS_OF_MONTH);
    nextDate.clear();
    nextDate = tmp;
    tmp.clear();

    tmp = calcTime(month, nextDate, MONTHS);
    nextDate.clear();
    nextDate = tmp;
    tmp.clear();

    tmp = calcTime(dayOfWeek, nextDate, DAYS_OF_WEEK);
    nextDate.clear();
    nextDate = tmp;
    tmp.clear();

    return nextDate;
}

//Функция возвращает вычисленную часть даты, поступившую на вход
QVector <QDateTime> MyScheduller::calcTime(QVector<int> time, QVector <QDateTime> nextDate, int limit)
{
    QDateTime date;
    if(nextDate.size()<1)
        nextDate.append(QDateTime::currentDateTime());
    QVector <QDateTime> newDate = nextDate;

    switch(limit)
    {
        case MINUTES:
        {
            int k=0;
            for(int i=0; i<time.size(); i++)
            {
                for(int j=0; j<nextDate.size(); j++)
                {
                    if(time.at(i) == -1)
                        date = nextDate.at(j);
                    else
                    {
                        int m = nextDate.at(j).time().minute();
                        if(m<time.at(i))
                            date = nextDate.at(j).addSecs(60*std::abs(time.at(i)-m));
                        else
                        {
                            QTime temp = nextDate.at(j).time();
                            date.setTime(QTime(temp.hour(), time.at(i), temp.second(), temp.msec()));
                            date.setDate(nextDate.at(j).date());
                        }
                    }
                    if(k>=nextDate.size())
                        newDate.append(date);
                    else
                    {
                        newDate.remove(j);
                        newDate.insert(j, date);
                    }
                    k++;
                }
            }
            break;
        }
        case HOURS:
        {
            int k=0;
            for(int i=0; i<time.size(); i++)
            {
                for(int j=0; j<nextDate.size(); j++)
                {
                    if(time.at(i) == -1)
                        date = nextDate.at(j);
                    else
                    {
                        int h = nextDate.at(j).time().hour();
                        if(h<=time.at(i))
                            date = nextDate.at(j).addSecs(3600*std::abs(time.at(i)-h));
                        else
                        {
                            QTime temp = nextDate.at(j).time();
                            date.setTime(QTime(time.at(i), temp.minute(), temp.second(), temp.msec()));
                            date.setDate(nextDate.at(j).date());
                        }
                    }
                    if(k>=nextDate.size())
                        newDate.append(date);
                    else
                    {
                        newDate.remove(j);
                        newDate.insert(j, date);
                    }
                    k++;
                }
            }
            break;
        }
        case DAYS_OF_MONTH:
        {
            int k=0;
            for(int i=0; i<time.size(); i++)
            {
                for(int j=0; j<nextDate.size(); j++)
                {
                    if(time.at(i) == -1)
                        date = nextDate.at(j);
                    else
                    {
                        int m = nextDate.at(j).date().day();
                        if(m<time.at(i))
                            date = nextDate.at(j).addDays(std::abs(time.at(i)-m));
                        else
                        {
                            if(time.at(i)!=0)
                            {
                                QDate temp = nextDate.at(j).date();
                                date.setDate(QDate(temp.year(), temp.month(), time.at(i)));
                                date.setTime(nextDate.at(j).time());
                            }
                            else date = nextDate.at(j);
                        }
                    }
                    if(k>=nextDate.size())
                        newDate.append(date);
                    else
                    {
                        newDate.remove(j);
                        newDate.insert(j, date);
                    }
                    k++;
                }
            }
            break;
        }
        case MONTHS:
        {
            int k=0;
            for(int i=0; i<time.size(); i++)
            {
                for(int j=0; j<nextDate.size(); j++)
                {
                    if(time.at(i) == -1)
                        date = nextDate.at(j);
                    else
                    {
                        int m = nextDate.at(j).date().month();
                        if(m<time.at(i))
                            date = nextDate.at(j).addMonths(std::abs(time.at(i)-m));
                        else
                        {
                            if(time.at(i)!=0)
                            {
                                QDate temp = nextDate.at(j).date();
                                date.setDate(QDate(temp.year(), time.at(i), temp.day()));
                                date.setTime(nextDate.at(j).time());
                            }
                            else date = nextDate.at(j);
                        }
                    }
                    if(k>=nextDate.size())
                        newDate.append(date);
                    else
                    {
                        newDate.remove(j);
                        newDate.insert(j, date);
                    }
                    k++;
                }
            }
            break;
        }
        case DAYS_OF_WEEK:
        {
            int k=0;
            for(int i=0; i<time.size(); i++)
            {
                for(int j=0; j<nextDate.size(); j++)
                {
                    if(time.at(i) == -1)
                        date = nextDate.at(j);
                    else
                    {
                        int d = nextDate.at(j).date().dayOfWeek();
                        if(d<time.at(i))
                            date = nextDate.at(j).addDays(std::abs(time.at(i)-d));
                        else
                        {
                            if(time.at(i)!=0)
                            {
                                d = 7 - d + time.at(i);
                                date = nextDate.at(j).addDays(d);
                            }
                            else date = nextDate.at(j);
                        }
                    }
                    if(k>=nextDate.size())
                        newDate.append(date);
                    else
                    {
                        newDate.remove(j);
                        newDate.insert(j, date);
                    }
                    k++;
                }
            }
            break;
        }
    }
    return newDate;
}

//мониторинг времени до выполнения нового задания
void MyScheduller::slotAlarm()
{
    QDateTime curDate = QDateTime::currentDateTime();
    //bool fl = false;

    for(int i=0; i<nextTasks.size(); i++)
    {
        if((curDate.date().month() == nextTasks.at(i).first.date().month())&&
           (curDate.date().day() == nextTasks.at(i).first.date().day())&&
           (curDate.date().dayOfWeek() == nextTasks.at(i).first.date().dayOfWeek())&&
           (curDate.time().minute() == nextTasks.at(i).first.time().minute())&&
           (curDate.time().hour() == nextTasks.at(i).first.time().hour()))
        {
            qDebug() << "ALARM!!" << nextTasks.at(i).first.toString("dd/MM/yy hh:mm:ss");
            timeCounter.stop();
            timeCounter.start(60000);
            //fl = true;
            emit timeOut(nextTasks.at(i).second.second);
            //emit updateTasks(tasks);
        }
    }
    //if(fl != false)emit updateTasks(tasks);
}

//
void MyScheduller::slotUpdateTasks(QVector<QPair<QString, QString> > variables)
{
    nextTasks.clear();
    for(int i=0; i<variables.length(); i++)
        addNewTask(variables.at(i), nextTasks);
}

//деструктор
MyScheduller::~MyScheduller()
{

}

