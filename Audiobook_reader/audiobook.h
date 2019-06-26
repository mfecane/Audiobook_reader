#pragma once

#include <QAbstractListModel>
#include <QDir>
#include <QList>
#include <QObject>
#include <QUrl>

#include "player.h"

class BackEnd;

struct AudioBookFile {
public:
    AudioBookFile(QString s = "") :
        fileName(s),
        size (0),
        pos (0)
    { }
    QString fileName;
    qint64 size;
    qint64 pos;
};

class AudioBook
{
public:
    AudioBook(QString path);
    
    void openBook();
    void closeBook();
    
    void readJson();
    void writeJson();

    int fileCount() {
        return m_data.size();
    }

    bool setCurrentFileIdex(int i) {
        if(m_currentFileIndex == i) {
            return false;
        }
        m_currentFileIndex = i;
        return true;
    }

    const AudioBookFile getCurrentFile() {
        return fileAt(m_currentFileIndex);
    }

    const QString getCurrentFilePath() {
        QDir d;
        QString path = m_path + d.separator() + getCurrentFile().fileName;
        QFile f(path);
        if(f.exists()) {
            return path;
        } else {
            return QString("");
        }
    }

    const QUrl getCurrentFilePathUrl() {
        QDir d;
        QUrl u = QUrl::fromLocalFile(getCurrentFilePath());
        return u;
    }

    qint64 getCurrentFilePos() {
        return getCurrentFile().pos;
    }

    const AudioBookFile fileAt(int i){
        return m_data.at(i);
    }

    void setCurrentPos(qint64 i) {
        m_data[m_currentFileIndex].pos = i;
    }

    void setNext() {
        if(m_currentFileIndex >= m_data.size()) return;
        ++m_currentFileIndex;
    }

    void setPrevious() {
        if(m_currentFileIndex <= 0) return;
        --m_currentFileIndex;
    }

private:

    void setFileTime(QString fileName, qint64 pos);

    BackEnd *backEnd();

    QString m_path;
    int m_currentFileIndex = -1;
    QVector<AudioBookFile> m_data;
    Player m_player;
};











