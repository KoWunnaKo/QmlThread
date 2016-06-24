#include <QApplication>
#include <QQmlApplicationEngine>
#include <qqml.h>

#include <QFile>
#include <QDebug>
#include <QJSEngine>
#include <QJSValue>
#include <QJSValueIterator>

#include "src/qmlthread/qmlthread.h"
#include "src/test/foo.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);


    qmlRegisterUncreatableType<QmlThreadHelper>("Core",
                                                0, 1,
                                                "ThreadHelper",
                                                "ThreadHelper is Uncreatable Type");

    qmlRegisterType<QmlThread>("Core", 0, 1, "Thread");

    qmlRegisterType<FooHelper>("Core", 0, 1, "FooHelper");

//    QJSEngine jsEngine;
//    QJSValue global = jsEngine.globalObject();
//    QJSValueIterator it(global);
//    while (it.hasNext()) {
//        it.next();
//        qDebug() << it.name() << ": " << it.value().toString();
//    }


    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/qml/main.qml")));

//    // remove qrc...
//    QFile file(":/main.qml");
//    if(!file.open(QFile::ReadOnly | QFile::Text) ) {
//        qDebug() << file.errorString();
//    } else {
//        qDebug()<< file.readAll();
//    }

    return app.exec();
}

