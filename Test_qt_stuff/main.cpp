#include <QCoreApplication>

#include <QAudioOutput>

#include "player.h"
//#include "player_file.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    Player player;
    //player.setFile("C:/dev/file.mp3");

    return a.exec();
}
