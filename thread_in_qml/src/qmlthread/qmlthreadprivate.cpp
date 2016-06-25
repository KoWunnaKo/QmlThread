#include "qmlthreadprivate_p.h"
#include "qmlthread.h"

#include <QQmlComponent>
#include <QQmlContext>
#include <QDebug>

namespace qyvlik {


QmlThreadPrivate::QmlThreadPrivate(QObject *parent) :
    QThread(parent)
{ }

QUrl QmlThreadPrivate::source() const
{
    return m_source;
}

void QmlThreadPrivate::setSource(const QUrl &source)
{
    if(source != m_source) {
        m_source = source;
        Q_EMIT sourceChanged(m_source);
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

