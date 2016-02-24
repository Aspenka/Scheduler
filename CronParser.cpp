#include "CronParser.h"
#include <QDebug>
#include <iostream>

CronParser::CronParser(QObject *parent) : QObject(parent)
{
    cronJob = "";
    cronDate = QDateTime::currentDateTime();
}

QString CronParser::getCronJob()
{
    return cronJob;
}

QDateTime CronParser::getDateTime(QString cron)
{
    cronJob = cron;
    cronDate = calcTaskDate();
    return cronDate;
}

QDateTime CronParser::calcTaskDate()
{
    QVector<QDateTime> dateList,
                       tmp;
    QDateTime optimalDate = QDateTime::currentDateTime();
    QStringList crons = cronJob.split(" ");                         //делим строку на части по пробелам;
    try
    {
        if(crons.size() != 5)
        {
            throw crons.size();
        }
        else
        {
            month = parse(crons.at(3), 1, MONTHS);
            tmp = calcDateUnit(month, dateList, MONTHS);
            dateList = tmp;
            tmp.clear();

            int dow = QDate::currentDate().daysInMonth();
            dayOfMonth = parse(crons.at(2), 1, dow);
            tmp = calcDateUnit(dayOfMonth, dateList, dow);
            dateList.clear();
            dateList = tmp;
            tmp.clear();

            dayOfWeek = parse(crons.at(4), 1, DAYS_OF_WEEK);
            tmp = calcDateUnit(dayOfWeek, dateList, DAYS_OF_WEEK);
            dateList.clear();
            dateList = tmp;
            tmp.clear();

            hour = parse(crons.at(1), 0, HOURS);
            tmp = calcDateUnit(hour, dateList, HOURS);
            dateList.clear();
            dateList = tmp;
            tmp.clear();

            minute = parse(crons.at(0), 0, MINUTES);
            tmp = calcDateUnit(minute, dateList, MINUTES);
            dateList.clear();
            dateList = tmp;
            tmp.clear();

            optimalDate = chooseReactionDate(dateList);
        }
    }
    catch(int)
    {
        std::cout << "[Scheduller]: Error: Invalid cronjob \"";
        std::cout << cronJob.toStdString() << "\"\n";
    }
    return optimalDate;
}

QVector<int> CronParser::parse(QString cronJob, int minLimit, int maxLimit)
{
    QVector <int> res;
        if(cronJob.contains("*"))
        {
            if(cronJob.contains("/"))
            {
                //символ "*/step", где step - интервал
                int step = cronJob.section("/", 1, 1).toInt();
                if(step >= minLimit && step<=maxLimit)
                    for(int i=0; i<=maxLimit; i = i+step)
                    {
                        if(((maxLimit == DAYS_OF_MONTH) && i != 0)||
                           ((maxLimit == DAYS_OF_WEEK) && i != 0)||
                           ((maxLimit == MONTHS) && i != 0))
                        res.append(i);
                        else if((maxLimit == HOURS) || (maxLimit == MINUTES))
                            res.append(i);
                    }
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
                    if(start >= minLimit && finish <= maxLimit)
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
                    if(result.at(j).toInt() >= minLimit && result.at(j).toInt() <= maxLimit)
                        res.append(result.at(j).toInt());
                    else res.append(0);
            }
            else if(cronJob.contains("/"))
            {
                int start = cronJob.section("/", 0, 0).toInt();
                int step = cronJob.section("/", 1, 1).toInt();
                if(start >= minLimit && start <= maxLimit)
                    for(int i=start; i<=maxLimit; i=i+step)
                        //значения типа start/step
                        res.append(i);
            }
            else if((cronJob.toInt() <= maxLimit) && (cronJob.toInt() >= minLimit))
            {
                res.append(cronJob.toInt());
            }
            else
            {
                res.append(0);
            }
        return res;
}

QVector <QDateTime> CronParser::calcDateUnit(QVector<int> time, QVector <QDateTime> nextDate, int limit)
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
                    {
                        QTime temp = nextDate.at(j).time();
                        date.setDate(nextDate.at(j).date());
                        date.setTime(QTime(temp.hour(), QTime::currentTime().minute()));
                    }
                    else
                    {
                        int m = nextDate.at(j).time().minute();
                        if(m<time.at(i))
                            date = nextDate.at(j).addSecs(60*std::abs(time.at(i)-m));
                        else
                        {
                            QTime temp = nextDate.at(j).time();
                            date.setTime(QTime(temp.hour(), time.at(i)));
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
                    {
                        QTime temp = nextDate.at(j).time();
                        date.setDate(nextDate.at(j).date());
                        date.setTime(QTime(QTime::currentTime().hour(), temp.minute()));
                    }
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
                    {
                        date.setTime(nextDate.at(j).time());
                        QDate temp = nextDate.at(j).date();
                        date.setDate(QDate(temp.year(), temp.month(), QDate::currentDate().day()));
                    }
                    else
                    {
                        int m = nextDate.at(j).date().day();
                        if(m<time.at(i))
                            date = nextDate.at(j).addDays(std::abs(time.at(i)-m));
                        else
                        {
                            QDate temp = nextDate.at(j).date();
                            date.setDate(QDate(temp.year(), temp.month(), time.at(i)));
                            date.setTime(nextDate.at(j).time());
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
                    {
                        date.setTime(nextDate.at(j).time());
                        QDate temp = nextDate.at(j).date();
                        date.setDate(QDate(temp.year(), QDate::currentDate().month(), temp.day()));
                    }
                    else
                    {
                        int m = nextDate.at(j).date().month();
                        if(m<time.at(i))
                            date = nextDate.at(j).addMonths(std::abs(time.at(i)-m));
                        else
                        {
                            QDate temp = nextDate.at(j).date();
                            date.setDate(QDate(temp.year(), time.at(i), temp.day()));
                            date.setTime(nextDate.at(j).time());
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
                    {
                        date.setTime(nextDate.at(j).time());
                        QDate temp = nextDate.at(j).date();
                        date.setDate(QDate(temp.year(), temp.month(), QDate::currentDate().day()));
                    }
                    else
                    {
                        int d = nextDate.at(j).date().dayOfWeek();
                        if(d<time.at(i))
                            date = nextDate.at(j).addDays(std::abs(time.at(i)-d));
                        else
                        {
                            d = 7 - d + time.at(i);
                            date = nextDate.at(j).addDays(d);
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

QDateTime CronParser::chooseReactionDate(QVector<QDateTime> dateList)
{
    QDateTime current = QDateTime::currentDateTime();
    QVector<QDateTime> temp;
    for(int i=0; i<dateList.size(); i++)
    {
        if(dateList.at(i)>=current)
            temp.append(dateList.at(i));
    }
    if(temp.empty())temp = dateList;
    QDateTime min = temp.at(0);
    for(int i=0; i<temp.size(); i++)
    {
        if(temp.at(i) <= min)
            min = temp.at(i);
    }
    return min;
}

CronParser::~CronParser()
{

}

