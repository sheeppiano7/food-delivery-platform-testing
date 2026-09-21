#include "commmsgmanager.h"
#include "netudp/parsedata.h"
CommMsgManager::CommMsgManager(QObject *parent) : QObject(parent)
{
    initComm();

    m_nHeartBeatTimerId = startTimer(100);

    sendUserInfo();
}

void CommMsgManager::timerEvent(QTimerEvent *event)
{
    if(m_nHeartBeatTimerId == event->timerId())
    {
        sendHeartBeat();
    }
}


void CommMsgManager::initComm()
{
    qRegisterMetaType<QSharedPointer<QByteArray> >("QSharedPointer<QByteArray>");
    /** udp*/
    m_sptUdpInstance = QSharedPointer<UdpCommManager>(new UdpCommManager(this));
    connect(m_sptUdpInstance.data(), SIGNAL(sigNetworkDatagramNotify(QString, quint32, QSharedPointer<QByteArray>)), this, SLOT(slotNetworkDatagramNotify(QString, quint32, QSharedPointer<QByteArray>)), Qt::QueuedConnection);

}


void CommMsgManager::processUserinfoList(const QString &pathkey, const quint64 &account)
{
    {
        QList<UserInfo> infoList = DataBaseInterface::getInstance()->getUserInfoList();

        QByteArray array = makedata::makeUserinfoListData(infoList);

        if(m_sptUdpInstance)
        {
            m_sptUdpInstance->send(pathkey, array);
        }
    }

    {
        QList<CustomMenu> infoList = DataBaseInterface::getInstance()->getCustomMenuList();

        QByteArray array = makedata::makeCustomMenuListData(infoList);

        if(m_sptUdpInstance)
        {
            m_sptUdpInstance->send(pathkey, array);
        }
    }

    {
        QList<MenuOrderInfo> infoList = DataBaseInterface::getInstance()->getMenuOrderList();

        QByteArray array = makedata::makeCustomMenuOrderListData(infoList);

        if(m_sptUdpInstance)
        {
            m_sptUdpInstance->send(pathkey, array);
        }
    }

}

void CommMsgManager::processRegistuseerInfo(const QString &pathkey, const QByteArray & array)
{
    UserInfo info;

    if(parsedata::execute(array, array.length(), info))
    {
        DataBaseInterface::getInstance()->insertUserInfo(info);
    }
}


void CommMsgManager::processInsertCustomMenuList(const QString &pathkey, const QByteArray &array)
{
    QList<CustomMenu> infoList;

    if(parsedata::execute(array, array.length(), infoList))
    {

        QString merchantname ;
        if(!infoList.isEmpty())
        {
            merchantname = infoList.at(0).merchantname;
        }

        //! 读取原来的数据
        QList<CustomMenu> allInfoList = DataBaseInterface::getInstance()->getCustomMenuList();
        auto database = QSqlDatabase::database();
        if (!database.transaction()) return;
        for(auto info : allInfoList)
        {
            if(info.merchantname == merchantname)
            {
                DataBaseInterface::getInstance()->deleteCustomMenu(info);
            }
        }
        for(const auto & info : infoList)
        {
            if (!DataBaseInterface::getInstance()->insertCustomMenu(info)) { database.rollback(); return; }
        }

        if (!database.commit()) database.rollback();
    }
}

void CommMsgManager::processOrderMenuList(const QString &pathkey, const QByteArray & array)
{
    QList<MenuOrderInfo> infoList;

    if(parsedata::execute(array, array.length(), infoList))
    {
        QList<MenuOrderInfo> allMenuOrderList = DataBaseInterface::getInstance()->getMenuOrderList();
        auto database = QSqlDatabase::database();
        if (!database.transaction()) return;

        for(const auto & info : infoList)
        {
            for(const auto &lastInfo : allMenuOrderList)
            {
                if(info.orderuuid == lastInfo.orderuuid)
                {
                    //! 已经存在则更新表
                    DataBaseInterface::getInstance()->deleteMenuOrder(info);
                }
            }

            if (!DataBaseInterface::getInstance()->insertMenuOrderInfo(info)) { database.rollback(); return; }
        }


        if (!database.commit()) database.rollback();
    }
}

void CommMsgManager::processClientUpdate(const QString &pathkey, const QByteArray &, const UpdateInfoType & type)
{
    if(orderMenu_info == type)
    {
        QList<MenuOrderInfo> infoList = DataBaseInterface::getInstance()->getMenuOrderList();

        QByteArray array = makedata::makeCustomMenuOrderListData(infoList);

        if(m_sptUdpInstance)
        {
            m_sptUdpInstance->send(pathkey, array);
        }
    }
    else if(CustomMenu_info == type)
    {
        QList<CustomMenu> infoList = DataBaseInterface::getInstance()->getCustomMenuList();

        QByteArray array = makedata::makeCustomMenuListData(infoList);

        if(m_sptUdpInstance)
        {
            m_sptUdpInstance->send(pathkey, array);
        }
    }
}




void CommMsgManager::sendHeartBeat()
{
    static quint64 index = 0;
    index ++;
    if(index == 0xffffffffffff)
    {
        index = 0;
    }
    CommCustomMsg msg;
    msg.head = 0x1122;
    msg.account = index;

    QByteArray array =  makedata::makeCustomMsg(msg);
    if(m_sptUdpInstance)
    {
        m_sptUdpInstance->setHeartBeat(array);
    }
}

void CommMsgManager::sendUserInfo()
{
    QList<UserInfo> infoList = DataBaseInterface::getInstance()->getUserInfoList();

    QByteArray array = makedata::makeUserinfoListData(infoList);

    if(m_sptUdpInstance)
    {
        m_sptUdpInstance->setHeartBeat(array);
    }
}

void CommMsgManager::slotNetworkDatagramNotify(QString pathkey, quint32, QSharedPointer<QByteArray> sptba)
{
    QByteArray array = *sptba;

    quint16 head;

    if(array.size() < 2)
    {
        return;
    }

    memcpy((void *)&head, array.left(2).data(), 2);

    switch (head) {
    case 0x1122:
        //! 登陆时候验证服务器连接状态

        break;
    case 0xaaaa:
        //! 登陆验证
        break;
    case 0xaabb:
        processUserinfoList(pathkey, 0);
        break;
    case 0xaacc:
        processRegistuseerInfo(pathkey, array);
        break;
    case 0xaaee:
        processInsertCustomMenuList(pathkey, array);
        break;
    case 0xaaff:
        processOrderMenuList(pathkey, array);
        break;
    case 0xbbaa:
        //! 向客户端更新商家菜单信息
        break;
    case 0xbbbb:
        //! 向客户端更新订单信息
        processClientUpdate(pathkey, array, orderMenu_info);
        break;
    }
}


