#ifndef NetUdp_H
#define NetUdp_H


#include <QObject>
#include <QUdpSocket>
#include <QSharedPointer>

class  NetUdp : public QObject
{
    Q_OBJECT
public:
    explicit NetUdp(QObject *parent = 0);
    ~NetUdp();

    bool networkcfg(QString localip, quint16 localport); /*配置udp本端远端信息*/
    qint64 send(const QByteArray& datagram, QString remoteip, quint16 remoteport);       /*发送接口*/

private:

    void init();
    void release();
    void releaseSocket();

    /*网络通信相关*/
    QHostAddress m_localAddress;        /*本端地址*/

    quint16 m_localPort;                /*udp 本地端口*/
    QUdpSocket* m_ptUdpSocket;          /*udp socket*/

signals:
    void signalNetworkDatagramNotify(QByteArray datagram, QString remoteip, quint16 remoteport);

public slots:
    void slotReadPendingDatagrams();

};

#endif // NetUdp_H
