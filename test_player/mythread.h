#ifndef MYTHREAD_H
#define MYTHREAD_H

#include "player.h"

class MyThread : public QThread
{
public:
    MyThread();

    void run();

    Player* m_player;
};

#endif // MYTHREAD_H
