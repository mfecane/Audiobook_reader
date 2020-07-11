#include "bookfile.h"

BookFile::BookFile(QString fileName, QObject* parent) :
    QObject(parent),
    m_fileName(fileName)
{

}

QString BookFile::fileName() const
{
    return m_fileName;
}

void BookFile::setFileName(const QString &fileName)
{
    m_fileName = fileName;
}

int BookFile::size() const
{
    return m_size;
}

void BookFile::setSize(int size)
{
    m_size = size;
}

int BookFile::pos() const
{
    return m_pos;
}

void BookFile::setPos(int pos)
{
    m_pos = pos;
}
