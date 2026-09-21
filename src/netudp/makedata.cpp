#include "makedata.h"

makedata::makedata(QObject *parent) : QObject(parent)
{

}


QByteArray makedata::makeCustomMsg(const CommCustomMsg &info)
{
    CommCustomMsg topic;
    topic = info;
    QByteArray message;
    message.clear();
    message.append((const char *)&topic, sizeof(topic));
    return message;
}

QByteArray makedata::makeSignalUserinfoData(const UserInfo &info)
{
    CommSignalUserInfo commuserinfo;

    strcpy(commuserinfo.info.userId, info.userId.toStdString().data());
    strcpy(commuserinfo.info.userName, info.userName.toStdString().data());
    commuserinfo.info.account = info.account;
    commuserinfo.info.password = info.password;
    commuserinfo.info.phoneNum = info.phoneNum;
    strcpy(commuserinfo.info.address, info.address.toStdString().data());
    commuserinfo.info.usertype = info.usertype;
    commuserinfo.info.onsalelevel = info.onsalelevel;
    commuserinfo.info.coupon = info.coupon;
    commuserinfo.info.managernum = info.managernum;

    QByteArray message;
    message.clear();
    message.append((const char *)&commuserinfo, sizeof(commuserinfo));
    return message;
}

QByteArray makedata::makeUserinfoListData(const QList<UserInfo> &infoList)
{

    CommUserInfoVec topic;

    quint8 index = 0;
    for(const auto & info : infoList)
    {
        CommUserInfo commuserinfo;
        strcpy(commuserinfo.userId, info.userId.toStdString().data());
        strcpy(commuserinfo.userName, info.userName.toStdString().data());
        commuserinfo.account = info.account;
        commuserinfo.password = info.password;
        commuserinfo.phoneNum = info.phoneNum;
        strcpy(commuserinfo.address, info.address.toStdString().data());
        commuserinfo.usertype = info.usertype;
        commuserinfo.onsalelevel = info.onsalelevel;
        commuserinfo.coupon = info.coupon;
        commuserinfo.managernum = info.managernum;

        topic.info[index] = commuserinfo;
        index ++;
    }

    topic.size = infoList.size();

    QByteArray message;
    message.clear();
    message.append((const char *)&topic, sizeof(topic));
    return message;
}

QByteArray makedata::makeCustomMenuListData(const QList<CustomMenu> &infoList)
{
    CommCustomMenuInfoVec topic;

    quint8 index = 0;
    for(const auto & info : infoList)
    {
        CommCustomMenu rawTopic;
        rawTopic.menuidx = info.menuidx;
        strcpy(rawTopic.name, info.name.toStdString().data());
        rawTopic.price = info.price;
        strcpy(rawTopic.discribe, info.discribe.toStdString().data());
        strcpy(rawTopic.imgurl, info.imgurl.toStdString().data());
        strcpy(rawTopic.uuid, info.uuid.toStdString().data());
        strcpy(rawTopic.merchantname, info.merchantname.toStdString().data());
        topic.info[index] = rawTopic;
        index ++;
    }

    topic.head = 0xaaee;
    topic.size = infoList.size();
    QByteArray message;
    message.clear();
    message.append((const char *)&topic, sizeof(topic));
    return message;
}

QByteArray makedata::makeCustomMenuOrderListData(const QList<MenuOrderInfo> &infoList)
{
    CommMenuOrderInfoVec topic;

    quint8 index = 0;
    for(const auto & info : infoList)
    {
        CommMenuOrderInfo rawTopic;
        strcpy(rawTopic.orderuuid, info.orderuuid.toStdString().data());
        rawTopic.orderid = info.orderid;
        strcpy(rawTopic.name, info.name.toStdString().data());
        strcpy(rawTopic.merchantname, info.merchantname.toStdString().data());
        rawTopic.customerphonenum = info.customerphonenum;
        strcpy(rawTopic.customeraddress, info.customeraddress.toStdString().data());
        strcpy(rawTopic.merchantaddress, info.merchantaddress.toStdString().data());
        rawTopic.status = info.orderstatus;
        strcpy(rawTopic.customerUuid, info.customerUuid.toStdString().data());
        strcpy(rawTopic.orderdate, info.orderdate.toStdString().data());
        rawTopic.price = info.price;
        topic.info[index] = rawTopic;
        index ++;
    }
    topic.head = 0xaaff;
    topic.size = infoList.size();
    QByteArray message;
    message.clear();
    message.append((const char *)&topic, sizeof(topic));
    return message;
}
