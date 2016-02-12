#include <QCoreApplication>
#include <QString>
#include <QPair>
#include <QVector>
#include "MyScheduller.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QPair <QString, QString> pair("* 18 * * */4", "TaskName");
    QVector <QPair <QString, QString>> vector;
    vector.append(pair);
    MyScheduller sched(vector);

    return a.exec();
}
