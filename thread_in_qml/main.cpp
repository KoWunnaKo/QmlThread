#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QtQml>

#include <qmlthread/qmlthread.h>
#include <utility/dirsize.h>

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    // only for xxx.thread.qml
    qmlRegisterType<qyvlik::QmlRunnable>("qyvlik.thread.runnable", 0, 1, "QmlRunnable");
    qmlRegisterType<qyvlik::QmlThread>("qyvlik.thread", 0, 1, "QmlThread");

    qmlRegisterType<qyvlik::DirSize>("qyvlik.utility", 0, 1, "DirSize");

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/qml/main.qml")));

    return app.exec();
}

