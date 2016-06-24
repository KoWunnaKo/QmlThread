TEMPLATE = app

QT += qml quick widgets

SOURCES += main.cpp
RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)

HEADERS += \
   src/test/foo.h

CONFIG += C++11

SOURCES += src/qmlthread/qmlthread.cpp \
    src/qmlthread/qmlthreadengine.cpp

HEADERS += \
    src/qmlthread/qmlthread.h \
    src/qmlthread/qmlthread_p.h \
    src/qmlthread/qmlthreadhelper.h \
    src/qmlthread/qmlthreadengine.h
