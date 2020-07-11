#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "backend.h"
#include "booklistmodel.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setOrganizationName("Gavitka software");
    QCoreApplication::setOrganizationDomain("gavitka.com");
    QCoreApplication::setApplicationName("Diversity recording");

    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);

    BookList bookList("C:/Audiobook");

    qmlRegisterType<BookListModel>("io.qt.examples.booklistmodel",
                                       1, 0, "BookListModel");
    qmlRegisterSingletonType<BackEnd>("io.qt.examples.backend",
                                      1, 0, "BackEnd", &BackEnd::qmlInstance);

    QQmlApplicationEngine engine;

    engine.addImportPath(":/imports");

    engine.rootContext()->setContextProperty(QStringLiteral("bookList"),
                                             &bookList);

    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
