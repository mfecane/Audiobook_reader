#include "pch.h"

#include <QtCore>

#include "player2.h"

class Task : public QObject
{
    Q_OBJECT
public:
    Task(QObject *parent = 0) : QObject(parent) {}

public slots:
    void run()
    {
        player2 = new Player2();
        player2->setFile("D:/Audiobooks/Abercrombie_Joe_-_Prezde_chem_ux_povecyat_(Golovin_K)/01_03.mp3");
        connect(player2, &Player2::finished, this, &Task::finishedSlot);
    }

public slots:
    void finishedSlot() {
        emit finished();
    }

signals:
    void finished();

private:
    Player2* player2;
};

#include "main.moc"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    // Task parented to the application so that it
    // will be deleted by the application.
    Task *task = new Task(&a);

    // This will cause the application to exit when
    // the task signals finished.
    QObject::connect(task, SIGNAL(finished()), &a, SLOT(quit()));

    // This will run the task from the application event loop.
    QTimer::singleShot(0, task, SLOT(run()));

    return a.exec();
}
