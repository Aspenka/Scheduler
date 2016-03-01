#include <QCoreApplication>
#include <QString>
#include <QPair>
#include <QVector>
#include <QSettings>
#include "MyScheduller.h"

QVector <QPair<QString, QString>> loadData()
{
    QSettings settings("TaskList", QSettings::IniFormat);
    int size = settings.beginReadArray("TaskList");
    QVector<QPair<QString, QString>> vector;
    QPair<QString, QString> pair;

    for(int i=0; i<size; i++)
    {
        settings.setArrayIndex(i);

        pair.first = settings.value("cron").toString();
        pair.second = settings.value("task").toString();
        vector.append(pair);
    }
    settings.endArray();
    return vector;
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QVector <QPair <QString, QString>> vector;
    vector = loadData();

    MyScheduller shed(vector);
    shed.start();

    return a.exec();
}
