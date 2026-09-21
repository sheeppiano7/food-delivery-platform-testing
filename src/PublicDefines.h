#pragma once

#include <QtGlobal>
#include <QString>
#include <QVector>
#include <QByteArray>
#include <qmetatype.h>
#include <QColor>
#include <QMap>

typedef  enum   _UpdateInfoType
{
    CustomMenu_info,
    orderMenu_info
}UpdateInfoType;
/**
 * 网络数据配置结构体
 */
typedef struct _NetworkConfig
{
    QString    pathkey;
    QString    targetIp;   //! 目标通信IP地址
    quint16    targetPort; //! 目标通信端口号
    QString    localIp;    //! 本地通信IP地址
    quint16    localPort;  //! 本地通信端口号
}NetworkConfig;


typedef  struct _NetworkConfigInfo
{
    QVector<NetworkConfig> networkConfig;
}NetworkConfigInfo;
typedef struct _OrderMenu
{
    quint16 menuId;         //! 菜单号
    quint16 dishId;         //! 菜品ID
}OrderMenu;

typedef struct _OrderInfo
{
    quint16 tableID;         //! 桌号
    quint16 menuId;         //! 菜单号
}OrderInfo;




typedef enum _UserType
{
    user_merchant = 10,
    user_customer,
    user_rider,
    user_manager,
    user_none = 0xff
}UserType;



typedef struct _UserInfo
{
    QString  userId = QString();            //! 唯一id
    QString  userName = QString();          //! 用户名
    quint64  account= 0;           //! 账号
    quint64  password= 0;          //! 密码
    quint64  phoneNum= 0;          //! 电话号码
    QString  address = QString();           //! 地址
    UserType usertype = user_none;          //! 用户类型 0xaa 商家 0xab客户 0xac 骑手 0xad 管理员
    quint32  onsalelevel= 0;       //! 优惠等级
    quint32  coupon = 0;            //! 优惠券（用了就没了）
    quint64  managernum= 0;        //! 管理员工号
}UserInfo;

typedef struct  _CustomMenu
{
    quint16 menuidx;              //! 菜单id 每个商家的菜单id都是从0开始的
    QString name;               //! 菜品名称
    double  price;              //! 价格
    QString discribe;           //! 描述
    QString imgurl;             //! 图片链接
    QString uuid;               //! 菜品唯一id
    QString merchantname;       //! 商家名称

}CustomMenu;


typedef  enum _MenuOrderStatus
{
    order_place,
    order_receiving,
    order_riderreceipt,
    order_finish
}MenuOrderStatus;
//! 订单信息 ：订单追踪（顾客下单->商家接单->骑手接单->完成订单）
typedef struct _MenuOrderInfo
{
    quint16 orderid;
    QString orderuuid;
    QString name;
    QString merchantname;
    quint64 customerphonenum;
    QString customeraddress;
    QString merchantaddress;
    MenuOrderStatus orderstatus;
    QString customerUuid;
    QString orderdate;
    double price;

}MenuOrderInfo;

Q_DECLARE_METATYPE(UserInfo)
Q_DECLARE_METATYPE(CustomMenu)
Q_DECLARE_METATYPE(MenuOrderInfo)
