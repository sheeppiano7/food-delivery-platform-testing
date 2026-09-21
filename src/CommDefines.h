#pragma once

#include <QtGlobal>
#include <QString>
#include <QVector>
#include <QByteArray>
#include <qmetatype.h>
#include <QColor>
#include <QMap>

#pragma pack(1)



//! 登陆账号验证
typedef struct _CommSearchAccount
{
    quint16 head; //! 0xaaaa
    quint64 account;
}CommSearchAccount;

typedef struct _CommCustomMsg
{
    quint16 head; //! 0xaaaa
    quint64 account;
}CommCustomMsg;
typedef struct _CommUserInfo
{
    char  userId[40];            //! 唯一id
    char  userName[40];          //! 用户名
    quint64  account;           //! 账号
    quint64  password;          //! 密码
    quint64  phoneNum;          //! 电话号码
    char  address[100];           //! 地址
    char usertype;          //! 用户类型 0xaa 商家 0xab客户 0xac 骑手 0xad 管理员
    quint32  onsalelevel;       //! 优惠等级
    quint32  coupon;            //! 优惠券（用了就没了）
    quint64  managernum;        //! 管理员工号
}CommUserInfo;

typedef struct _CommUserInfoVec
{
    quint16 head = 0xaabb;  //! 0xaabb
    quint8 size;
    CommUserInfo info[100];
}CommUserInfoVec;

typedef struct _CommSignalUserInfo
{
    quint16 head = 0xaacc;  //! 0xaacc
    CommUserInfo info;
}CommSignalUserInfo;

typedef struct  _CommCustomMenu
{
    quint16 menuidx;              //! 菜单id 每个商家的菜单id都是从0开始的
    char name[40];               //! 菜品名称
    double  price;              //! 价格
    char discribe[50];           //! 描述
    char imgurl[50];             //! 图片链接
    char uuid[40];               //! 菜品唯一id
    char merchantname[40];       //! 商家名称

}CommCustomMenu;

typedef struct _CommCustomMenuInfo
{
    quint16 head;  //! 0xaadd
    CommCustomMenu info;
}CommCustomMenuInfo;

typedef struct _CommCustomMenuInfoVec
{
    quint16 head;  //! 0xaaee
    quint8 size;
    CommCustomMenu info[200];
}CommCustomMenuInfoVec;

//! 订单信息 ：订单追踪（顾客下单->商家接单->骑手接单->完成订单）
typedef struct _CommMenuOrderInfo
{
    char orderuuid[40];
    quint16 orderid;
    char name[40];
    char merchantname[40];
    quint64 customerphonenum;
    char customeraddress[50];
    char merchantaddress[50];
    quint8 status;
    char customerUuid[40];
    char orderdate[40];
    double price;

}CommMenuOrderInfo;

typedef struct _CommMenuOrderInfoVec
{
    quint16 head;  //! 0xaaff
    quint8 size;
    CommMenuOrderInfo info[200];
}CommMenuOrderInfoVec;

#pragma pack()
