#ifndef QMLTHREAD_P_H
#define QMLTHREAD_P_H

#include <QUrl>
#include <QFile>
#include <QJSEngine>
#include <QThread>
#include <QJsonValue>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QDebug>

#include "qmlthreadengine.h"

class Worker : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QJSValue onMessage READ onMessage WRITE setOnMessage NOTIFY onMessageChanged)
public:
    Worker(QObject *parent=0):
        QObject(parent),
        jsEngine(new QmlThreadEngine(this))
    {
        this->init();

        connect(this, SIGNAL(sendEvaluateToEngine(QString,QString,int)),
                this, SLOT(onSendEvaluateToEngine(QString,QString,int)) );

        connect(this, SIGNAL(registerHelper(QString,QmlThreadHelper*)),
                jsEngine, SIGNAL(registerHelper(QString,QmlThreadHelper*)) );
    }

    ~Worker()
    {
    }

    QJSValue onMessage() const
    {
        return m_onMessage;
    }

    void setOnMessage(const QJSValue &onMessage)
    {
        m_onMessage = onMessage;
        Q_EMIT onMessageChanged();
    }

    void clear()
    {
        this->init();
    }

    QmlThreadEngine *getJsEngine()
    { return jsEngine; }

Q_SIGNALS:
    void onMessageChanged();
    void sendMessageToThread(QJsonValue messageObject);
    void finished();
    void sendEvaluateToEngine(const QString &program, const QString &fileName,
                              int lineNumber);
    void registerHelper(const QString& helperName, QmlThreadHelper* threadHelper);
    void unregisterHelper(const QString& helperName);
    void error(const QString& errorString);

public Q_SLOTS:
    void doWork(const QJsonValue& message)
    {
        //#ifdef QT_DEBUG
        //        qDebug() << Q_FUNC_INFO;
        //#endif
        QJSValue result = this->work(message);
        if(result.isError()) {
            qDebug() << result.property("stack").toString() + ":"
                     << result.property("lineNumber").toInt()
                     << ":" << result.toString();
        } else {
            Q_EMIT finished();
        }
    }

protected:
    void init()
    {
        jsEngine->setContextProperty("Worker", jsEngine->newQObject(this));
        setOnMessage(jsEngine->evaluate("(function(msg){\n"
                                        "   console.log('this is default onMessage!');\n"
                                        "})\n"));
    }

    QJSValue work(const QJsonValue& message)
    {
        QJsonDocument jsonDocument;
        if(message.isArray()) {
            jsonDocument.setArray(message.toArray());
        } else if(message.isObject()) {
            jsonDocument.setObject(message.toObject());
        }
        QJSValue msg = jsEngine->evaluate("(JSON.parse('"
                                          +jsonDocument.toJson(QJsonDocument::Compact)
                                          +"'));");
        QJSValueList args;
        args << msg;
        return m_onMessage.call(args);
    }

private Q_SLOTS:
    void onSendEvaluateToEngine(const QString &program, const QString &fileName, int lineNumber)
    {
        QJSValue result = this->getJsEngine()->evaluate(program, fileName, lineNumber);
        if(result.isError()) {
            qDebug() << result.property("stack").toString()
                     << result.property("fileName").toString()
                     <<  result.property("lineNumber").toInt()
                     << ":" << result.toString();
            Q_EMIT error(fileName
                         + ":"
                         + result.property("lineNumber").toString()
                         + result.toString()
                         );
        }
    }

private:
    QmlThreadEngine *jsEngine;
    QJSValue m_onMessage;
};

///////////////////////////////////////////////////////////

class QmlThreadPrivate : public QObject
{
    Q_OBJECT
public:
    QmlThreadPrivate(QObject* parent):
        QObject(parent),
        worker(new Worker)
    {
        worker->moveToThread(&thread);

        connect(this, &QmlThreadPrivate::startWork,
                worker, &Worker::doWork);

        connect(worker, &Worker::sendMessageToThread,
                this, &QmlThreadPrivate::sendMessageToThread);

        connect(worker, &Worker::finished,
                this, &QmlThreadPrivate::finished);

        connect(worker, &Worker::error,
                this, &QmlThreadPrivate::error);

        connect(this, &QmlThreadPrivate::registerHelper,
                worker, &Worker::registerHelper);

        connect(this, &QmlThreadPrivate::unregisterHelper,
                worker, &Worker::unregisterHelper);

        thread.start();
    }

    ~QmlThreadPrivate()
    {
        thread.quit();
        // TODO
        thread.wait(10);
        worker->deleteLater();
    }

    QString executeString(const QString& sourceFile) {
        QString fileName = sourceFile;
        if(fileName.contains("qrc")) {
            fileName.remove("qrc");
        }
        QFile file(fileName);
        if(!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            //            qDebug() << Q_FUNC_INFO << file.errorString()
            //                     << sourceFile;
            return QString();
        }
        return QString(file.readAll());
    }

    QUrl getSource() const
    { return source; }

    void setSource(const QUrl &value)
    {
        source = value;
        worker->clear();
        // here use signal handle
        worker->sendEvaluateToEngine(executeString(source.toString()),
                                     source.toString(),
                                     1);
    }

    bool isFinished() const
    { return thread.isFinished(); }

    bool isRunning() const
    { return thread.isRunning(); }

Q_SIGNALS:
    void finished();
    void error(const QString& errorString);
    void startWork(QJsonValue message);
    void sendMessageToThread(QJsonValue message);
    void registerHelper(const QString& helperName, QmlThreadHelper* threadHelper);
    void unregisterHelper(const QString& helperName);

private:
    QUrl source;
    QThread thread;
    Worker* worker;
};


#endif // QMLTHREAD_P_H
