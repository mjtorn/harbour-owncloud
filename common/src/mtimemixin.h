#pragma once

#include <QObject>

#include <errno.h>
#include <time.h>
#include <unistd.h>
#include <utime.h>
#include <sys/stat.h>

#include "owncloudbrowser.h"
#include "transferentry.h"

class MtimeMixin : public QObject
{
    Q_OBJECT

public:
    MtimeMixin(QObject *parent = 0, OwnCloudBrowser *browser = 0);

private:
    OwnCloudBrowser *browser;

signals:
    void localMtimeFailed(int status);
    void localMtimeSucceeded();
    void remoteMtimeFailed(int status);
    void remoteMtimeSucceeded();

public slots:
    void setLocalLastModified(TransferEntry* entry);
    void setRemoteLastModified(TransferEntry* entry, QString remotePath);
    void setRemoteMtimeFinished(QNetworkReply* networkReply);
};

