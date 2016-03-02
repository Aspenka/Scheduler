#include "CronParser.h"
#include <iostream>
#include <QStringList>
#include <QRegExp>

//конструктор
CronParser::CronParser(QObject *parent) : QObject(parent)
{
    cronJob = "";
    cronDate = QDateTime::currentDateTime();
}

//метод возвращает cronJob
QString CronParser::getCronJob()
{
    return cronJob;
}

//метод возвращает ближайшую дату вызова функции
QDateTime CronParser::getDateTime(QString cron)
{
    cronJob = cron;
    cronDate = calcTaskDate();
    return cronDate;
}

//метод парсит cron-выражение и рассчитывает ближайшую дату вызова функции
QDateTime CronParser::calcTaskDate()
{
    QVector<QDateTime> dateList,
                       tmp;
    QDateTime optimalDate;
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

//метод парсит cron-выражения
QVector<int> CronParser::parse(QString cronJob, int minLimit, int maxLimit)
{
    QVector <int> res;

    int step = 1;
    int start = minLimit;
    int finish = maxLimit;

    QRegExp comma("((\\d+)(,))+(\\d+)");
    QRegExp value("(\\d+)");
    QRegExp star("\\*");
    QRegExp starStep("(\\*)(\\/)(\\d+)");
    QRegExp startStep("(\\d+)(\\/)(\\d+)");
    QRegExp startFinish("(\\d+)(\\-)(\\d+)");
    QRegExp startFinishStep("(\\d+)(\\-)(\\d+)(\\/)(\\d+)");

    if(comma.exactMatch(cronJob))
    {
        QStringList result;
        result = cronJob.split(",");
        for(int i=0; i<result.size(); i++)
        {
            try
            {
                if(result.at(i).toInt() < minLimit || result.at(i).toInt() > maxLimit)
                    throw result.at(i);
                else res.append(result.at(i).toInt());
            }
            catch(QString)
            {
                std::cout << "[Scheduller]: Error: Invalid value in a cronjob \"";
                std::cout << cronJob.toStdString() << "\"\n";
            }
        }
    }
    else if(value.exactMatch(cronJob))
    {
        try
        {
            if(cronJob.toInt() < minLimit || cronJob.toInt() > maxLimit)
                throw cronJob.toInt();
            else res.append(cronJob.toInt());
        }
        catch(int)
        {
            std::cout << "[Scheduller]: Error: Invalid value in a cronjob \"";
            std::cout << cronJob.toStdString() << "\"\n";
        }
    }
    else
    {
        if(star.exactMatch(cronJob))
            step = 1;
        else if(starStep.exactMatch(cronJob))
            step = cronJob.section("/", 1, 1).toInt();
        else if(startStep.exactMatch(cronJob))
        {
            start = cronJob.section("/", 0, 0).toInt();
            step = cronJob.section("/", 1, 1).toInt();
        }
        else if(startFinish.exactMatch(cronJob))
        {
            start = cronJob.section("-", 0, 0).toInt();
            finish = cronJob.section("-", 1, 1).toInt();
        }
        else if(startFinishStep.exactMatch(cronJob))
        {
            step = cronJob.section("/", 1, 1).toInt();
            start = cronJob.section("-", 0, 0).toInt();
            QString tmp = cronJob.section("-", 1, 1);
            tmp = tmp.section("/", 0, 0);
            finish = tmp.toInt();
        }
        else
            start = -1;

        try
        {
            if(start < minLimit || start > maxLimit)
                throw start;
            else if(step < minLimit || step > maxLimit)
                throw step;
            else
            {
                for(int i = start; i <= finish; i+= step)
                    res.append(i);
            }
        }
        catch(int)
        {
            std::cout << "[Scheduller]: Error: Invalid value in a cronjob \"";
            std::cout << cronJob.toStdString() << "\"\n";
        }
    }
    return res;
}

//метод выбирает все возможные даты вызова функции по cron-выражению
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
                    int m = nextDate.at(j).time().minute();
                    if(m<time.at(i))
                        date = nextDate.at(j).addSecs(60*std::abs(time.at(i)-m));
                    else
                    {
                        QTime temp = nextDate.at(j).time();
                        date.setTime(QTime(temp.hour(), time.at(i)));
                        date.setDate(nextDate.at(j).date());
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
                    int h = nextDate.at(j).time().hour();
                    if(h<time.at(i))
                        date = nextDate.at(j).addSecs(3600*std::abs(time.at(i)-h));
                    else
                    {
                        QTime temp = nextDate.at(j).time();
                        date.setTime(QTime(time.at(i), temp.minute()));
                        date.setDate(nextDate.at(j).date());
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
                    int m = nextDate.at(j).date().day();
                    if(m<time.at(i))
                        date = nextDate.at(j).addDays(std::abs(time.at(i)-m));
                    else
                    {
                        QDate temp = nextDate.at(j).date();
                        date.setDate(QDate(temp.year(), temp.month(), time.at(i)));
                        date.setTime(nextDate.at(j).time());
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

                    int m = nextDate.at(j).date().month();
                    if(m<time.at(i))
                        date = nextDate.at(j).addMonths(std::abs(time.at(i)-m));
                    else
                    {
                        QDate temp = nextDate.at(j).date();
                        date.setDate(QDate(temp.year(), time.at(i), temp.day()));
                        date.setTime(nextDate.at(j).time());
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
                    int d = nextDate.at(j).date().dayOfWeek();
                    if(d<time.at(i))
                        date = nextDate.at(j).addDays(std::abs(time.at(i)-d));
                    else
                    {
                        d = 7 - d + time.at(i);
                        date = nextDate.at(j).addDays(d);
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

//метод выбирает ближайшую дату вызова функции
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

//деструктор
CronParser::~CronParser()
{

}

