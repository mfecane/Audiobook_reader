#include <QAudioDeviceInfo>
#include <QDir>

#include "book.h"
#include "bookfile.h"

Book::Book(QString path, QObject *parent) :
    QObject(parent)
{
    QAudioDeviceInfo device = QAudioDeviceInfo::defaultOutputDevice();
    QAudioFormat format = device.preferredFormat();
    m_path = path;
    QDir d(path);
    d.setFilter(QDir::Files | QDir::NoDotAndDotDot);
    d.setSorting(QDir::Name);
    QStringList filters;
    filters << "*.mp3";
    d.setNameFilters(filters);
    QFileInfoList list = d.entryInfoList();
    for(int i = 0; i < list.size(); ++i) {
        const QFileInfo &fi = list.at(i);
        QString sname = fi.fileName();
        BookFile* bf = new BookFile(sname);
        m_list.append(bf);
    }
    m_index = 0;
    //readJson();
}

Book *Book::createbook(QString path, QObject *parent)
{
    QDir d(path);
    if(d.exists()) {
        d.setFilter(QDir::Files | QDir::NoDotAndDotDot);
        d.setSorting(QDir::Name);
        QStringList filters;
        filters << "*.mp3";
        QFileInfoList list = d.entryInfoList();
        if(list.size() > 0) {
            return new Book(path, parent);
        }
    }
    return nullptr;
}

QString Book::folder() const
{
    return m_folder;
}

void Book::setFolder(const QString &value)
{
    m_folder = value;
}

int Book::progress() const
{
    return m_progress;
}

void Book::setProgress(int value)
{
    m_progress = value;
}
