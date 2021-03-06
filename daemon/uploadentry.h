#ifndef UPLOADENTRY_H
#define UPLOADENTRY_H

#include <QObject>
#include <QStringList>
#include <QNetworkReply>

class QWebdav;

class UploadEntry : public QObject
{
    Q_OBJECT
public:
    explicit UploadEntry(QString localPath, QString remotePath, QStringList pathsToCreate, QWebdav *connection, QObject *parent = 0);
    ~UploadEntry();

    QString localPath() { return m_localPath; }
    QString remotePath() { return m_remotePath; }
    QStringList pathsToCreate() { return m_createdPaths; }
    bool succeeded() { return m_succeeded; }
    void abort() { if(m_currentReply) m_currentReply->abort(); }

signals:
    void uploadFailed(QString errorMessage);
    void finished();

private slots:
    void doUpload();
    void errorHandler(QNetworkReply::NetworkError);
    void resetReply();

private:
    void createDirectory();
    QNetworkReply *m_currentReply;
    QWebdav *m_connection;
    QStringList m_pathsToCreate;
    QStringList m_createdPaths;
    QString m_localPath;
    QString m_remotePath;
    bool m_succeeded;
};

#endif // UPLOADENTRY_H
