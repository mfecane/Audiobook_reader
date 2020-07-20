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

    Q_PROPERTY(bool isPlaying READ isPlaying NOTIFY playerStateChanged)
    Q_PROPERTY(qreal fileProgress READ fileProgress WRITE setFileProgress NOTIFY fileProgressChanged)
    Q_PROPERTY(qreal bookProgress READ bookProgress NOTIFY fileProgressChanged)

    Q_PROPERTY(QString rootPath READ rootPath WRITE setRootPath NOTIFY rootPathChanged)
    Q_PROPERTY(QString rootPathUrl READ rootPathUrl WRITE setRootPathUrl NOTIFY rootPathChanged)

    Q_PROPERTY(QString tempo READ tempo NOTIFY tempoChanged)
    Q_PROPERTY(qreal volume READ volume WRITE setVolume NOTIFY volumeChanged)

    Q_PROPERTY(QString currentTime READ currentTime NOTIFY fileProgressChanged)
    Q_PROPERTY(QString audioBookName READ audioBookName NOTIFY audioBookChanged)

    Q_PROPERTY(AudioBookList* audioBookList READ audioBookList NOTIFY rootPathChanged)
    Q_PROPERTY(AudioBook* audioBook READ audioBook NOTIFY audioBookChanged)

public:

    explicit BackEnd(QObject *parent = nullptr);
    static QObject *qmlInstance(QQmlEngine *engine, QJSEngine *scriptEngine);
    static BackEnd* getInstance();

 // <PROPERTIES>

    bool isPlaying();

    qreal fileProgress();
    void setFileProgress(qreal value);

    qreal bookProgress();

    QString rootPath();
    void setRootPath(QString value);
    QString rootPathUrl();
    void setRootPathUrl(QString url);

    QString tempo();

    QString currentTime();

    qreal volume();
    void setVolume(qreal value);

    QString audioBookName();

// </PROPERTIES>

    AudioBook* audioBook();
    void setAudioBook(QString path);

    AudioBookList* audioBookList();

    void setEngine(QQmlApplicationEngine* engine);

    void initAudioBooks();

signals:

    void isPlayingChanged();
    void fileProgressChanged();
    void rootPathChanged();
    void tempoChanged();
    void volumeChanged();
    void audioBookChanged();
    void playerStateChanged();

    void playerPlay();
    void playerPause();
    void playerKill();

    void playerVolume(qreal volume);
    void playerTempo(qreal tempo);
    void playerPosition(int pos);
    void playerJump(Player::Jump jump);
    void playerSetFile(QString filename);

public slots:

// <UI_COMMANDS>

    void play();
    void stop();
    void next();
    void prev();
    void jumpfwd();
    void jumpfwdx2();
    void jumpback();
    void jumpbackx2();
    void increaseTempo();
    void decreaseTempo();

// </UI_COMMANDS>

    void indexChangedSlot();
    void positionChangedSlot(int pos, int dur);
    void isPlayingSlot(QMediaPlayer::State state);
    void autoSave();
    void autoLoad();
    void onFinishedSlot();
    void playerStateChangedSlot(QMediaPlayer::State state);

private:
    
    bool loadJson();
    bool saveJson();
    void writeCurrentJson();
    void readCurrentJson(QString &savedFolder, int &savedIndex);
    void setupAutosave();
    void closeAudioBook();
    QString formatTime(int msec);
    void createPlayer();
    void resetPlayer();

    static BackEnd* m_instance;

    qint64 m_currentPosition = 0;
    qint64 m_currentDuration = 0;
    Player* m_player = nullptr;
    QThread* m_thread = nullptr;
    QTimer* m_autoSaveTimer = nullptr;
    QSettings m_settings;
    QJsonObject m_jsonRoot;
    QString m_jsonFileName = "save.json";
    QMutex muxJson;
    QString m_rootPath;
    QVector<qreal> m_tempoValues;

    qreal m_volume;
    int m_tempo;

    QQmlApplicationEngine* m_engine = nullptr;
    AudioBookList* m_audioBookList = nullptr;
    AudioBook* m_audiobook = nullptr;
    QMediaPlayer::State m_playerstate;
};
