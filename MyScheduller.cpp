#include <iostream>
#include "MyScheduller.h"

//пустой конструктор
MyScheduller::MyScheduller(QObject *parent) : QObject(parent)
{
    std::cout << "[Scheduller]: Starting...\n";
}

//конструктор, принимающий одно задание
MyScheduller::MyScheduller(TaskPair oneTask, QObject *parent) : QObject(parent)
{
    std::cout << "[Scheduller]: Starting...\n";
    append(oneTask);
}

//конструктор, принимающий перечень заданий
MyScheduller::MyScheduller(TaskVector taskVector, QObject *parent) : QObject(parent)
{
    std::cout << "[Scheduller]: Starting...\n";
    //timeCounter = new QTimer();
    appendNewTask(taskVector);

    /*qDebug() << "Starting MyScheduller...";
    for(int i=0; i<taskVector.length(); i++)
        addNewTask(taskVector.at(i), nextTasks);
    timeCounter.start(1000);              //забить в QSettings
    connect(&timeCounter, SIGNAL(timeout()), this, SLOT(slotStartTask()));*/
    //connect(this, SIGNAL(updateTasks(TaskVector)), this, SLOT(slotUpdateTasks(TaskVector)));
}

//добавить новое задание в список
void MyScheduller::append(TaskPair oneTask)
{
    taskVect.append(oneTask);
    appendNewTask(oneTask);
}

//добавить перечень новых заданий в список
void MyScheduller::append(TaskVector taskVector)
{
    taskVect = taskVector;
    append(taskVector);
}

//расчет вектора срабатывания для одного задания
void MyScheduller::appendNewTask(TaskPair oneTask)
{
    //std::cout << "[Scheduller]: adding new task: ";
    //std::cout << oneTask.first.toStdString() << " " << oneTask.second.toStdString() << "\n";
    QVector<QDateTime> dates = calcNewDatetime(oneTask.first);  //получение массива дат для следующего срабатывания
    for(int i=0; i<dates.size(); i++)
    {
        TaskTime pair(dates.at(i), oneTask);
        nextTasks.append(pair);                             //добавление дат срабатывания для задания к вектору срабатывания
    }
}

//расчет вектора срабатывания для перечня заданий
void MyScheduller::appendNewTask(TaskVector taskVector)
{
    //std::cout << "[Scheduller]: is vector of tasks:\n";
    for(int i=0; i<taskVector.size(); i++)
        append(taskVector.at(i));
    //TaskTime time = nextTasks.at(2);
    //remove(time);
}

//метод рассчитывает даты срабатывания по cron-выражению
QVector <QDateTime> MyScheduller::calcNewDatetime(QString cronjob)
{
    QVector <QDateTime> nextDate;
    QStringList crons = cronjob.split(" ");                         //делим строку на части по пробелам
    //если количество полученных частей не равно 5, то cron-выражение является ошибочным
    try
    {
        if(crons.size() != 5)
        {
            throw crons.size();
        }
        else
        {
            minute = parseCronJob(crons.at(0), 0, MINUTES);             //===========================
            hour = parseCronJob(crons.at(1), 0, HOURS);                 //получаем массив значений
            dayOfMonth = parseCronJob(crons.at(2), 1, DAYS_OF_MONTH);   //для каждой составляющей даты
            month = parseCronJob(crons.at(3), 1, MONTHS);               //
            dayOfWeek = parseCronJob(crons.at(4), 1, DAYS_OF_WEEK);     //============================
            nextDate = getNextDate();                                   //получаем перечень, содержащий время срабатываний заданий
        }
    }
    catch(int)
    {
        std::cout << "[Scheduller]: Error: Invalid cronjob \"";
        std::cout << cronjob.toStdString() << "\"\n";
    }
    return nextDate;
}

//метод парсит крон-выражение и возвращает значение срабатывания для каждой единицы времени
QVector<int> MyScheduller::parseCronJob(QString cronJob, int minLimit, int maxLimit)
{
    QVector <int> res;
    //обработка последовательностей типа '*' и '*/step',
    //где step - значение интервала, в который выполняется операция,
    //* - любое значение
    if(cronJob.contains("*"))
    {
        if(cronJob.contains("/"))
        {
            //символ "*/step"
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
     //обаботка последовательностей типа 'start-finish' 'start-finish/step',
     //где start - начальное значение срабатывания,
     //stop - конечное значение срабатывания,
     //step - значение интервала, в который выполняется операция
     else if(cronJob.contains("-"))
        {
            //значения типа start-finish/step
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
                    res.append(i);
            }
            //значения типа "start"-"finish"
            else
            {
                int start = cronJob.section("-", 0, 0).toInt();
                int finish = cronJob.section("-", 1, 1).toInt();
                if(start > minLimit && finish < maxLimit)
                    for(int j=start; j<=finish; j++)
                        res.append(j);
                else res.append(0);
            }
        }
        //перечисление значений через ","
        else if(cronJob.contains(","))
        {
            QStringList result;
            result = cronJob.split(",");
            for(int j=0; j<result.size(); j++)
                if(result.at(j).toInt() > minLimit && result.at(j).toInt() < maxLimit)
                    res.append(result.at(j).toInt());
                else res.append(0);
        }
        //значения типа "start/step", где
        //start - начальное значение,
        //step - интервал, в который срабатывает вызов
        else if(cronJob.contains("/"))
        {
            int start = cronJob.section("/", 0, 0).toInt();
            int step = cronJob.section("/", 1, 1).toInt();
            if(start >= minLimit && start < maxLimit)
                for(int i=start; i<maxLimit; i=i+step)
                    res.append(i);
        }
        //число, входящее в рамки допустимых значений для данной временной единицы
        else if((cronJob.toInt() < maxLimit) && (cronJob.toInt() > minLimit))
        {
            res.append(cronJob.toInt());
        }
        //ВЫПИЛИТЬ???!!! НУ ИЛИ ОТЛОВИТЬ TRY/CATCH
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

    tmp = calcTimeUnit(minute, nextDate, MINUTES);
    nextDate = tmp;
    tmp.clear();

    tmp = calcTimeUnit(hour, nextDate, HOURS);
    nextDate.clear();
    nextDate = tmp;
    tmp.clear();

    tmp = calcTimeUnit(dayOfMonth, nextDate, DAYS_OF_MONTH);
    nextDate.clear();
    nextDate = tmp;
    tmp.clear();

    tmp = calcTimeUnit(month, nextDate, MONTHS);
    nextDate.clear();
    nextDate = tmp;
    tmp.clear();

    tmp = calcTimeUnit(dayOfWeek, nextDate, DAYS_OF_WEEK);
    nextDate.clear();
    nextDate = tmp;
    tmp.clear();

    return nextDate;
}

//Функция возвращает вычисленную часть даты, поступившую на вход
QVector <QDateTime> MyScheduller::calcTimeUnit(QVector<int> time, QVector <QDateTime> nextDate, int limit)
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

//удалить одно задание из списка
void MyScheduller::removeOne(TaskTime task)
{
    nextTasks.removeOne(task);
}

//удалить все задания, содержащие cron-выражение и имя таска, переданных в параметре
void MyScheduller::remove(TaskPair oneTask)
{
    for(int i=0; i<nextTasks.size(); i++)
    {
        if(nextTasks.at(i).second == oneTask)
            removeOne(nextTasks.at(i));
    }
}

//очистить вектор срабатываний
void MyScheduller::clear()
{
    nextTasks.clear();
}


/*void MyScheduller::addNewTask(QPair <QString, QString> newTask, QVector <QPair <QDateTime, QPair <QString, QString>>> &vect)
{
    qDebug() << "Adding new task = " << newTask;
    QVector <QDateTime> nextDT = calcNewCron(newTask.first);
    for(int i=0; i<nextDT.size(); i++)
    {
        QPair <QDateTime, QPair <QString, QString>> pair(nextDT.at(i), newTask);
        vect.append(pair);
    }
}*/

int MyScheduller::calcTimeout(TaskTime oneTask)
{
    int msec;
    QDateTime curDate = QDateTime::currentDateTime();
    QDateTime taskDate = oneTask.first;
    msec = (taskDate.toTime_t() - curDate.toTime_t())*1000;
    if(msec > INT_MAX||msec < 0)
    {
        return -1;
    }
    return msec;
}

void MyScheduller::startSheduller()
{
    timer = new Timer[nextTasks.size()];
    for(int i=0; i<nextTasks.size(); i++)
    {
        int interval = calcTimeout(nextTasks.at(i));
        if(interval >= 0)
        {
            timer[i].setIndex(i);
            timer[i].start(interval);
            connect(&timer[i], SIGNAL(timeout()), &timer[i], SLOT(sentTimerIndex()));
            connect(&timer[i], SIGNAL(sentIndex(int)), this, SLOT(slotReaction(int)));
        }
    }
}

void MyScheduller::slotReaction(int ind)
{
    timer[ind].stop();
    QDateTime curDate = QDateTime::currentDateTime();
    for(int i=0; i < nextTasks.size(); i++)
    {
        if((curDate.date().month() == nextTasks.at(i).first.date().month())&&
          (curDate.date().day() == nextTasks.at(i).first.date().day())&&
          (curDate.date().dayOfWeek() == nextTasks.at(i).first.date().dayOfWeek())&&
          (curDate.time().minute() == nextTasks.at(i).first.time().minute())&&
          (curDate.time().hour() == nextTasks.at(i).first.time().hour()))
        {
            qDebug() << "[Scheduller " << nextTasks.at(i).first.toString("dd/MM/yy hh:mm:ss") << "]:    " << "Call " << nextTasks.at(i).second.second;
            emit timeOut(nextTasks.at(i).second.second);
        }
    }
    qDebug() << taskVect.size();
}

//
void MyScheduller::slotUpdateTasks(TaskVector variables)
{
    nextTasks.clear();
    for(int i=0; i<variables.length(); i++)
        append(variables.at(i));
}

//деструктор
MyScheduller::~MyScheduller()
{

}

