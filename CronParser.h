#ifndef CRONPARSER_H
#define CRONPARSER_H

#include <QObject>
#include <QString>
#include <QPair>
#include <QVector>
#include <QDateTime>

//перечисление предельных значений времен
enum timeLimits
{
    MINUTES = 59,       //минуты
    HOURS = 23,         //часы
    DAYS_OF_MONTH = 31, //дни месяца
    MONTHS = 12,        //месяцы
    DAYS_OF_WEEK = 7    //дни недели
};

class CronParser : public QObject
{
    Q_OBJECT
private:
    QString cronJob;            //cron-выражение
    QDateTime cronDate;         //дата вызова функции
    QVector <int>   minute,     //значение минут
                    hour,       //значение часов
                    dayOfMonth, //значение дней месяца
                    month,      //значение месяца
                    dayOfWeek;  //значение дня недели

    QVector <int> parse(QString cronJob, int minLimit, int maxLimit);   //метод обрабатывает cron-выражение и возвращает
                                                                        //значение срабатывания для каждой единицы времени
    QDateTime calcTaskDate();   //метод парсит cron-выражение и рассчитывает ближайшую дату вызова функции
    QVector<QDateTime> calcDateUnit(QVector<int> time, QVector <QDateTime> nextDate, int limit);    //метод выбирает все возможные
                                                                                                    //даты вызова функции по cron-выражению
    QDateTime chooseReactionDate(QVector<QDateTime> dateList);  //метод выбирает ближайшую дату вызова функции

public:
    explicit CronParser(QObject *parent = 0);   //конструктор
    ~CronParser();  //деструктор

    QString getCronJob();   //метод возвращает cron-выражение
    QDateTime getDateTime(QString cron);    //метод возвращает время ближайшего вызова функции

signals:

public slots:
};

#endif // CRONPARSER_H
