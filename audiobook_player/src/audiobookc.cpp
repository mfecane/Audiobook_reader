#include <QDir>
#include <QFileInfoList>
#include <QJsonObject>
#include <stdexcept>

#include "audiobookc.h"
#include "globaljson.h"

void load_audiobook_list(QString path)
{
    QDir d(path);
    if(d.exists()) {
        d.setFilter(QDir::Dirs | QDir::NoDotAndDotDot);
        d.setSorting(QDir::Name);
        QFileInfoList list = d.entryInfoList();
        for(int i = 0; i < list.size(); ++i) {
            auto path = list.at(i).absoluteFilePath();
            AudioBook ab;
            bool ret = load_audiobook(ab, path, true);
            if(ret == true) {
                audiobook_list.append(ab);
            }
            //if(current == path) m_index = i;
            audiobook_list.append(ab);
        }
    }
}

bool load_audiobook(AudioBook &audioBook, QString path, bool preload)
{
    QDir d(path);
    audioBook.path = path;
    d.setFilter(QDir::Files | QDir::NoDotAndDotDot);
    d.setSorting(QDir::Name);
    QStringList filters;
    filters << "*.mp3";
    d.setNameFilters(filters);
    QFileInfoList list =
            d.entryInfoList();
    if(preload) {
        if(list.size() == 0) return false;
    } else {
        for(int i = 0; i < list.size(); ++i) {
            const QFileInfo &fi = list.at(i);
            QString sname = fi.fileName();
            AudioBookFile abf;
            abf.path = sname;
            audioBook.fileList.append(abf);
        }
    }
    return true;
    audiobook_read_JSON(audioBook);
    audiobook_request_update_sizes();
}

void audiobook_read_JSON(AudioBookPreview &audioBook)
{
    throw std::logic_error("Not implamented exception");
}

void audiobook_request_update_sizes()
{
    throw std::logic_error("Not implamented exception");
}

qint64 audiobook_current_file_pos(AudioBook &audioBook)
{
    int index = audioBook.fileIndex;
    audioBook.fileList[index];
    return 0;
}

void audiobook_write_JSON(AudioBook &audioBook)
{
    QJsonObject bookObject;
    QString path = audioBook.path;
    bookObject["url"] = audioBook.path;
    bookObject["index"] = audioBook.fileIndex;
    bookObject["file_pos"] = 0;
    bookObject["size_before"] = 0;
    bookObject["size_total"] = 0;
    GlobalJSON::getInstance()->setBook(bookObject, path);
}
