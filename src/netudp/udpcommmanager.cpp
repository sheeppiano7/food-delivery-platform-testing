#include "udpcommmanager.h"
#include <QtEndian>
#include <QFile>
#define NetworkFilePath "config/NetworkConfig.xml"

UdpCommManager::UdpCommManager(QObject *parent) : QObject(parent)
{
    init();

    initUdpConfig();
}

UdpCommManager::~UdpCommManager()
{
    release();
}

void UdpCommManager::init()
{
    qRegisterMetaType< QSharedPointer<QByteArray> >("QSharedPointer<QByteArray>");
}

void UdpCommManager::release()
{
    QMapIterator<QString, UdpSocketInfo > iter(m_pathkeyMap);
    while (iter.hasNext())
    {
        iter.next();
        const UdpSocketInfo& savedInfo = iter.value();

        disconnect(savedInfo.sptUdpInst.data(), SIGNAL(signalNetworkDatagramNotify(QByteArray, QString, quint16)),
                this, SLOT(slotNetworkDatagramNotify(QByteArray, QString, quint16)));
    }
    m_pathkeyMap.clear();
}

void UdpCommManager::initUdpConfig()
{
    QFile file(NetworkFilePath);
    if (!file.open(QIODevice::ReadOnly))
    {
        qDebug() << "open networkConfig.xml failed";
        return ;
    }

    QDomDocument doc;
    if (!doc.setContent(&file))
    {
        qDebug() << "load networkConfig.xml failed";
        file.close();
        return ;
    }
    file.close();

    QDomElement root = doc.documentElement();

    if(root.nodeName() != "Config")
    {
        return ;
    }
    QDomNode child = root.firstChild();
    while (!child.isNull()) {
        if (child.toElement().tagName() == "IpConfig")
        {
            NetworkConfig netConfig;
            netConfig.pathkey = child.toElement().attribute("pathkey");
            QDomNode child1 = child.toElement().firstChild();
            while (!child1.isNull())
            {
                if(child1.toElement().tagName() == "local"){
                    netConfig.localIp = child1.toElement().attribute("ip");
                    netConfig.localPort = child1.toElement().attribute("port").toInt();
                }else if(child1.toElement().tagName() == "target"){
                    netConfig.targetIp = child1.toElement().attribute("ip");
                    netConfig.targetPort = child1.toElement().attribute("port").toInt();
                }
                child1 = child1.nextSibling();
            }
            m_networkconfig.networkConfig.append(netConfig);
            child = child.nextSibling();
        }
    }


    for(auto key : m_networkconfig.networkConfig)
    {
        networkcfg(key.pathkey, key.localIp, key.localPort, key.targetIp, key.targetPort);
    }
    return;
}

QString UdpCommManager::generateIpportKey(QString ip, quint16 port)
{
    QString rslt = QString("%1|%2").arg(ip).arg(QString("%1").arg(port, 6, 10, QLatin1Char( '0' )));
    return rslt;
}

bool UdpCommManager::isDuplucateLocalRemoteIpPort(QString localip, quint16 localport, QString remoteip, quint16 remoteport)
{
    bool rslt = false;
    /*遍历 看是否已经存在同样地址端口的内容*/
    {
        QMapIterator<QString, UdpSocketInfo > iter(m_pathkeyMap);
        while (iter.hasNext())
        {
            iter.next();
            const UdpSocketInfo& savedInfo = iter.value();

            if (localip == savedInfo.localip && localport == savedInfo.localport &&
                remoteip == savedInfo.remoteip && remoteport == savedInfo.remoteport)
            {   /*重复配置*/
                qDebug() << "duplicate udp local remote ip port";
                rslt = true;
                break;
            }
        }
    }
    return rslt;
}

bool UdpCommManager::findLocalIpPort(QString localip, quint16 localport, QSharedPointer<NetUdp> &sptUdpInst, bool& hasUsed)
{
    bool findSocketInst = false;
    /*遍历 检查是否本地端口已经被绑定*/
    hasUsed = false;

    QMapIterator<QString, UdpSocketInfo > iter(m_pathkeyMap);
    while (iter.hasNext())
    {
        iter.next();
        const UdpSocketInfo& savedInfo = iter.value();

        if (localport == savedInfo.localport)
        {
            findSocketInst = true;
            /*已经被占用 绑定了已经*/
            if (localip == savedInfo.localip)
            {
                sptUdpInst = savedInfo.sptUdpInst;
            }
            else
            {
                qDebug() << "local port has been used!";
                hasUsed = true;
            }
            break;
        }
    }
    return findSocketInst;
}

bool UdpCommManager::networkcfg(const QString pathkey, QString localip, quint16 localport, QString remoteip, quint16 remoteport)
{
    bool rslt = false;

    bool need_insert = false;

    do
    {
        if (m_pathkeyMap.contains(pathkey))
        {
            UdpSocketInfo& savedinfo = m_pathkeyMap[pathkey];

            if (savedinfo.localip == localip && savedinfo.localport == localport)
            {
                if (savedinfo.remoteip == remoteip && savedinfo.remoteport == remoteport)
                {   /*一模一样*/
                }
                else
                {   /*之前的socket还是接着用*/

                    /*先把socket属性里面路径清空 */
                    QString savedRemoteIpportKey = generateIpportKey(savedinfo.remoteip, savedinfo.remoteport);

                    QSharedPointer<NetUdp>& sptSavedUdpInst = savedinfo.sptUdpInst;

                    sptSavedUdpInst.data()->setProperty(savedRemoteIpportKey.toStdString().c_str(), QVariant());/*清空*/

                    /*再把socket属性里面路径保存新的远端ip port*/
                    savedinfo.remoteip = remoteip; savedinfo.remoteport = remoteport;

                    QString remoteIpportKey = generateIpportKey(remoteip, remoteport);

                    sptSavedUdpInst.data()->setProperty(remoteIpportKey.toStdString().c_str(), pathkey);
                }
                rslt = true;
                break;
            }
            else
            {   /*之前的全部清除*/
                /*先把socket属性里面路径清空 */
                QString savedRemoteIpportKey = generateIpportKey(savedinfo.remoteip, savedinfo.remoteport);

                QSharedPointer<NetUdp>& sptSavedUdpInst = savedinfo.sptUdpInst;

                sptSavedUdpInst.data()->setProperty(savedRemoteIpportKey.toStdString().c_str(), QVariant());/*清空*/

                m_pathkeyMap.remove(pathkey);

                /*走后面流程 不用break*/
            }
        }

        if (isDuplucateLocalRemoteIpPort(localip, localport, remoteip, remoteport)) { break; }

        bool localportHasOccupied = false;
        QSharedPointer<NetUdp> sptUdpInst;

        bool findSocketInst = findLocalIpPort(localip, localport, sptUdpInst, localportHasOccupied);

        if (findSocketInst)
        {
            if (localportHasOccupied)
            {   /*相同port 不同ip 无法申请了已经*/
            }
            else
            {   /*相同port 相同ip 就用之前的socket 只需要增加新的ip port*/
                /*再把socket属性里面路径保存新的远端ip port*/
                QString remoteIpportKey = generateIpportKey(remoteip, remoteport);

                sptUdpInst.data()->setProperty(remoteIpportKey.toStdString().c_str(), pathkey);

                rslt = true; need_insert = true;
            }
        }
        else
        {
            sptUdpInst = QSharedPointer<NetUdp>(new NetUdp(this));

            if (sptUdpInst == NULL || sptUdpInst.isNull()) { break; }

            if (sptUdpInst->networkcfg(localip, localport))
            {
                //qRegisterMetaType< QSharedPointer<QByteArray> >("QSharedPointer<QByteArray>");

                connect(sptUdpInst.data(), SIGNAL(signalNetworkDatagramNotify(QByteArray, QString, quint16)),
                        this, SLOT(slotNetworkDatagramNotify(QByteArray, QString, quint16)), Qt::QueuedConnection);

                QString localIpportKey = generateIpportKey(localip, localport);

                sptUdpInst.data()->setProperty("LOCAL_IPPORT", localIpportKey);

                QString remoteIpportKey = generateIpportKey(remoteip, remoteport);

                sptUdpInst.data()->setProperty(remoteIpportKey.toStdString().c_str(), pathkey);

                rslt = true; need_insert = true;
            }
            else
            {
            }
        }

        if (need_insert)
        {
            UdpSocketInfo item;
            item.localip = localip;
            item.localport = localport;
            item.remoteip = remoteip;
            item.remoteport = remoteport;
            item.sptUdpInst = sptUdpInst;
            m_pathkeyMap.insert(pathkey, item);
        }

    }while(0);



    return rslt;
}

qint32 UdpCommManager::send(const QString pathkey, const QByteArray &ba)
{
    qint32 sentlen = -1;
    if (m_pathkeyMap.contains(pathkey))
    {
        UdpSocketInfo& skt = m_pathkeyMap[pathkey];
        sentlen = skt.sptUdpInst->send(ba, skt.remoteip, skt.remoteport);
    }
    return sentlen;
}

void UdpCommManager::setHeartBeat(const QByteArray &array)
{
    for(auto udp : m_networkconfig.networkConfig)
    {
        send(udp.pathkey, array);
    }
}

void UdpCommManager::slotNetworkDatagramNotify(QByteArray datagram, QString remoteip, quint16 remoteport)
{
    QSharedPointer<QByteArray> sptDatagram = QSharedPointer<QByteArray>(new QByteArray(datagram));
    QString remoteIpportKey = generateIpportKey(remoteip, remoteport);
    QString pathkey = QObject::sender()->property(remoteIpportKey.toStdString().c_str()).toString();
    emit sigNetworkDatagramNotify(pathkey, 0, sptDatagram);
}
