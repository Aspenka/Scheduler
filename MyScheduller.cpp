#include <iostream>
#include "MyScheduller.h"

//пустой конструктор
MyScheduller::MyScheduller(QObject *parent) : QObject(parent)
{

}

//конструктор, принимающий одно задание
MyScheduller::MyScheduller(TaskPair oneTask, QObject *parent) : QObject(parent)
{
    append(oneTask);
}

//конструктор, принимающий перечень заданий
MyScheduller::MyScheduller(TaskVector taskVector, QObject *parent) : QObject(parent)
{
    append(taskVector);
}

//добавить новое задание в список
void MyScheduller::append(TaskPair oneTask)
{
    taskVect.append(oneTask);
}

//добавить перечень новых заданий в список
void MyScheduller::append(TaskVector taskVector)
{
    taskVect = taskVector;
}

//удалить все задания, содержащие cron-выражение и имя таска, переданных в параметре
void MyScheduller::remove(TaskPair oneTask)
{
    taskVect.removeOne(oneTask);
}

void MyScheduller::remove(int index)
{
    taskVect.remove(index,1);
}

//очистить вектор срабатываний
void MyScheduller::clear()
{
    taskVect.clear();
}

//метод запускает планировщик
void MyScheduller::start()
{
    std::cout << "[Scheduller]: Starting...\n";
    for(int i=0; i<taskVect.size(); i++)
    {
        timer = new Timer();
        //timer->setSingleShot(true);
        connect(timer, SIGNAL(timeout(QPair<QString,QString>)), this, SLOT(slotReaction(QPair<QString,QString>)));
        timer->start(taskVect.at(i));
    }
}

//метод вызывает процедуры с нужным именем по таймауту таймера
void MyScheduller::slotReaction(TaskPair task)
{
    std::cout << "[" << QDateTime::currentDateTime().toString("dd/MM/yy hh:mm:ss").toStdString();
    std::cout << "]: Call " << task.second.toStdString() << std::endl;
    emit callTask(task.second);
}

//деструктор
MyScheduller::~MyScheduller()
{

}

