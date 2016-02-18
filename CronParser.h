#ifndef CRONPARSER_H
#define CRONPARSER_H

#include <QObject>
#include <QString>
#include <QPair>
#include <QVector>
#include <QDateTime>

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
struct ValueProperties
{
    int type;
    QVector<int> startVal;
    int step=0;
    int finish = 0;
};

class CronParser : public QObject
{
    Q_OBJECT
private:
    QString cronJob,
            taskName;
    QDateTime cronDate;

    ValueProperties minute,     //значение минут
                    hour,       //значение часов
                    dayOfMonth, //значение дней месяца
                    month,      //значение месяца
                    dayOfWeek;  //значение дня недели

    ValueProperties parse(QString cron);    //метод обрабатывает cron-выражение и возвращает
                                               //значение срабатывания для каждой единицы времени
    QDateTime calcTaskDate();
    QDateTime calcDateUnit(int dateType, ValueProperties dateMean, QDateTime date);

    QDateTime anyDateTime(int dateType, ValueProperties dateMean, QDateTime date);
    QDateTime intervalDateTime(int dateType, ValueProperties dateMean, QDateTime date);
    QDateTime enumDateTime(int dateType, ValueProperties dateMean, QDateTime date);

    int calcSumma(int start, int finish, int current, int step);
    int chooseValue(int current, QVector<int> value);

public:
    explicit CronParser(QObject *parent = 0);
    CronParser(QString cron, QString task, QObject *parent = 0);
    ~CronParser();

    QString getCronJob();
    QString getTaskName();
    QDateTime getDateTime(QString cron, QString task);

signals:

public slots:
};

#endif // CRONPARSER_H
