#pragma once

#include <QObject>
#include <QMediaPlayer>
#include <QMediaMetaData>

class FileSizeRequest : public QObject
{
    Q_OBJECT
public:
    explicit FileSizeRequest(int i, QString path, QObject *parent = nullptr);

signals:

    void metaDataChanged(int index, qint64 size);

public slots:

    void metaDataChangedSlot(bool v);

private:

    int m_index;
    int size;
    QString m_path;

    QMediaPlayer* m_player;

};
