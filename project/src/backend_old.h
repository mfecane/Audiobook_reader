#pragma once

#include <QObject>
#include <QQmlEngine>
#include <QFileSystemModel>
#include <QDebug>
#include <QUrl>
#include <QMediaPlayer>
#include <QTimer>
#include <QSettings>
#include <QJsonObject>
#include <QMutex>

class BookFile;

typedef QPair<QString, qint64> FileTimePair;

class BackEnd : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString currentFolderUrl READ currentFolderUrl NOTIFY currentFolderChanged)
    Q_PROPERTY(bool isPlaying READ isPlaying NOTIFY isPlayingChanged)
    Q_PROPERTY(qreal fileProgress READ fileProgress NOTIFY fileProgressChanged)
    Q_PROPERTY(QString rootPath READ rootPath WRITE setRootPath NOTIFY rootPathChanged)
    Q_PROPERTY(QString rootPathUrl READ rootPathUrl WRITE setRootPathUrl NOTIFY rootPathChanged)
    Q_PROPERTY(QList<QObject*> bookFileList READ bookFileList NOTIFY bookFileListChanged)
    Q_PROPERTY(int currentBookFile READ currentBookFile WRITE setCurrentBookFile NOTIFY currentBookFileChanged)

public:
    explicit BackEnd(QObject *parent = nullptr);
    ~BackEnd();

    static QObject *qmlInstance(QQmlEngine *engine, QJSEngine *scriptEngine) {
        Q_UNUSED(engine);
        Q_UNUSED(scriptEngine);
        return (QObject*)getInstance();
    }

    static BackEnd* getInstance() {
        if(!m_instance) {
            m_instance = new BackEnd();
        }
        return m_instance;
    }

    QString currentFolder() {
        return m_currentFolder;
    }
    QString currentFolderUrl() {
        QUrl u = QUrl::fromLocalFile(currentFolder());
        QString s = u.url();
        return s;
    }
    void setCurrentFolder(QString value) {
        QDir d = QDir(value);
        if(d.exists()) {
            m_currentFolder = d.absolutePath();
        }
        emit currentFolderChanged();
    }

    void setFileName(QString value);
    QString FileName() {return m_currentFileName;}

    QUrl getFileUrl() {
        QDir d;
        QUrl u = QUrl::fromLocalFile(m_currentFolder + d.separator() + m_currentFileName);
        qDebug() << "getFileUrl" << u ;
        return u;
    }
    void setFileUrl(QUrl u) {
        QFile f(u.toLocalFile());
        QString s = f.fileName();
        setFileName(s);
    }

    bool isPlaying() {
        return m_player.state() == QMediaPlayer::PlayingState;
    }

    qreal fileProgress() {
        if(m_player.duration() != 0) {
            qreal r;
            return r;
        } else {
            return 0;
        }
    }

    void loadCurrentPos() {
        m_currentPos = 0;
        for(const QPair<QString, qint64>& p : m_fileTimes) {
            if(p.first == m_currentFileName) {
                m_currentPos = p.second;
            }
        }
    }
    void saveCurrentPos() {
        bool success = false;
        for (int i = 0; i < m_fileTimes.size(); ++i) {
            if (m_fileTimes[i].first == m_currentFileName) {
                m_fileTimes[i].second = m_currentPos;
                success = true;
            }
        }
        if(!success) {
            m_fileTimes.append(FileTimePair(m_currentFileName, m_currentPos));
        }
    }

    QString rootPath() {
        return m_rootPath;
    }
    void setRootPath(QString value) {
        m_rootPath = value;
        m_settings.setValue("rootPath", value);
        emit rootPathChanged();
    }

    QString rootPathUrl() {
        QUrl u;
        QDir d(m_rootPath);
        u = u.fromLocalFile(d.absolutePath());
        QString s = u.toString();
        return s;
    }
    void setRootPathUrl(QString url) {
        QUrl u = QUrl(url);
        setRootPath(u.toLocalFile());
    }

    QList<QObject*> bookFileList() {
        return m_bookFileList;
    }

    int currentBookFile() {
        return m_currentBookFile;
    }
    void setCurrentBookFile(int value);

signals:

    void currentFolderChanged();
    void isPlayingChanged();
    void fileProgressChanged();
    void rootPathChanged();
    void bookFileListChanged();
    void currentBookFileChanged();

public slots:

    void openBookFolder(QString value);
    void closeBookFolder();

    void play();
    void stop();
    void setFileSlot(QString value) {
        QFileInfo fi(value);
        if(fi.exists()) {
            QString s = fi.fileName();
            setFileName(s);
        }
    }
    void positionChangedSlot(qint64 pos){
        Q_UNUSED(pos)
        emit fileProgressChanged();
    }
    void isPlayingSlot(QMediaPlayer::State state) {
        Q_UNUSED(state)
        emit isPlayingChanged();
    }
    void autoSave();

private:
    
    bool loadJson();
    bool saveJson();

    void writeBookJson(QJsonObject &bookObject);
    void readBookJson(const QJsonObject &bookObject);
    void writeFileJson(QJsonObject &fileObject);
    void readFileJson(const QJsonObject &fileObject);
    void writeBookArrayJson();
    void readBookArrayJson();
    void writeCurrentJson();
    void readCurrentJson();
    void fillBookFileList();

    static BackEnd* m_instance;
    QString m_currentFolder;
    QString m_currentFileName;
    qint64 m_currentPos;
    QMediaPlayer m_player;
    QTimer* m_autoSaveTimer;
    QSettings m_settings;
    QJsonObject m_jsonRoot;
    QString m_jsonFileName = "save.json";
    QMutex muxJson;
    QVector<FileTimePair> m_fileTimes;
    QString m_rootPath;
    QList<QObject*> m_bookFileList;
    int m_currentBookFile;
};

class BookFile : public QObject {
    Q_OBJECT

    Q_PROPERTY(QString name READ name NOTIFY nameChanged)

public:
    BookFile(QString s, qint64 i) : QObject() {
        m_fileName = s;
        m_size = i;
    }

    QString name(){
        return m_fileName;
    }

signals:

    void nameChanged();

private:

    QString m_fileName;
    qint64 m_size;

};
