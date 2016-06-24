#include "threadconnector.h"

#include <QQmlEngine>
#include <QQmlComponent>
#include <QQmlContext>
#include <QDebug>

namespace qyvlik {

ThreadConnector::ThreadConnector(QObject *parent) :
    QObject(parent)
{
    Worker *worker = new Worker;

    connect(this, &ThreadConnector::sourceChanged, worker, &Worker::doWork);
    connect(&thread, &QThread::destroyed, worker, &QObject::deleteLater);

    connect(worker, &Worker::sendMessage, this, &ThreadConnector::messageReceived);
    connect(this, &ThreadConnector::sendMessage, worker, &Worker::messageReceived);

    worker->moveToThread(&thread);

    thread.start();
}

ThreadConnector::~ThreadConnector()
{
    thread.quit();
    thread.wait();
}

QUrl ThreadConnector::source() const
{
    return m_source;
}

void ThreadConnector::setSource(const QUrl &newValue)
{
    if ( newValue != m_source ) {
        m_source = newValue;
        emit sourceChanged( m_source );
    }
}

//--------------------Worker-----------------------

Worker::Worker(QObject *parent) :
    QObject(parent),
    engine(nullptr)
{

}

void Worker::doWork(const QUrl &thread_qml_file)
{
    if(engine == nullptr) {
        qDebug() << "current thread Id:" << QThread::currentThreadId();
        engine = new QQmlEngine(this);
    }

    QQmlComponent component(engine, thread_qml_file);
    QObject *myObject = component.create();

    // qDebug() << "myObject: " << myObject->parent() ;

    QmlRunnable *runnable = qobject_cast<QmlRunnable*>(myObject);
    if(runnable) {

        QQmlEngine::setObjectOwnership(runnable, QQmlEngine::JavaScriptOwnership);

        connect(runnable, &QmlRunnable::sendMessage, this, &Worker::sendMessage);
        connect(this, &Worker::messageReceived, runnable, &QmlRunnable::messageReceived);

    } else {
        qDebug() << "qml file is not QmlRunnable";
        qDebug() << component.errors();
        myObject->deleteLater();
    }
}

//--------------------Worker-----------------------

} // namespace qyvlik

