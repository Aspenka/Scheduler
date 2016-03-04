#include "CronParser.h"
#include <iostream>
#include <QStringList>
#include <QRegExp>

//конструктор
CronParser::CronParser(QObject *parent) : QObject(parent)
{
    cronJob = "";
    cronDate = QDateTime::currentDateTime();
    isCalled = false;
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
            int value;
            value = QTime::currentTime().minute();
            if(isCalled != true)
            {
                value += 1;
                setCall(false);
            }
            minute = parse(crons.at(0), value, 0, 59);

            value = QTime::currentTime().hour();
            if(minute.second != false)value +=1;
            hour = parse(crons.at(1), value, 0, 23);

            value = QDate::currentDate().day();
            if(hour.second != false)value += 1;
            dayOfMonth = parse(crons.at(2), value, 1, QDate::currentDate().daysInMonth());

            value = QDate::currentDate().month();
            if(dayOfMonth.second != false)value += 1;
            month = parse(crons.at(3), value, 1, 12);

            year.first = QDate::currentDate().year();
            if(month.second != false)year.first += 1;

            value = QDate::currentDate().dayOfWeek();
            dayOfWeek = parse(crons.at(4), value, 1, 7);

            if(dayOfWeek.first < value)dayOfWeek.first = value - dayOfWeek.first;
            else if(dayOfWeek.first == value) dayOfWeek.first = 0;
            else dayOfWeek.first = 7 - dayOfWeek.first + value;

            value = QDate::currentDate().day() + dayOfWeek.first;
            try
            {
                if(crons.at(2) == "*")dayOfMonth.first = value;
                else if(dayOfWeek.first != value)
                    throw value;
            }
            catch(int)
            {
                std::cout << "[Scheduller]: Error: Invalid cronjob \"";
                std::cout << cronJob.toStdString() << "\". Day of week does not correspond with day of month\n";
            }

            optimalDate.setDate(QDate(year.first, month.first, dayOfMonth.first));
            optimalDate.setTime(QTime(hour.first, minute.first, 0));
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
QPair<int, bool> CronParser::parse(QString cronJob, int var, int minLimit, int maxLimit)
{
    QPair <int, bool> res;
    res.first = var;
    res.second = false;

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
        int i=0;
        bool f = false;
        while(i < result.size() && result.at(i).toInt() < var)
            i++;
        if(i == result.size())
        {
            i = 0;
            f = true;
        }
        try
        {
            if(result.at(i).toInt() < minLimit && result.at(i).toInt() > maxLimit)
                throw result.at(i);
            else
            {
                res.first = result.at(i).toInt();
                res.second = f;
            }
        }
        catch(QString)
        {
            std::cout << "[Scheduller]: Error: Invalid value in a cronjob \"";
            std::cout << cronJob.toStdString() << "\"\n";
        }
    }
    else if(value.exactMatch(cronJob))
    {
        try
        {
            if(cronJob.toInt() < minLimit || cronJob.toInt() > maxLimit)
                throw cronJob.toInt();
            else
            {
                if(cronJob.toInt() < var) res.second = true;
                res.first = cronJob.toInt();
            }
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
            else if(finish < minLimit || finish > maxLimit)
                throw finish;
            else if(step < minLimit || step > maxLimit)
                throw step;
            else
            {
                int i = start;
                while(i < var && i <= finish)
                    i += step;
                if(i > finish)
                {
                    res.first = start;
                    res.second = true;
                }
                else res.first = i;
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

void CronParser::setCall(bool var)
{
    isCalled = var;
}

//деструктор
CronParser::~CronParser()
{

}

