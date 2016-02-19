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
    appendNewTask(oneTask);
}

//добавить перечень новых заданий в список
void MyScheduller::append(TaskVector taskVector)
{
    taskVect = taskVector;
    for(int i=0; i<taskVect.size(); i++)
        appendNewTask(taskVect.at(i));
}

//расчет вектора срабатывания для одного задания
void MyScheduller::appendNewTask(TaskPair oneTask)
{   
    QDateTime date = parser.getDateTime(oneTask.first);

    std::cout << "[Scheduller]: adding new task: ";
    std::cout << date.toString("dd.MM.yy hh:mm").toStdString() << " " << oneTask.second.toStdString() << "\n";

    TaskTime pair(date, oneTask);
    nextTasks.append(pair);                             //добавление дат срабатывания для задания к вектору срабатывания
}

//удалить одно задание из списка
void MyScheduller::removeOne(TaskTime task)
{
    nextTasks.removeOne(task);
}

//удалить все задания, содержащие cron-выражение и имя таска, переданных в параметре
void MyScheduller::remove(TaskPair oneTask)
{
    for(int i=0; i<nextTasks.size(); i++)
    {
        if(nextTasks.at(i).second == oneTask)
            removeOne(nextTasks.at(i));
    }
}

//очистить вектор срабатываний
void MyScheduller::clear()
{
    nextTasks.clear();
}

//метод подсчитывает время до следующего срабатывания таймера в миллисекундах
//ДОДУМАТЬ!!!
int MyScheduller::calcTimeout(TaskTime oneTask)
{
    //int msec = (QDateTime::currentDateTime().toTime_t() - oneTask.first.toTime_t())*100;
    int msec = (QDateTime::currentDateTime().toTime_t() - oneTask.first.toTime_t())*100;
    //qDebug() << int(QDateTime::currentDateTime().toTime_t());
    qDebug() << msec/(1000*60);
    qDebug() << msec;
    if(msec <= INT_MAX)
        return msec;
}

//метод запускает планировщик
void MyScheduller::startSheduller()
{
    std::cout << "[Scheduller]: Starting...\n";
    timer = new Timer[nextTasks.size()];
    for(int i=0; i<nextTasks.size(); i++)
    {
        int interval = calcTimeout(nextTasks.at(i));
        if(interval >= 0)
        {
            timer[i].setIndex(i);
            timer[i].start(interval);
            connect(&timer[i], SIGNAL(timeout()), &timer[i], SLOT(sentTimerIndex()));
            connect(&timer[i], SIGNAL(sentIndex(int)), this, SLOT(slotReaction(int)));
        }
    }
}

//метод вызывает процедуры с нужным именем по таймауту таймера
void MyScheduller::slotReaction(int ind)
{
    timer[ind].stop();
    QDateTime curDate = QDateTime::currentDateTime();
    for(int i=0; i < nextTasks.size(); i++)
    {
        if((curDate.date().month() == nextTasks.at(i).first.date().month())&&
          (curDate.date().day() == nextTasks.at(i).first.date().day())&&
          (curDate.date().dayOfWeek() == nextTasks.at(i).first.date().dayOfWeek())&&
          (curDate.time().minute() == nextTasks.at(i).first.time().minute())&&
          (curDate.time().hour() == nextTasks.at(i).first.time().hour()))
        {
            qDebug() << "[Scheduller" << nextTasks.at(i).first.toString("dd/MM/yy hh:mm") << "]:    " << "Call " << nextTasks.at(i).second.second;
            emit timeOut(nextTasks.at(i).second.second);
        }
    }
}

//метод рассчитывает новое время срабатывания для заданий
void MyScheduller::slotUpdateTasks(int index)
{
    //nextTasks.clear();
    //for(int i=0; i<oneTask.length(); i++)
        //append(oneTask.at(i));

}

//деструктор
MyScheduller::~MyScheduller()
{
    delete timer;
}

