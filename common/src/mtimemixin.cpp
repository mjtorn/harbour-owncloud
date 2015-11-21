#include "mtimemixin.h"

MtimeMixin::MtimeMixin(QObject *parent, OwnCloudBrowser *browser) :
    QObject(parent)
{
    this->browser = browser;
}

void MtimeMixin::setLocalLastModified(TransferEntry* entry)
{
    QString localName = entry->getLocalPath();
    struct utimbuf newTimes;
    int retval;

    newTimes.actime = time(NULL);
    newTimes.modtime = entry->getLastModified().toMSecsSinceEpoch() / 1000; // seconds

    retval = utime(localName.toStdString().c_str(), &newTimes);
    if (retval != 0 ) {
        emit localMtimeFailed(errno);
    } else {
        emit localMtimeSucceeded();
    }

    qDebug() << "Local last modified " << newTimes.modtime;
    //disconnect(this, &MtimeMixin::downloadComplete, this, &MtimeMixin::setLocalLastModified);
}

void MtimeMixin::setRemoteLastModified(TransferEntry *entry, QString remotePath)
{
    Q_ASSERT(remotePath == entry->getRemotePath()); // Technicality for QML

    QWebdav::PropValues props;
    QMap<QString, QVariant> propMap;
    QString remoteName = remotePath + entry->getName();
    qint64 lastModified = entry->getLastModified().toMSecsSinceEpoch() / 1000; // seconds
    QWebdav* webdav = this->browser->getNewWebdav(); // entry's webdav is private

    webdav->setParent(this);
    connect(webdav, &QWebdav::finished, this, &MtimeMixin::setRemoteMtimeFinished);

    propMap["lastmodified"] = (QVariant) lastModified;
    props["DAV:"] = propMap;

    webdav->proppatch(remoteName, props);
    qDebug() << "Remote last modified " << lastModified;
    //disconnect(this, &MtimeMixin::uploadComplete, this, &MtimeMi::setRemoteLastModified);
}

void MtimeMixin::setRemoteMtimeFinished(QNetworkReply *networkReply)
{
    QVariant attr = networkReply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    int status = attr.toInt();
    qDebug() << "setting mtime status " << status;
    if (status < 200 || status >= 300) {
        emit remoteMtimeFailed(status);
    } else {
        emit remoteMtimeSucceeded();
        // Refresh to see the current, new mtime
        this->browser->getDirectoryContent(this->browser->getCurrentPath());
    }
}

