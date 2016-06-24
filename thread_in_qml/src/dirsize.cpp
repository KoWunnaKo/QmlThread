#include "dirsize.h"

#include <QDir>
#include <QtDebug>

namespace qyvlik {

DirSize::DirSize(QObject *parent) :
    QObject(parent)
{

}

qint64 DirSize::dirSize(const QString &path)
{
    QString path_ = path;
    QDir dir(path_.replace("file:///", ""));

    // qDebug() << path_ << " exists:" << dir.exists();

    qint64 size = 0;

    foreach (QFileInfo fileInfo, dir.entryInfoList(QDir::Files)) {
        size += fileInfo.size();
    }

    foreach (QString subDir, dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
        size += dirSize(path + QDir::separator() + subDir);
    }
    return size;
}

} // namespace qyvlik

