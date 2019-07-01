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

#include "audiobooklist.h"
#include "player.h"

class BookFile;
class AudioBook;

class AudioBookFileList : public QAbstractListModel {

    Q_OBJECT

public:

    enum BookFileRoles {
        TextRole = Qt::UserRole +1,
        progressRole
    };

    AudioBookFileList(AudioBook *audiobook);

    QHash<int, QByteArray> roleNames() const;

    int rowCount(const QModelIndex &parent = QModelIndex()) const;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

    void setAudiobook(AudioBook* value) {
        m_audiobook = value;
    }

public slots:

    void playlistItemChanged();
    void playlistChanged();

private:

    AudioBook* m_audiobook;
};

QString int2str(int i);

class BackEnd : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool isPlaying READ isPlaying NOTIFY isPlayingChanged)
    Q_PROPERTY(qreal fileProgress READ fileProgress WRITE setFileProgress NOTIFY fileProgressChanged)
    Q_PROPERTY(qreal bookProgress READ bookProgress NOTIFY fileProgressChanged)

    Q_PROPERTY(QString rootPath READ rootPath WRITE setRootPath NOTIFY rootPathChanged)
    Q_PROPERTY(QString rootPathUrl READ rootPathUrl WRITE setRootPathUrl NOTIFY rootPathChanged)

    Q_PROPERTY(AudioBookFileList* playlist READ playlist NOTIFY playlistChanged)
    Q_PROPERTY(int playlistIndex READ playlistIndex WRITE setPlaylistIndex NOTIFY playListIndexChanged)

    Q_PROPERTY(AudioBookList* audioBookList READ audioBookList NOTIFY audioBookListChanged)
    Q_PROPERTY(int audioBookListIndex READ audioBookListIndex WRITE setAudioBookListIndex NOTIFY audioBookListIndexChanged)

    Q_PROPERTY(QString tempo READ tempo NOTIFY tempoChanged)
    Q_PROPERTY(qreal volume READ volume WRITE setVolume NOTIFY volumeChanged)

    Q_PROPERTY(QString currentTime READ currentTime NOTIFY fileProgressChanged)

public:
    explicit BackEnd(QObject *parent = nullptr);

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

    QString FileName() {return m_currentFileName;}

    QUrl getFileUrl() {
        QDir d;
        QUrl u = QUrl::fromLocalFile(m_currentFolder + d.separator() + m_currentFileName);
        qDebug() << "getFileUrl" << u ;
        return u;
    }

    bool isPlaying() {
        return m_player.state() == QMediaPlayer::PlayingState;
    }

    qreal fileProgress();
    void setFileProgress(qreal value);

    qreal bookProgress() {
        if(m_audiobook != nullptr) {
            return m_audiobook->progress();
        }
    }

    QString rootPath() {
        return m_rootPath;
    }

    void setRootPath(QString value);

    QString rootPathUrl();

    void setRootPathUrl(QString url) {
        QUrl u = QUrl(url);
        setRootPath(u.toLocalFile());
    }

    QList<QObject*> bookFileList() {
        return m_bookFileList;
    }

    int playlistIndex() {
        if(m_audiobook == nullptr) return 0;
        return m_audiobook->index();
    }

    void setPlaylistIndex(int value);
    void setPlaylistFile(QString fileName);

    AudioBookFileList* playlist() {
        if(m_audiobook == nullptr) {
            return nullptr;
        }
        return m_playlist;
    }

    AudioBook* audioBook() {
        return m_audiobook;
    }

    void openAudioBook(QString folder);

    AudioBookList* audioBookList() {
        return m_audioBookList;
    }

    int audioBookListIndex() {
        return m_audioBookList->getIndex();
    }

    void setAudioBookListIndex(int value);

    QString tempo() {
        qreal value = m_tempoValues.at(m_tempo);
        return QString::number(value, 'f', 2);
    }

    QString currentTime();
    
    QString formatTime(int msec);

    qreal volume() {
        return m_volume;
    }

    void setVolume(qreal value) {
        m_volume = value;
        emit volumeChanged();
    }
    
signals:

    void currentFolderChanged();
    void isPlayingChanged();
    void fileProgressChanged();
    void rootPathChanged();
    void bookFileListChanged();
    void audioBookListChanged();
    void playListIndexChanged();
    void playlistChanged();
    void playlistItemChanged();
    void tempoChanged();
    void audioBookListIndexChanged();
    void volumeChanged();

public slots:

    void closeBookFolder();
    void play();
    void stop();
    void next();
    void prev();
    void jumpForeward(int sec);
    void jumpBack(int sec);
    void speedUp();
    void positionChangedSlot(int pos);
    void isPlayingSlot(QMediaPlayer::State state);
    void autoSave();
    void autoLoad();
    void onFinishedSlot();
    void setCurrentAudiobookIndex(int i);
    void increaseTempo();
    void decreaseTempo();

private:
    
    bool loadJson();
    bool saveJson();
    void writeCurrentJson();
    void readCurrentJson(QString &savedFolder, QString &savedFile);
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
    AudioBook* m_audiobook;
    AudioBookFileList* m_playlist;
    void updatePlayer();
    void closeAudioBook();
    AudioBookList* m_audioBookList;
    int m_tempo;
    QVector<qreal> m_tempoValues;
    qreal m_volume;

};
