#ifndef COMMMSGMANAGER_H
#define COMMMSGMANAGER_H

#include <QObject>
#include "netudp/udpcommmanager.h"
#include "databaseinterface.h"
#include "netudp/makedata.h"
#include <QTimerEvent>

class CommMsgManager : public QObject
{
    Q_OBJECT
public:
    explicit CommMsgManager(QObject *parent = nullptr);
protected:
    void timerEvent(QTimerEvent *event) override;

signals:

    void sigAccountVerif(const QString & pathkey, const quint64 & account);

private slots:
   void slotNetworkDatagramNotify(QString, quint32, QSharedPointer<QByteArray>);
private:
    void initComm();

    //! 处理用户上报信息
    void processUserinfoList(const QString & pathkey, const quint64 & account);

    //! 用户注册信息
    void processRegistuseerInfo(const QString & pathkey, const QByteArray &);

    //! 处理商铺菜单信息
    void processInsertCustomMenuList(const QString & pathkey, const QByteArray &);

    //! 处理订单信息
    void processOrderMenuList(const QString & pathkey, const QByteArray &);

    //! 处理数据更新请求
    void processClientUpdate(const QString & pathkey, const QByteArray &, const UpdateInfoType&);

    //! 发送心跳
    void sendHeartBeat();

    //! 发送用户列表信息
    void sendUserInfo();
private:
    /**
     * @brief m_sptUdpInstance UDP管理器实例
     */
    QSharedPointer<UdpCommManager> m_sptUdpInstance = Q_NULLPTR;

    int m_nHeartBeatTimerId;

};

#endif // COMMMSGMANAGER_H
