#ifndef QYVLIK_DIRSIZE_H
#define QYVLIK_DIRSIZE_H

#include <QObject>

namespace qyvlik {

class DirSize : public QObject
{
    Q_OBJECT
public:
    explicit DirSize(QObject *parent = 0);

    Q_INVOKABLE qint64 dirSize(const QString &path);
};

} // namespace qyvlik

#endif // QYVLIK_DIRSIZE_H
