#include <QCoreApplication>
#include <QString>
#include <QPair>
#include <QVector>
#include <QDebug>
#include "MyScheduller.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QPair <QString, QString> pair1("* 13 *  */5", "Task1");
    QPair <QString, QString> pair2("* */2 10-14 * *", "Task2");
    QPair <QString, QString> pair3("7,12,20 * * * 5 ", "Task3");
    QVector <QPair <QString, QString>> vector;
    vector.append(pair1);
    vector.append(pair2);
    vector.append(pair3);
    MyScheduller shed(vector);

    return a.exec();
}
