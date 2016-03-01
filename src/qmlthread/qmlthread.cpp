#include "qmlthread.h"
#include "qmlthread_p.h"

QmlThread::QmlThread(QObject *parent)
    : QObject(parent),
      d_ptr(new QmlThreadPrivate(this))
{
    connect(d_ptr, &QmlThreadPrivate::sendMessageToThread,
            this, &QmlThread::message);
}


QmlThread::~QmlThread()
{
    d_ptr->deleteLater();
}


QUrl QmlThread::source() const
{
    return d_ptr->getSource();
}


void QmlThread::setSource(const QUrl &value)
{
    if(d_ptr->getSource() != value) {
        d_ptr->setSource(value);
        Q_EMIT sourceChanged();
    }
}

QQmlListProperty<QmlThreadHelper> QmlThread::helpers()
{
    return QQmlListProperty<QmlThreadHelper>(this,
                                             &this->m_threadHelpers,
                                             &QmlThread::helpers_append,
                                             &QmlThread::helpers_count,
                                             &QmlThread::helpers_at,
                                             &QmlThread::helpers_clear
                                             );
}


void QmlThread::sendMessageToWorker(QJsonValue message)
{
    //#ifdef QT_DEBUG
    //        qDebug() << Q_FUNC_INFO;
    //#endif
    Q_EMIT d_ptr->startWork(message);
}


void QmlThread::registerThreadHelper(const QString &name, QmlThreadHelper *threadHelper)
{
    d_ptr->registerHelper(name, threadHelper);
}


void QmlThread::helpers_append(QQmlListProperty<QmlThreadHelper> *list, QmlThreadHelper *value)
{
    QmlThread* root = qobject_cast<QmlThread*>(list->object);
    if(root) {
        root->m_threadHelpers.append(value);
        //
        root->registerThreadHelper(value->factoryName(), value);
    }
}


QmlThreadHelper *QmlThread::helpers_at(QQmlListProperty<QmlThreadHelper> *list, int index)
{
    QmlThread* root = qobject_cast<QmlThread*>(list->object);
    if(root && root->m_threadHelpers.size() > index) {
        return root->m_threadHelpers.at(index);
    } else {
        return Q_NULLPTR;
    }
}


void QmlThread::helpers_clear(QQmlListProperty<QmlThreadHelper> *list)
{
    QmlThread* root = qobject_cast<QmlThread*>(list->object);
    if(root) {
        root->m_threadHelpers.clear();
    }
}


int QmlThread::helpers_count(QQmlListProperty<QmlThreadHelper> *list)
{
    QmlThread* root = qobject_cast<QmlThread*>(list->object);
    if(root) {
        return root->m_threadHelpers.size();
    } else {
        return 0;
    }
}
