#include "audiobookinfo.h"
#include <exception>
#include <QDir>
#include "globaljson.h"

AudioBookInfo::AudioBookInfo(QString path, QObject* parent):
    QObject(parent)
{
    m_path = path;
    QDir d(path);
    if(d.dirName().length() == 0) throw std::exception("Not an audiobook");
    m_name = d.dirName();

    d.setFilter(QDir::Files | QDir::NoDotAndDotDot);
    d.setSorting(QDir::Name);
    QStringList filters;
    filters << "*.mp3";
    d.setNameFilters(filters);
    QFileInfoList list = d.entryInfoList();
    if(list.size() <= 0) throw std::exception("Not an audiobook");

    readJson();// Load from JSON
}

void AudioBookInfo::readJson() {
    QJsonObject bookObject = GlobalJSON::getInstance()->getBook(m_path);
    if(bookObject.contains("index") && bookObject["index"].isDouble()) {
        setIndex(bookObject["index"].toInt());
    }
    if(bookObject.contains("size_before") && bookObject["size_before"].isDouble()) {
        int val = bookObject["size_before"].toInt();
        if(val > 0) m_sizeBefore = val;
    }
    if(bookObject.contains("size_total") && bookObject["size_total"].isDouble()) {
        int val = bookObject["size_total"].toInt();
        if(val > m_sizeBefore) m_sizeTotal = val;
    }
    if(bookObject.contains("file_pos") && bookObject["file_pos"].isDouble()) {
        int val = bookObject["file_pos"].toInt();
        if(val > 0) m_currentFilePos = val; // TODO: use setter
    }
}

void AudioBookInfo::setIndex(int i)
{
    return;
}

QString AudioBookInfo::name()
{
    return m_name;
}

qreal AudioBookInfo::progress() {
    if(m_sizeTotal > 0) {
        return (qreal)(m_sizeBefore + m_currentFilePos) / m_sizeTotal;
    }
    else return 0;
}

QString AudioBookInfo::path()
{
    return m_path;
}
