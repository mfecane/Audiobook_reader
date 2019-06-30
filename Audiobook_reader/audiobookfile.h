#pragma once
#include <QAudioDecoder>
#include <QMediaPlayer>
#include <QObject>

class AudioBookFile : public QObject
{
    Q_OBJECT

public:

    explicit AudioBookFile(QString sname = "", QObject *parent = nullptr);

    QString fileName() const{
        return m_fileName;
    }

    qint64 size() const{
        return m_size;
    }

    void setSize(qint64 value) {
        m_size = value;
    }

    qint64 pos() const{
        return m_pos;
    }

    void setPos(qint64 value) {
        m_pos = value;
    }

    void setDecoder(QAudioDecoder* dec) {
        m_decoder = dec;
    }

    void setPlayer(QMediaPlayer* value) {
        m_player = value;
    }

public slots:

    void metaDataChanged(bool b);

private:

    QString m_fileName;
    qint64 m_size;
    qint64 m_pos;

    QAudioDecoder* m_decoder;
    QMediaPlayer* m_player;

};
