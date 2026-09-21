#ifndef UDPCOMMMANAGER_H
#define UDPCOMMMANAGER_H

#include "netudp.h"
#include <QVariant>
#include <QObject>
#include <QMap>
#include <QDomDocument>
#include <QDebug>
#include <QFile>
#include "PublicDefines.h"


/*
1、while using different path-key, the manager support that the same local socket sending to different remote socket(different ip&port)
2、while using the same path-key, supporting reconfig the same local socket, the new configged local-ip&local-port&remote-ip&remote-port will take effect.
3、the same local-ip&local-port&remote-ip&remote-port will return ok directlly.
4、while using the new path-key, but the local&port has been occupied, will return false.
*/

typedef struct _UdpSocketInfo
{
    QString  localip;
    quint16  localport;
    QString  remoteip;
    quint16 remoteport;
    QSharedPointer<NetUdp> sptUdpInst;
}UdpSocketInfo;

class  UdpCommManager : public QObject
{
    Q_OBJECT
public:
    explicit UdpCommManager(QObject *parent = 0);
    ~UdpCommManager();

private:
    QMap< QString, UdpSocketInfo > m_pathkeyMap;

    void init();
    void release();

    void initUdpConfig();

    QString generateIpportKey(QString ip, quint16 port);
    bool isDuplucateLocalRemoteIpPort(QString localip, quint16 localport, QString remoteip, quint16 remoteport);
    bool findLocalIpPort(QString localip, quint16 localport, QSharedPointer<NetUdp>& sptUdpInst, bool& hasUsed);
public:
    bool networkcfg(const QString pathkey, QString localip, quint16 localport, QString remoteip, quint16 remoteport); /*配置udp本端远端信息*/
    qint32 send(const QString pathkey, const QByteArray& ba);

    void setHeartBeat(const QByteArray & array);

signals:
    void sigNetworkDatagramNotify(QString pathkey, quint32 msgid, QSharedPointer<QByteArray> sptba);

public slots:

    void slotNetworkDatagramNotify(QByteArray datagram, QString remoteip, quint16 remoteport);

private:
    NetworkConfigInfo m_networkconfig;
};

#endif // UDPCOMMMANAGER_H
