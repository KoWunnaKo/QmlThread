#include "qmlthread.h"

#include <QDebug>
#include <QThread>
#include <QtQml>

namespace qyvlik {

Worker::Worker(QObject *parent) :
    QObject(parent),
    mEngine(nullptr)
{
}

void Worker::doWork(const QUrl &thread_qml_file)
{
    if(mEngine == nullptr) {
        mEngine = new QQmlEngine(this);
        qmlInfo(mEngine) << tr("thread qml file: ") << thread_qml_file
                         << tr("current thread id: ")<< QThread::currentThreadId();
    }

    QQmlComponent component(mEngine, thread_qml_file, QQmlComponent::PreferSynchronous);

    if(component.status() != QQmlComponent::Ready) {
        qmlInfo(&component) << component.errors();
        return;
    }

    QObject *myObject = component.create();

    QmlRunnable *runnable = qobject_cast<QmlRunnable*>(myObject);
    if(runnable == nullptr) {
        qmlInfo(myObject) << tr("qml file is not QmlRunnable") << component.errors();
        myObject->deleteLater();
        return;
    }

    QQmlEngine::setObjectOwnership(runnable, QQmlEngine::JavaScriptOwnership);

    connect(this, &Worker::messageReceived, runnable, &QmlRunnable::messageReceived);
    connect(runnable, &QmlRunnable::sendMessage, this, &Worker::sendMessage);
}


QmlRunnable::QmlRunnable(QObject *parent) :
    QObject(parent)
{}

void QmlRunnable::classBegin()
{
    QThread* mainThread = QCoreApplication::instance()->thread();
    QThread* currentThread = QThread::currentThread();

    if(mainThread == currentThread) {

        qmlInfo(this) << "mainThread: " << (void*)mainThread
                      << "currentThread: " << (void*)currentThread;
        // abort();
    }
}

void QmlRunnable::componentComplete()
{
}


class QmlThreadPrivate
{
public:
    QmlThreadPrivate(QThread* thread, Worker* worker):
        mThread(thread),
        mWorker(worker)
    {}
    QThread* mThread;
    Worker* mWorker;
    QUrl mRunnableSource;
};

QmlThread::QmlThread(QObject *parent) :
    QObject(parent),
    d_ptr(new QmlThreadPrivate(new QThread(this), new Worker))
{

    d_ptr->mWorker->moveToThread(d_ptr->mThread);

    connect(this, &QmlThread::runnableSourceChanged, d_ptr->mWorker, &Worker::doWork);
    connect(d_ptr->mThread, &QThread::destroyed, d_ptr->mWorker, &QObject::deleteLater);

    connect(d_ptr->mWorker, &Worker::sendMessage, this, &QmlThread::messageReceived);

    d_ptr->mThread->start();
}

QmlThread::~QmlThread()
{
    d_ptr->mThread->quit();
    d_ptr->mThread->wait();
    delete d_ptr;
}

bool QmlThread::sendMessage(const QJsonValue& message)
{
    if(!d_ptr->mThread->isRunning()) {
        qmlInfo(d_ptr->mWorker) << "thread is running";
        return false;
    }

    Q_EMIT d_ptr->mWorker->messageReceived(message);

    return true;
}

QUrl QmlThread::runnableSource() const
{
    return d_ptr->mRunnableSource;
}

void QmlThread::setRunnableSource(const QUrl &newValue)
{
    if(d_ptr->mRunnableSource != newValue) {
        d_ptr->mRunnableSource = newValue;
        Q_EMIT runnableSourceChanged(d_ptr->mRunnableSource);
    }
}



} // namespace qyvlik

