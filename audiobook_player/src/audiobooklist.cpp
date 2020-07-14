#include "audiobooklist.h"
#include "audiobookinfo.h"
#include "backend.h"

#include <QDir>
#include <QFileInfoList>
#include <QDebug>
#include <stdexcept>

AudioBookList::AudioBookList(QString root, QString current) :
    m_root(root)
{
    QDir d(m_root);
    if(d.exists()) {
        d.setFilter(QDir::Dirs | QDir::NoDotAndDotDot);
        d.setSorting(QDir::Name);
        QFileInfoList list = d.entryInfoList();
        for(int i = 0; i < list.size(); ++i) {
            auto path = list.at(i).absoluteFilePath();
            try
            {
                auto ab = new AudioBookInfo(path, nullptr);
                if(current == path) m_index = i;
                m_list.append(ab);
            }
            catch(std::exception &e)
            {
                continue;
            }
        }
    }
}

void AudioBookList::setRootFolder(QString value) {
    m_root = value;
}

AudioBookInfo *AudioBookList::at(int i) {
    return m_list.at(i);
}

void AudioBookList::setIndex(int value) {
    m_index = value;
    QString path = m_list.at(m_index)->path();
    BackEnd::getInstance()->setAudioBook(path);
}

int AudioBookList::getIndex() {
    return m_index;
}

int AudioBookList::size() {
    return m_list.size();
}

void AudioBookList::checkIndexOf(QString path) {
    for(int i = 0; i < m_list.size(); ++i) {
        if(m_list.at(i)->path() == path) {
            qDebug() << "Audiobook found in list";
            m_index = i;
            emit indexChanged();
            // TODO: Notify model
        }
    }
}

