#pragma once

#include <QDebug>
#include <QString>
#include <QVector>

struct AudioBookFile{
    QString path;
    qint64 size = 0;
};

struct AudioBookPreview {
    QString path;
    int size;
};

struct AudioBook: AudioBookPreview {
    QVector<AudioBookFile> fileList;
    int fileIndex;
};

QVector<AudioBook> audiobook_list;
AudioBook currentAudioBook;

void load_audiobook_list(QString root_path);

bool load_audiobook(AudioBook &audioBook, QString path, bool preload = true);

qint64 audiobook_current_file_pos(AudioBook &audioBook);

void audiobook_read_JSON(AudioBookPreview &audioBook);
void audiobook_write_JSON(AudioBook &audioBook);

void audiobook_request_update_sizes();
