#pragma once

#include <QCoreApplication>
#include "Player2.h"

//#include "player.h"

//extern "C" {
//#include "SoundTouchDLL.h"
//}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

//    Player player;
//    player.init();
//    player.play();

    Player2 player;


    return a.exec();
}
