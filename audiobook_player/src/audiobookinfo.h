#pragma once

#include <QString>
#include <QObject>

class AudioBookInfo : public QObject
{

    Q_OBJECT

public:

    AudioBookInfo(QString path, QObject* parent);

    QString name();
    float progress();
    QString path();

private:


    QString m_path;
    QString m_name;
    float m_progress;

};
