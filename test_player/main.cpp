#include <QCoreApplication>
#include "mythread.h"
#include "player.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

//    MyThread thr;
//    thr.start();
    Player* player = new Player();

    QTextStream stream(stdin);
    QString line;
//    while (stream.readLineInto(&line)) {
//        if(line == "fwd") {
//            thr.m_player->back();
//        }


//        if(line == "bck") {
//            thr.m_player->fwd();
//        }
//        QThread::msleep(50);
//    }

    return a.exec();
}
