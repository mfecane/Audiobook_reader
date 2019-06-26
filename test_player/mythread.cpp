#include "mythread.h"

MyThread::MyThread() {

}

void MyThread::run() {
    m_player = new Player();
    while(true) {
        QThread::msleep(100);
    }
}
