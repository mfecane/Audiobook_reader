#pragma once

#include <QAbstractListModel>
#include <QDir>
#include <QList>
#include <QObject>
#include <QUrl>

#include "player.h"
#include "audiobookfile.h"

class BackEnd;

class AudioBook : public QObject
{
    Q_OBJECT

public:

    static AudioBook* createAudiobok(QString path, QObject* parent = nullptr);

    AudioBook(QString path, QObject* parent = nullptr);
    
    void openBook();
    void closeBook();
    
    void readJson();
    void writeJson();

    int fileCount() {
        return m_data.size();
    }

    int index() {
        return m_index;
    }

    bool setCurrentFileIdex(int i);

    bool setCurrentFileName(QString filename);

    const AudioBookFile* getCurrentFile();
    QString getCurrentFilePath();
    qint64 getCurrentFilePos() {
        getCurrentFile()->pos();
    }

    const AudioBookFile* fileAt(int i){
        return m_data.at(i);
    }

    qreal progressOf(int i);

    void setCurrentPos(qint64 i) {
        m_data[m_index]->setPos(i);
    }

    bool setNext();
    bool setPrevious();

    QString getPath() const {
        return m_path;
    }

    QString folderName() const {
        QFileInfo fi(m_path);
        return fi.fileName();
    }

    qreal progress();

    int progressInt();

    int totaltime();

    void readFileSizes();

    int size() {
        return m_data.size();
    }

private:

    void setFileTime(QString fileName, qint64 pos, bool overwrite = true);

    BackEnd *backEnd();

    QString m_path;
    int m_index;
    QVector<AudioBookFile*> m_data;
    AudioBookFile *findFile(QString fileName);
    QString getFilePath(int i);
    qint64 m_progress;
    qint64 m_totaltime;
};











