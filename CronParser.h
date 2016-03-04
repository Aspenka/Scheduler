#ifndef CRONPARSER_H
#define CRONPARSER_H

#include <QObject>
#include <QString>
#include <QPair>
#include <QVector>
#include <QDateTime>

class CronParser : public QObject
{
    Q_OBJECT
private:
    QString cronJob;            //cron-выражение
    QDateTime cronDate;         //дата вызова функции
    QPair<int, bool> minute,     //значение минут
                     hour,       //значение часов
                     dayOfMonth, //значение дней месяца
                     month,      //значение месяца
                     year,
                     dayOfWeek;  //значение дня недели
    bool isCalled;  //флаг, показывающий, что данный объект уже был вызван

    QPair<int, bool> parse(QString cronJob, int var, int minLimit, int maxLimit);   //метод обрабатывает cron-выражение и возвращает
                                                                                    //значение срабатывания для каждой единицы времени
    QDateTime calcTaskDate();   //метод парсит cron-выражение и рассчитывает ближайшую дату вызова функции

public:
    explicit CronParser(QObject *parent = 0);   //конструктор
    ~CronParser();  //деструктор

    QString getCronJob();   //метод возвращает cron-выражение
    QDateTime getDateTime(QString cron);    //метод возвращает время ближайшего вызова функции
    void setCall(bool var);

signals:

public slots:
};

#endif // CRONPARSER_H
