#include "parsedata.h"

parsedata::parsedata(QObject *parent) : QObject(parent)
{

}

bool parsedata::execute(const char *data, int len, UserInfo &info)
{
    if(len != sizeof (CommSignalUserInfo))
    {
        return  false;
    }

    CommSignalUserInfo sigUserInfo;

    memcpy((void *)&sigUserInfo, data, len);

    info.userId = QString(sigUserInfo.info.userId);
    info.userName = QString(sigUserInfo.info.userName);
    info.account = sigUserInfo.info.account;
    info.password = sigUserInfo.info.password;
    info.phoneNum = sigUserInfo.info.phoneNum;
    info.account = sigUserInfo.info.account;
    info.address = QString(sigUserInfo.info.address);
    info.usertype = (UserType)sigUserInfo.info.usertype;
    info.onsalelevel = sigUserInfo.info.onsalelevel;
    info.coupon = sigUserInfo.info.coupon;
    info.managernum = sigUserInfo.info.managernum;


    return true;
}

bool parsedata::execute(const char *data, int len, QList<UserInfo> &userinfolist)
{
    if(len != sizeof (CommUserInfoVec))
    {
        return  false;
    }


    CommUserInfoVec commuserInfoVec;


    memcpy((void *) &commuserInfoVec, data, len);

    for(int i = 0; i < commuserInfoVec.size; i++)
    {
        UserInfo info;
        CommUserInfo commUserInfo = commuserInfoVec.info[i];
        info.userId = QString(commUserInfo.userId);
        info.userName = QString(commUserInfo.userName);
        info.account = commUserInfo.account;
        info.password = commUserInfo.password;
        info.phoneNum = commUserInfo.phoneNum;
        info.account = commUserInfo.account;
        info.address = QString(commUserInfo.address);
        info.usertype = (UserType)commUserInfo.usertype;
        info.onsalelevel = commUserInfo.onsalelevel;
        info.coupon = commUserInfo.coupon;
        info.managernum = commUserInfo.managernum;
        userinfolist.push_back(info);
    }

    return true;
}

bool parsedata::execute(const char *data, int len, CustomMenu &info)
{
    if(len != sizeof (CommCustomMenuInfo))
    {
        return  false;
    }

    CommCustomMenuInfo commCustomMenu;

    memcpy((void *)&commCustomMenu, data, len);

    info.menuidx = commCustomMenu.info.menuidx;
    info.name = QString(commCustomMenu.info.name);
    info.price = commCustomMenu.info.price;

    info.discribe = QString(commCustomMenu.info.discribe);
    info.imgurl = QString(commCustomMenu.info.imgurl);
    info.uuid = QString(commCustomMenu.info.uuid);
    info.merchantname = QString(commCustomMenu.info.merchantname);
    return true;
}

bool parsedata::execute(const char *data, int len, QList<CustomMenu> &customMenuList)
{
    if(len != sizeof (CommCustomMenuInfoVec))
    {
        return  false;
    }

    CommCustomMenuInfoVec InfoVec;
    memcpy((void *) &InfoVec, data, len);

    for(int i = 0; i < InfoVec.size; i++)
    {
        CustomMenu info;
        CommCustomMenu customInfo = InfoVec.info[i];

        info.menuidx = customInfo.menuidx;
        info.name = QString(customInfo.name);
        info.price = customInfo.price;
        info.discribe = QString(customInfo.discribe);
        info.imgurl = QString(customInfo.imgurl);
        info.uuid = QString(customInfo.uuid);
        info.merchantname = QString(customInfo.merchantname);

        customMenuList.push_back(info);
    }

    return true;
}

bool parsedata::execute(const char *data, int len, QList<MenuOrderInfo> &customMenuList)
{
    if(len != sizeof (CommMenuOrderInfoVec))
    {
        return  false;
    }

    CommMenuOrderInfoVec InfoVec;
    memcpy((void *) &InfoVec, data, len);

    for(int i = 0; i < InfoVec.size; i++)
    {
        MenuOrderInfo info;
        CommMenuOrderInfo customInfo = InfoVec.info[i];

        info.orderuuid = QString(customInfo.orderuuid);
        info.orderid = customInfo.orderid;
        info.name = QString(customInfo.name);
        info.merchantname = QString(customInfo.merchantname);
        info.customerphonenum = customInfo.customerphonenum;
        info.customeraddress = QString(customInfo.customeraddress);
        info.merchantaddress = QString(customInfo.merchantaddress);
        info.orderstatus = (MenuOrderStatus)customInfo.status;
        info.customerUuid = QString(customInfo.customerUuid);
        info.orderdate = QString(customInfo.orderdate);
        info.price = customInfo.price;

        customMenuList.push_back(info);
    }

    return true;
}
