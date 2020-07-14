#pragma once

#include <QAbstractListModel>
#include <QDir>
#include <QList>
#include <QObject>
#include <QUrl>

#include "player.h"
#include "audiobookfile.h"
#include "audiobookinfo.h"
#include "backend.h"

class AudioBook : public AudioBookInfo
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

    int index() {
        return m_index;
    }

    QString name() const {
        return m_name;
    }

    bool setCurrentFileIndex(int i);

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

    QString getPath() const;
    QString path() const;

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

    BackEnd *backEnd(); // BackEnd shortcut
    void setFileTime(QString fileName, qint64 pos, bool overwrite = true);
    AudioBookFile *findFile(QString fileName);

    QString m_name;
    QString m_path;
    QVector<AudioBookFile*> m_data;
    int m_index = 0;
    QString getFilePath(int i);
    qint64 m_progress;
    qint64 m_totaltime;
};











