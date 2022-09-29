#include "lib.h"

#include <QDir>

QString filepath(QString path, QString filename) {
    QDir d(path + QDir::separator() + filename);
    return d.absolutePath();
}

QString format_time(qint64 msec)
{
    qint64 t;
    int sec, min, hour;
    t = msec;
    sec = floor(t / 1000);
    min = floor(sec / 60);
    sec = sec % 60;
    hour = floor(min / 60);
    min = min % 60;
    return QString("%1:%2:%3").arg(int00(hour)).arg(int00(min)).arg(int00(sec));
}

QString int00(int i)
{
    QString s;
    s = QString::number(i);
    if(s.length() < 2) {
        s = QString("0") + s.right(1);
        return s;
    } else {
        return s;
    }
}
