#ifndef DATABASEINTERFACE_H
#define DATABASEINTERFACE_H

#include <QObject>
#include <QSqlQuery>
#include <QSqlError>
#include <functional>
#include <QSqlDatabase>
#include <QMutex>
#include <QSemaphore>

#include "PublicDefines.h"
using namespace  std;


typedef struct SqlQueue{
    QString sql{""};
    std::function<void (const bool &isQuery,const  QSqlQuery &query)> callback{[&](const bool &isQuery,const  QSqlQuery &query){}};
}SqlQueue;

class DataBaseInterface : public QObject
{
    Q_OBJECT
public:
    /**
     * @brief getInstance 获取单例
     * @return
     */
    static DataBaseInterface *getInstance();

    ~DataBaseInterface();

    /**
     * @brief execSql 添加查询sql语句
     * @param sql
     * @return
     */
    QSqlQuery execSql(const QString &sql);

    //! 创建数据库
    void createTable();


    UserInfo getLoginUserInfo(const quint64 & account);

    //! 获取所有用户信息
    QList<UserInfo> getUserInfoList();

    //! 插入用户信息
    bool insertUserInfo(const UserInfo & info);

    //! 获取所有菜品信息
    QList<CustomMenu> getCustomMenuList();

    //! 添加菜品
    bool insertCustomMenu(const CustomMenu & info);

    //! 删除菜品信息
    void deleteCustomMenu(const CustomMenu & info);


    //! 获取数据库所有订单信息
    QList<MenuOrderInfo> getMenuOrderList();

    //! 插入订单信息
    bool insertMenuOrderInfo(const MenuOrderInfo & info);

    //! 删除商家菜品信息
    void deleteMenuOrder(const MenuOrderInfo & info);


    //! 更新订单信息
    bool updateMenuOrderStatus(const MenuOrderInfo & info);

    //! 从数据库表删除订单信息
    void deleteMenuOrderInfo(const MenuOrderInfo & info );



private:
    explicit DataBaseInterface(QObject *parent = nullptr);

    /**
     * @brief init
     */
    void init();

    /**
     * @brief release
     */
    void release();

    /**
     * @brief initDatabase 初始化数据库
     */
    void initDatabase();

    /**
     * @brief dealData 处理数据
     * @param queue
     */
    void dealData(const SqlQueue &queue);

    /**
     * @brief reportSqlError 返回错误
     * @param query
     */
    void reportSqlError(QSqlQuery &query);



private:

    static DataBaseInterface            *m_instance;

    /**
     * @brief m_database 数据库对象
     */
    QSqlDatabase                        *m_database;

    QSqlQuery                           m_sqlQuery;
    QMutex                              m_dbMutex;
};

#endif // DATABASEINTERFACE_H
