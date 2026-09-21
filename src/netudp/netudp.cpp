#include "netudp.h"
//

NetUdp::NetUdp(QObject *parent) : QObject(parent)
{
    init();

}

NetUdp::~NetUdp()
{
    release();
}

/*初始化*/
void NetUdp::init()
{
    return;
}

/*释放资源*/
void NetUdp::release()
{
    releaseSocket();
}

/*释放socket资源*/
void NetUdp::releaseSocket()
{
    if (m_ptUdpSocket != NULL)
    {
        disconnect(m_ptUdpSocket, SIGNAL(readyRead()), this, SLOT(slotReadPendingDatagrams()));
        delete m_ptUdpSocket; m_ptUdpSocket = NULL;
    }

    return;
}

/*接收数据的槽函数 控制端口都从这里接收*/
void NetUdp::slotReadPendingDatagrams()
{
    while (m_ptUdpSocket->hasPendingDatagrams())
    {
        qint64 pending_size = m_ptUdpSocket->pendingDatagramSize();
        QSharedPointer<QByteArray> sptDatagram = QSharedPointer<QByteArray>(new QByteArray(pending_size, (char)(quint8)(0)));
        if (sptDatagram.isNull() || sptDatagram == NULL)
        {
            return;
        }
        quint16 remote_port;
        QHostAddress remote_address;
        qint64 recv_len = m_ptUdpSocket->readDatagram(sptDatagram->data(), pending_size, &remote_address, &remote_port);
        if (recv_len > 0)
        {
            emit signalNetworkDatagramNotify(*sptDatagram, remote_address.toString(), remote_port);
        }
    }

    return;
}

/*配置udp本端远端信息*/
bool NetUdp::networkcfg(QString localip, quint16 localport)
{
    /*获取配置的地址IP*/
    m_localAddress = QHostAddress(localip);

    /*获取配置的端口号*/
    m_localPort  = localport;

    bool rslt = false;
    /*udp 端口*/
    if ((m_ptUdpSocket = new QUdpSocket(this)) == NULL) { return false; };
    /*in win64 enviroment, the connenc para must not use Qt::QueuedConnection,
     * otherwise the readyRead() just can be received at the first time ,
     * the following readyRead() can't be received. i have no idea the reason.*/
    /*in linux enviroment, suggest using the Qt::QueuedConnection method*/
    /*The bind code should write behind the connect method.*/
    connect(m_ptUdpSocket, SIGNAL(readyRead()), this, SLOT(slotReadPendingDatagrams())/*, Qt::QueuedConnection*/);
    if (m_ptUdpSocket->bind(m_localAddress, m_localPort))
    {   /*connect一定要放在bind前面 否则收不到readyRead信号*/
        /*connect(m_ptUdpSocket, SIGNAL(readyRead()), this, SLOT(slotReadPendingDatagrams()))*/;
        rslt = true;
    }
    return rslt;
}

qint64 NetUdp::send(const QByteArray &datagram, QString remoteip, quint16 remoteport)
{
    qint64 sendlen = -1;

    //static quint32 testcout = 0;
    //qint32 testcoutplus = testcout + 1;

    do
    {
        if (m_ptUdpSocket == NULL) { break; }/*保护*/

        /*发送报文*/
        sendlen = m_ptUdpSocket->writeDatagram(datagram, QHostAddress(remoteip), remoteport);


    }while(0);

    return sendlen;
}
