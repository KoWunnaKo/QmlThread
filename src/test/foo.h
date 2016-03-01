#ifndef FOO
#define FOO

#include "src/qmlthread/qmlthreadhelper.h"

class Foo : public QObject
{
    Q_OBJECT
public:
    Foo(QObject *parent = Q_NULLPTR):
        QObject(parent)
    { }
    Q_INVOKABLE int helper(int i) {
        if(i<=0) {
            return 1;
        } else {
            return helper(i-1) + helper(i-2);
        }
    }
};


class FooHelperFactory : public QmlThreadHelperFactory
{
    Q_OBJECT
public:
    FooHelperFactory(QObject *parent, QJSEngine* engine):
        QmlThreadHelperFactory(parent, engine)
    { }

    QJSValue create() {
        static Foo object;
        return getJsEngine()->newQObject(&object);
    }
};


class FooHelper : public QmlThreadHelper
{
    Q_OBJECT
public:
    FooHelper(QObject *parent = Q_NULLPTR):
        QmlThreadHelper(parent)
    { this->setFactoryName("FooHelperFactory");}

    QmlThreadHelperFactory* helperFactory(QObject* factoryParent, QJSEngine* engine) {
        return new FooHelperFactory(factoryParent, engine);
    }
};


#endif // FOO
