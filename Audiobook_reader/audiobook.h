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

class AudioBook : public QObject
{
    Q_OBJECT

public:
    AudioBook(QString path, QObject* parent = nullptr);
    
    void openBook();
    void closeBook();
    
    void readJson();
    void writeJson();

    int fileCount() {
        return m_data.size();
    }

    bool setCurrentFileIdex(int i);

    bool setCurrentFileName (QString filename);

    AudioBookFile getCurrentFile();
    QString getCurrentFilePath();
    qint64 getCurrentFilePos() {
        getCurrentFile().pos;
    }

    const AudioBookFile fileAt(int i){
        return m_data.at(i);
    }

    void setCurrentPos(qint64 i) {
        m_data[m_index].pos = i;
    }

    bool setNext();
    bool setPrevious();

    QString getPath() {
        return m_path;
    }

private:

    void setFileTime(QString fileName, qint64 pos);

    BackEnd *backEnd();

    QString m_path;
    int m_index;
    QVector<AudioBookFile> m_data;
};











