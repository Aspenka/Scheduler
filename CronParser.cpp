#include "CronParser.h"
#include <QDebug>
#include <iostream>

CronParser::CronParser(QObject *parent) : QObject(parent)
{
    cronJob = "";
    taskName = "";
    cronDate = QDateTime::currentDateTime();
}

CronParser::CronParser(QString cron, QString task, QObject *parent) : QObject(parent)
{
    cronJob = cron;
    taskName = task;
    cronDate = QDateTime::currentDateTime();
}

QString CronParser::getCronJob()
{
    return cronJob;
}

QString CronParser::getTaskName()
{
    return taskName;
}

QDateTime CronParser::getDateTime(QString cron, QString task)
{
    cronJob = cron;
    taskName = task;
    cronDate = calcTaskDate();
    qDebug() << cronDate;
    return cronDate;
}

QDateTime CronParser::calcTaskDate()
{
    QDateTime date = QDateTime::currentDateTime();
    QDateTime tmp;
    QStringList crons = cronJob.split(" ");                         //делим строку на части по пробелам;
    try
    {
        if(crons.size() != 5)
        {
            throw crons.size();
        }
        else
        {
            dayOfWeek = parse(crons.at(4));
            tmp = calcDateUnit(DAYS_OF_WEEK, dayOfWeek, date);
            date = tmp;

            month = parse(crons.at(3));
            tmp = calcDateUnit(MONTHS, month, date);
            date = tmp;

            dayOfMonth = parse(crons.at(2));
            tmp = calcDateUnit(DAYS_OF_MONTH, dayOfMonth, date);
            date = tmp;

            hour = parse(crons.at(1));
            tmp = calcDateUnit(HOURS, hour, date);
            date = tmp;

            minute = parse(crons.at(0));
            tmp = calcDateUnit(MINUTES, minute, date);
            date = tmp;
        }
    }
    catch(int)
    {
        std::cout << "[Scheduller]: Error: Invalid cronjob \"";
        std::cout << cronJob.toStdString() << "\"\n";
    }
    return date;
}

ValueProperties CronParser::parse(QString cron)
{
    ValueProperties res;
    if(cron.contains("*"))
    {
        if(cron.contains("/"))
        {
            res.step = cron.section("/", 1, 1).toInt();
            res.type = ANY_STEP;
        }
        else res.type = ANY;
    }
    else if(cron.contains("-"))
    {
        if(cron.contains("/"))
        {
            QString tmp = cron.section("-", 1, 1);
            tmp = tmp.section("/", 0, 0);
            res.finish = tmp.toInt();
            tmp.clear();
            tmp = cron.section("-", 0, 0);
            res.startVal.append(tmp.toInt());
            res.step = cron.section("/", 1, 1).toInt();
            res.type = INTERVAL_STEP;
        }
        else
        {
            res.startVal.append(cron.section("-", 0, 0).toInt());
            res.finish = cron.section("-", 1, 1).toInt();
            res.type = INTERVAL;
        }
    }
    else if(cronJob.contains(","))
    {
        QStringList result;
        result = cron.split(",");
        for(int i=0; i<result.size(); i++)
            res.startVal.append(result.at(i).toInt());
        res.type = ENUMERATION;
    }
    else if(cronJob.contains("/"))
    {
        res.startVal.append(cron.section("/", 0, 0).toInt());
        res.step = cron.section("/", 1, 1).toInt();
        res.type = START_STEP;
    }
    else
    {
        res.startVal.append(cron.toInt());
        res.type = VALUE;
    }
    return res;
}

QDateTime CronParser::calcDateUnit(int dateType, ValueProperties dateMean, QDateTime date)
{
    QDateTime result = date;
    switch(dateMean.type)
    {
        case ANY:
        {
            result = anyDateTime(dateType, dateMean, date);
            break;
        }
        case ANY_STEP:
        {
            dateMean.startVal.append(0);
            result = anyDateTime(dateType, dateMean, date);
            break;
        }
        case INTERVAL:
        {
            result = intervalDateTime(dateType, dateMean, date);
            break;
        }
        case INTERVAL_STEP:
        {
            result = intervalDateTime(dateType, dateMean, date);
            break;
        }
        case ENUMERATION:
        {
            break;
        }
        case START_STEP:
        {
            switch(dateType)
            {
                case MINUTES:
                {
                    break;
                }
                case HOURS:
                {
                    break;
                }
                case DAYS_OF_MONTH:
                {
                    break;
                }
                case MONTHS:
                {
                    break;
                }
                case DAYS_OF_WEEK:
                {
                    break;
                }
            }
            break;
        }
        case VALUE:
        {
            switch(dateType)
            {
                case MINUTES:
                {
                    break;
                }
                case HOURS:
                {
                    break;
                }
                case DAYS_OF_MONTH:
                {
                    break;
                }
                case MONTHS:
                {
                    break;
                }
                case DAYS_OF_WEEK:
                {
                    break;
                }
            }
            break;
        }
    }
    return result;
}

QDateTime CronParser::anyDateTime(int dateType, ValueProperties dateMean, QDateTime date)
{
    QDateTime result = QDateTime::currentDateTime();
    switch(dateType)
    {
        case MINUTES:
        {
            int temp = QTime::currentTime().minute();
            if(dateMean.startVal.empty())
                dateMean.startVal.append(temp);
            int min = calcSumma(dateMean.startVal.first(), MINUTES, temp, dateMean.step);
            result.setDate(date.date());
            result.setTime(QTime(date.time().hour(), min));
            break;
        }
        case HOURS:
        {
            int temp = QTime::currentTime().hour();
            if(dateMean.startVal.empty())
                dateMean.startVal.append(temp);
            int ho = calcSumma(dateMean.startVal.first(), HOURS, temp, dateMean.step);
            result.setDate(date.date());
            result.setTime(QTime(ho, date.time().minute()));
            break;
        }
        case DAYS_OF_MONTH:
        {
            int temp = QDate::currentDate().day();
            if(dateMean.startVal.empty())
                dateMean.startVal.append(temp);
            int dom = calcSumma(dateMean.startVal.first(), DAYS_OF_MONTH, temp, dateMean.step);
            result.setDate(QDate(date.date().year(), date.date().month(), dom));
            result.setTime(date.time());
            break;
        }
        case MONTHS:
        {
            int temp = QDate::currentDate().month();
            if(dateMean.startVal.empty())
                dateMean.startVal.append(temp);
            int limit = QDate::currentDate().daysInMonth();
            int mon = calcSumma(dateMean.startVal.first(), limit, temp, dateMean.step);
            result.setDate(QDate(date.date().year(), mon, date.date().day()));
            result.setTime(date.time());
            break;
        }
        case DAYS_OF_WEEK:
        {
            int temp = QDate::currentDate().dayOfWeek();
            if(dateMean.startVal.empty())
                dateMean.startVal.append(temp);
            int dow = calcSumma(dateMean.startVal.first(), DAYS_OF_WEEK, temp, dateMean.step);
            if(dow > 0)
            {
                date.addDays(dateMean.step);
                result = date;
            }
            break;
        }
    }
    return result;
}

QDateTime CronParser::intervalDateTime(int dateType, ValueProperties dateMean, QDateTime date)
{
    QDateTime result = QDateTime::currentDateTime();
    switch(dateType)
    {
        case MINUTES:
        {
            int current = QTime::currentTime().minute();
            int min = calcSumma(dateMean.startVal.first(), dateMean.finish, current, dateMean.step);
            if(min >= current)
            result.setTime(QTime(date.time().hour(), min));
            else
            {
                current = QTime::currentTime().hour()+1;
                if((current <= hour.finish)||(hour.type == ANY))
                    result.setTime(QTime(current, min));
                else result.setTime(QTime(date.time().hour(), min));
            }
            result.setDate(date.date());
            break;
        }
        case HOURS:
        {
            int current = QTime::currentTime().hour();
            int ho = calcSumma(dateMean.startVal.first(), dateMean.finish, current, dateMean.step);
            if(ho >= current)
            {
                result.setTime(QTime(ho, date.time().minute()));
                result.setDate(date.date());
            }
            else
            {
                current = QDate::currentDate().day()+1;
                if((current <= dayOfMonth.finish)||(dayOfMonth.type == ANY))
                {
                    result.setTime(QTime(ho, date.time().minute()));
                    result.setDate(QDate(date.date().year(), date.date().month(), current));
                }
                else
                {
                    result.setTime(QTime(ho, date.time().minute()));
                    result.setDate(date.date());
                }
            }
            break;
        }
        case DAYS_OF_MONTH:
        {
            int current = QDate::currentDate().day();
            int dom = calcSumma(dateMean.startVal.first(), dateMean.finish, current, dateMean.step);
            if(dom >= current)
                result.setDate(QDate(date.date().year(), date.date().month(), dom));
            else
            {
                current = QDate::currentDate().month()+1;
                if((current <= month.finish)||(month.type == ANY))
                    result.setDate(QDate(date.date().year(), current, dom));
                else
                    result.setDate(QDate(date.date().year(), date.date().month(), dom));
            }
            result.setTime(date.time());
            break;
        }
        case MONTHS:
        {
            int current = QDate::currentDate().month();
            int mo = calcSumma(dateMean.startVal.first(), dateMean.finish, current, dateMean.step);
            if(mo >= current)
                result.setDate(QDate(date.date().year(), mo, date.date().day()));
            else
            {
                current = QDate::currentDate().year()+1;
                result.setDate(QDate(current, mo, date.date().day()));
            }
            result.setTime(date.time());
            break;
        }
        case DAYS_OF_WEEK:
        {
            int current = QDate::currentDate().dayOfWeek();
            int dow = calcSumma(dateMean.startVal.first(), dateMean.finish, current, dateMean.step);

            //??????????????
            break;
        }
    }
    return result;
}

QDateTime CronParser::enumDateTime(int dateType, ValueProperties dateMean, QDateTime date)
{
    switch(dateType)
    {
        case MINUTES:
        {
            int current = QTime::currentTime().minute();
            int min = chooseValue(current, dateMean.startVal);
            break;
        }
        case HOURS:
        {
            break;
        }
        case DAYS_OF_MONTH:
        {
            break;
        }
        case MONTHS:
        {
            break;
        }
        case DAYS_OF_WEEK:
        {
            break;
        }
    }
}

int CronParser::calcSumma(int start, int finish, int current, int step)
{
    int i = start + step;
    if(step == 0)step++;
    while((i < current) && (i != finish))
        i += step;
    if(i < finish) return i;
    else if((i == finish) && current == finish)return finish;
    else return start;
}

int CronParser::chooseValue(int current, QVector<int> value)
{
    int i=0;
    while((current>value.at(i))&&(i < value.size()))
    {
        i++;
    }
    if(i != value.size()) return value.at(i);
    else return value.at(0);
}

CronParser::~CronParser()
{

}

