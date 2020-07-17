#include <QGuiApplication>
#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <stdexcept>
#include <QIcon>

#include "backend.h"
#include "audiobookmodel.h"
#include "audiobooklistmodel.h"
#include "audiobook.h"

int main(int argc, char *argv[]) {

    QCoreApplication::setOrganizationName("Gavitka software");
    QCoreApplication::setOrganizationDomain("gavitka.com");
    QCoreApplication::setApplicationName("Diverse audiobook reader");

    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QApplication app(argc, argv);

    AudioBookList audioBookList("C:/Audiobook");

    qmlRegisterType<AudioBookModel>("io.qt.examples.audiobookmodel", 1, 0, "AudioBookModel");
    qmlRegisterType<AudioBookListModel>("io.qt.examples.booklistmodel", 1, 0, "BookListModel");
    qmlRegisterSingletonType<BackEnd>("io.qt.examples.backend", 1, 0, "BackEnd", &BackEnd::qmlInstance);

    QQmlApplicationEngine engine;

    engine.addImportPath(":/imports");

    BackEnd::getInstance()->setEngine(&engine);
    BackEnd::getInstance()->initAudioBooks();

    //engine.rootContext()->setContextProperty(QStringLiteral("bookList"), &audioBookList);

//    try {
//    AudioBook* audioBook = new AudioBook("C:/Audiobook/Abercrombie_Joe_-_Geroi_(Golovin_K)");
//    engine.rootContext()->setContextProperty(QStringLiteral("audioBook2"), audioBook);
//    }
//    catch(std::exception &e){
//        qDebug() << "this is not an audiobook, bro";
//    }

//    AudioBook* audioBook = new AudioBook("C:/Audiobook/Abercrombie_Joe_-_Geroi_(Golovin_K)");
//    engine.rootContext()->setContextProperty(QStringLiteral("ctxAudioBook"), audioBook);

    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    app.setWindowIcon(QIcon("qrc:/images/icon.ico"));

    return app.exec();
}
