#include "audiobookinfo.h"
#include <exception>
#include <QDir>

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

    m_progress = .5f; // Load from JSON
}

QString AudioBookInfo::name()
{
    return m_name;
}

float AudioBookInfo::progress()
{
    return m_progress;
}

QString AudioBookInfo::path()
{
    return m_path;
}
