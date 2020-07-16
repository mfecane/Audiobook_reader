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
#include <QQmlApplicationEngine>

#include "audiobooklist.h"
#include "player.h"

class BookFile;
class AudioBook;

QString int2str(int i);

class BackEnd : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool isPlaying READ isPlaying NOTIFY isPlayingChanged)
    Q_PROPERTY(qreal fileProgress READ fileProgress WRITE setFileProgress NOTIFY fileProgressChanged)
    Q_PROPERTY(qreal bookProgress READ bookProgress NOTIFY fileProgressChanged)

    Q_PROPERTY(QString rootPath READ rootPath WRITE setRootPath NOTIFY rootPathChanged)
    Q_PROPERTY(QString rootPathUrl READ rootPathUrl WRITE setRootPathUrl NOTIFY rootPathChanged)

    Q_PROPERTY(QString tempo READ tempo NOTIFY tempoChanged)
    Q_PROPERTY(qreal volume READ volume WRITE setVolume NOTIFY volumeChanged)

    Q_PROPERTY(QString currentTime READ currentTime NOTIFY fileProgressChanged)

    Q_PROPERTY(AudioBookList* audioBookList READ audioBookList NOTIFY rootPathChanged)
    Q_PROPERTY(AudioBook* audioBook READ audioBook NOTIFY audioBookChanged)

public:

    explicit BackEnd(QObject *parent = nullptr);

    static QObject *qmlInstance(QQmlEngine *engine, QJSEngine *scriptEngine);

    static BackEnd* getInstance();

    QString FileName() {return m_currentFileName;}
    bool isPlaying();

    qreal fileProgress();
    void setFileProgress(qreal value);

    qreal bookProgress();

    QString rootPath();
    void setRootPath(QString value);
    QString rootPathUrl();
    void setRootPathUrl(QString url);

    QList<QObject*> bookFileList();

    int playlistIndex();

    void setPlaylistIndex(int value);
    void setPlaylistFile(QString fileName);

    AudioBook* audioBook();
    void setAudioBook(QString path);

    AudioBookList* audioBookList();

    QString tempo();

    QString currentTime();
    
    QString formatTime(int msec);

    qreal volume();
    void setVolume(qreal value);

    void setEngine(QQmlApplicationEngine* engine);
    void initAudioBooks();

signals:

    void currentFolderChanged();
    void isPlayingChanged();
    void fileProgressChanged();
    void rootPathChanged();
    void bookFileListChanged();
    void tempoChanged();
    void volumeChanged();
    void audioBookChanged();

public slots:

    void closeBookFolder();
    void play();
    void stop();
    void next();
    void prev();
    void jumpForeward(int sec);
    void jumpBack(int sec);
    void indexChangedSlot();
    void positionChangedSlot(int pos);
    void isPlayingSlot(QMediaPlayer::State state);
    void autoSave();
    void autoLoad();
    void onFinishedSlot();
    void increaseTempo();
    void decreaseTempo();

private:
    
    bool loadJson();
    bool saveJson();
    void writeCurrentJson();
    void readCurrentJson(QString &savedFolder, int &savedIndex);
    void setupAutosave();

    static BackEnd* m_instance;

    QString m_currentFolder;
    QString m_currentFileName;
    qint64 m_currentPos;
    Player m_player;
    QTimer* m_autoSaveTimer;
    QSettings m_settings;
    QJsonObject m_jsonRoot;
    QString m_jsonFileName = "save.json";
    QMutex muxJson;
    QString m_rootPath;
    QList<QObject*> m_bookFileList;
    int m_currentBookFile;
    void updatePlayer();
    void closeAudioBook();
    QVector<qreal> m_tempoValues;

    qreal m_volume;
    int m_tempo;

    QQmlApplicationEngine* m_engine = nullptr;
    AudioBookList* m_audioBookList;
    AudioBook* m_audiobook = nullptr;
};
