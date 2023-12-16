#pragma once

#include <QObject>
#include <QFile>
class NetworkInterface;

class FilePeer : public QObject
{
    Q_OBJECT
public:
    explicit FilePeer(NetworkInterface* pNetworkInterface, const QString & filePath, qint64 socketId);

    void startUpload();
    void startDownload();
signals:
    void sigDownloadInfo(qint64 sendBytes, qint64 size, bool atEnd);
public slots:
    void sendNextPacket();
    void receivedPacket(const QJsonObject &objData);
private:
    NetworkInterface* m_pNetworkInterface;
    const QString m_filePath;
    QFile m_file;
    QDataStream m_fileStream{&m_file};
    qint64 m_connectSocket;
};

