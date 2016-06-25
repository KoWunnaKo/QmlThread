#include "qmlthread.h"
#include "qmlthreadprivate_p.h"

#include <QDebug>

namespace qyvlik {

QmlThread::QmlThread(QObject *parent) :
    QObject(parent),
    d_ptr(new QmlThreadPrivate(this))
{
    connect( d_ptr, &QmlThreadPrivate::sourceChanged, this, &QmlThread::sourceChanged );
    connect( d_ptr, &QmlThreadPrivate::messageReceived, this, &QmlThread::messageReceived );

    Worker *worker = new Worker;

    connect(d_ptr, &QmlThreadPrivate::sourceChanged, worker, &Worker::doWork);
    connect(d_ptr, &QThread::destroyed, worker, &QObject::deleteLater);

    connect( d_ptr, &QmlThreadPrivate::sendMessage, worker, &Worker::messageReceived );
    connect( worker, &Worker::sendMessage, d_ptr, &QmlThreadPrivate::messageReceived);

    worker->moveToThread(d_ptr);

    d_ptr->start();
}

QmlThread::~QmlThread()
{
    d_ptr->quit();
    d_ptr->wait();
}

bool QmlThread::sendMessage(QJsonValue message)
{
    if(d_ptr->isRunning()) {

        Q_EMIT d_ptr->sendMessage(message);

        return true;
    } else {
        return false;
    }
}

QUrl QmlThread::source() const
{
    return d_ptr->source();
}

void QmlThread::setSource(const QUrl &newValue)
{
    d_ptr->setSource(newValue);
}










} // namespace qyvlik

