#include "qmlthreadengine.h"

Console::Console(QObject *parent) :
    QObject(parent)
{}

void Console::print(const QJSValue &value) {
    printf(qPrintable(value.toString()));
    fflush(stdout);
}

QmlThreadEngine::QmlThreadEngine(QObject *parent):
    QJSEngine(parent)
{
    globalObject().setProperty("console", newQObject(new Console(this)));
    globalObject()
            .property("console")
            .setProperty("log", evaluate("function(){                                \n"
                                         "    console.print('QmlThread:')            \n"
                                         "    for(var iter in arguments) {           \n"
                                         "        console.print(arguments[iter]+' ');\n"
                                         "    }                                      \n"
                                         "    console.print('\\n')                   \n"
                                         " }\n"));

    connect( this, SIGNAL(registerHelper(QString,QmlThreadHelper*)),
             this, SLOT(onRegisterHelper(QString,QmlThreadHelper*)) );
}

void QmlThreadEngine::setContextProperty(const QString &name, const QJSValue &value)
{
    this->globalObject().setProperty(name, value);
}

void QmlThreadEngine::onRegisterHelper(const QString &helperName,
                                           QmlThreadHelper *threadHelper)
{
    auto find = threadHelperFactoryMap.find(helperName);
    auto end = threadHelperFactoryMap.end();
    if(find == end) {
        threadHelperFactoryMap.insert(helperName, threadHelper->helperFactory(this, this));
        this->setContextProperty(helperName,
                                 this->newQObject(threadHelperFactoryMap[helperName]));
    } else {
        threadHelperFactoryMap[helperName]->deleteLater();
        threadHelperFactoryMap[helperName] = threadHelper->helperFactory(this, this);
        this->setContextProperty(helperName,
                                 this->newQObject(threadHelperFactoryMap[helperName]));
    }
}



/*
"Object" :  "function() { [code] }"
"String" :  "function() { [code] }"
"Number" :  "function() { [code] }"
"Boolean" :  "function() { [code] }"
"Array" :  "function() { [code] }"
"Function" :  "function() { [code] }"
"Date" :  "function() { [code] }"
"RegExp" :  "function() { [code] }"
"Error" :  "function() { [code] }"
"EvalError" :  "function() { [code] }"
"RangeError" :  "function() { [code] }"
"ReferenceError" :  "function() { [code] }"
"SyntaxError" :  "function() { [code] }"
"TypeError" :  "function() { [code] }"
"URIError" :  "function() { [code] }"
"ArrayBuffer" :  "function() { [code] }"
"DataView" :  "function() { [code] }"
"Int8Array" :  "function() { [code] }"
"Uint8Array" :  "function() { [code] }"
"Uint8ClampedArray" :  "function() { [code] }"
"Int16Array" :  "function() { [code] }"
"Uint16Array" :  "function() { [code] }"
"Int32Array" :  "function() { [code] }"
"Uint32Array" :  "function() { [code] }"
"Float32Array" :  "function() { [code] }"
"Float64Array" :  "function() { [code] }"
"Math" :  "[object Math]"
"JSON" :  "[object JSON]"
"undefined" :  "undefined"
"NaN" :  "NaN"
"Infinity" :  "Infinity"
"eval" :  "function() { [code] }"
"parseInt" :  "function() { [code] }"
"parseFloat" :  "function() { [code] }"
"isNaN" :  "function() { [code] }"
"isFinite" :  "function() { [code] }"
"decodeURI" :  "function() { [code] }"
"decodeURIComponent" :  "function() { [code] }"
"encodeURI" :  "function() { [code] }"
"encodeURIComponent" :  "function() { [code] }"
"escape" :  "function() { [code] }"
"unescape" :  "function() { [code] }"
*/
