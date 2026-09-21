#include "databaseinterface.h"

#include <QDebug>

#pragma execution_character_set("utf-8");
DataBaseInterface* DataBaseInterface::m_instance = nullptr;

DataBaseInterface *DataBaseInterface::getInstance()
{
    if(m_instance == nullptr)
    {
        m_instance = new DataBaseInterface();
    }
    return m_instance;
}

DataBaseInterface::~DataBaseInterface()
{
    release();
}

QSqlQuery DataBaseInterface::execSql(const QString &sql)
{
    QSqlQuery t_query;
    bool t_isQuery = false;
    SqlQueue queue;
    queue.sql = sql;
    queue.callback = [&](const bool &isQuery,const  QSqlQuery &query){
        t_isQuery = isQuery;
        t_query = query;
    };
    dealData(queue);
    return t_query;
}

void DataBaseInterface::createTable()
{

    QString createTableQuery = "CREATE TABLE IF NOT EXISTS userinof ("
                               "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                               "userId  VARCHAR(40) NOT NULL, "
                               "userName  VARCHAR(40) NOT NULL, "
                               "account  INTEGER UNIQUE, "
                               "password  INTEGER NOT NULL, "
                               "phoneNum  INTEGER NOT NULL, "
                               "address VARCHAR(40) NOT NULL, "
                               "usertype INTEGER NOT NULL, "
                               "onsalelevel INTEGER , "
                               "coupon INTEGER , "
                               "managernum INTEGER )";


    QSqlQuery query;

    if (!query.exec(createTableQuery)) {
        qDebug() << "Unable to create table:" << query.lastError();
    } else {
        qDebug() << "Table created successfully";
    }



    createTableQuery = "CREATE TABLE IF NOT EXISTS custommenu ("
                       "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                       "menuidx  INTEGER NOT NULL, "
                       "name  VARCHAR(40) NOT NULL, "
                       "price  REAL NOT NULL, "
                       "discribe  VARCHAR(100) NOT NULL, "
                       "imgurl  VARCHAR(100) NOT NULL, "
                       "uuid  VARCHAR(40) NOT NULL, "
                       "merchantname VARCHAR(40) NOT NULL)";

    QSqlQuery custommenuquery;

    if (!custommenuquery.exec(createTableQuery)) {
        qDebug() << "Unable to create table:" << custommenuquery.lastError();
    } else {
        qDebug() << "Table created successfully";
    }

    createTableQuery = "CREATE TABLE IF NOT EXISTS menuorder ("
                       "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                       "orderid  INTEGER NOT NULL, "
                       "orderuuid  VARCHAR(40) NOT NULL, "
                       "customerphonenum  INTEGER NOT NULL, "
                       "customeraddress  VARCHAR(100) NOT NULL, "
                       "merchantaddress VARCHAR(40) NOT NULL, "
                       "customeruuid VARCHAR(40) NOT NULL, "
                       "price REAL NOT NULL, "
                       "orderstatu INTEGER NOT NULL, "
                       "name VARCHAR(40) NOT NULL, "
                       "orderdate VARCHAR(40) NOT NULL, "
                       "merchantname VARCHAR(40) NOT NULL)";

    QSqlQuery menuorderquery;

    if (!menuorderquery.exec(createTableQuery)) {
        qDebug() << "Unable to create table:" << menuorderquery.lastError();
    } else {
        qDebug() << "Table created successfully";
    }
}


bool DataBaseInterface::insertUserInfo(const UserInfo &info)
{
    QSqlQuery query;
    query.prepare("INSERT INTO userinof (userId,userName,account,password,phoneNum,address,usertype,onsalelevel,coupon,managernum) VALUES (?,?,?,?,?,?,?,?,?,?)");
    query.addBindValue(QVariant::fromValue(info.userId));
    query.addBindValue(QVariant::fromValue(info.userName));
    query.addBindValue(QVariant::fromValue(info.account));
    query.addBindValue(QVariant::fromValue(info.password));
    query.addBindValue(QVariant::fromValue(info.phoneNum));
    query.addBindValue(QVariant::fromValue(info.address));
    query.addBindValue(static_cast<int>(info.usertype));
    query.addBindValue(QVariant::fromValue(info.onsalelevel));
    query.addBindValue(QVariant::fromValue(info.coupon));
    query.addBindValue(QVariant::fromValue(info.managernum));
    if (!query.exec()) { qWarning() << query.lastError(); return false; }
    return true;
}


bool DataBaseInterface::insertCustomMenu(const CustomMenu &info)
{
    QSqlQuery query;
    query.prepare("INSERT INTO custommenu (menuidx,name,price,discribe,imgurl,uuid,merchantname) VALUES (?,?,?,?,?,?,?)");
    query.addBindValue(QVariant::fromValue(info.menuidx));
    query.addBindValue(QVariant::fromValue(info.name));
    query.addBindValue(QVariant::fromValue(info.price));
    query.addBindValue(QVariant::fromValue(info.discribe));
    query.addBindValue(QVariant::fromValue(info.imgurl));
    query.addBindValue(QVariant::fromValue(info.uuid));
    query.addBindValue(QVariant::fromValue(info.merchantname));
    if (!query.exec()) { qWarning() << query.lastError(); return false; }
    return true;
}

void DataBaseInterface::deleteCustomMenu(const CustomMenu &info)
{
    QSqlQuery query;
    query.prepare("DELETE FROM custommenu WHERE uuid = ?");
    query.addBindValue(info.uuid);
    if (!query.exec()) qWarning() << query.lastError();
}


UserInfo DataBaseInterface::getLoginUserInfo(const quint64 &account)
{
    UserInfo info;
    QString strSql = QString("SELECT * FROM userinfo WHERE account = %1 ").arg(account);
    QSqlQuery query = DataBaseInterface::getInstance()->execSql(strSql);
    while (query.next()) {


        info.userId = query.value(1).toString();
        info.userName = query.value(2).toString();
        info.account = query.value(3).toUInt();
        info.password = query.value(4).toUInt();
        info.phoneNum = query.value(5).toUInt();
        info.address = query.value(6).toString();
        info.usertype = (UserType)query.value(7).toInt();
        info.onsalelevel = query.value(8).toUInt();
        info.coupon = query.value(9).toUInt();
        info.managernum = query.value(10).toUInt();

        if(query.value(3).toUInt() == account)
        {
            break;
        }

    }
    return  info;

}

QList<UserInfo> DataBaseInterface::getUserInfoList()
{
    QList<UserInfo> userinfolist;
    QString strSql = QString("SELECT * FROM userinof");
    QSqlQuery query = DataBaseInterface::getInstance()->execSql(strSql);
    while (query.next()) {
        UserInfo info;

        info.userId = query.value(1).toString();
        info.userName = query.value(2).toString();
        info.account = query.value(3).toUInt();
        info.password = query.value(4).toUInt();
        info.phoneNum = query.value(5).toUInt();
        info.address = query.value(6).toString();
        info.usertype = (UserType)query.value(7).toInt();
        info.onsalelevel = query.value(8).toUInt();
        info.coupon = query.value(9).toUInt();
        info.managernum = query.value(10).toUInt();

        userinfolist.push_back(info);

    }
    return  userinfolist;
}

QList<CustomMenu> DataBaseInterface::getCustomMenuList()
{
    QList<CustomMenu> infoList;
    QString strSql = QString("SELECT * FROM custommenu");
    QSqlQuery query = DataBaseInterface::getInstance()->execSql(strSql);
    while (query.next()) {
        CustomMenu info;

        info.menuidx = query.value(1).toUInt();
        info.name = query.value(2).toString();
        info.price = query.value(3).toDouble();
        info.discribe = query.value(4).toString();
        info.imgurl = query.value(5).toString();
        info.uuid = query.value(6).toString();
        info.merchantname = query.value(7).toString();

        infoList.push_back(info);

    }
    return  infoList;
}

QList<MenuOrderInfo> DataBaseInterface::getMenuOrderList()
{
    QList<MenuOrderInfo> infoList;
    QString strSql = QString("SELECT * FROM menuorder");
    QSqlQuery query = DataBaseInterface::getInstance()->execSql(strSql);
    while (query.next()) {
        MenuOrderInfo info;

        info.orderid = query.value(1).toUInt();
        info.orderuuid = query.value(2).toString();
        info.customerphonenum = query.value(3).toULongLong();
        info.customeraddress = query.value(4).toString();
        info.merchantaddress = query.value(5).toString();
        info.customerUuid = query.value(6).toString();
        info.price = query.value(7).toDouble();
        info.orderstatus = (MenuOrderStatus)query.value(8).toUInt();
        info.name = query.value(9).toString();
        info.orderdate = query.value(10).toString();
        info.merchantname = query.value(11).toString();

        infoList.push_back(info);

    }
    return  infoList;
}


void DataBaseInterface::deleteMenuOrder(const MenuOrderInfo &info)
{
    QSqlQuery query;
    query.prepare("DELETE FROM menuorder WHERE orderuuid = ?");
    query.addBindValue(info.orderuuid);
    if (!query.exec()) qWarning() << query.lastError();
}

bool DataBaseInterface::insertMenuOrderInfo(const MenuOrderInfo &info)
{
    QSqlQuery query;
    query.prepare("INSERT INTO menuorder (orderid,orderuuid,customerphonenum,customeraddress,merchantaddress,customeruuid,price,orderstatu,name,orderdate,merchantname) VALUES (?,?,?,?,?,?,?,?,?,?,?)");
    query.addBindValue(QVariant::fromValue(info.orderid));
    query.addBindValue(QVariant::fromValue(info.orderuuid));
    query.addBindValue(QVariant::fromValue(info.customerphonenum));
    query.addBindValue(QVariant::fromValue(info.customeraddress));
    query.addBindValue(QVariant::fromValue(info.merchantaddress));
    query.addBindValue(QVariant::fromValue(info.customerUuid));
    query.addBindValue(QVariant::fromValue(info.price));
    query.addBindValue(static_cast<int>(info.orderstatus));
    query.addBindValue(QVariant::fromValue(info.name));
    query.addBindValue(QVariant::fromValue(info.orderdate));
    query.addBindValue(QVariant::fromValue(info.merchantname));
    if (!query.exec()) { qWarning() << query.lastError(); return false; }
    return true;
}

bool DataBaseInterface::updateMenuOrderStatus(const MenuOrderInfo &info)
{
    bool rslt = false;
    QString insertDataQuery = QString("UPDATE custommenu SET orderstatu = 1% WHERE orderuuid = '%2'").arg(info.orderstatus).arg(info.orderuuid);
     QSqlQuery query;

    if (!query.exec(insertDataQuery)) {
        qDebug() << "Unable to update data:" << query.lastError();

    } else {
        rslt = true;
        qDebug() << "Data update successfully";
    }
    return rslt;
}

void DataBaseInterface::deleteMenuOrderInfo(const MenuOrderInfo &info)
{
    QString strSql = QString("DELETE FROM custommenu WHERE  orderuuid = '%1' ").arg(info.orderuuid);
    QSqlQuery query = DataBaseInterface::getInstance()->execSql(strSql);
}



DataBaseInterface::DataBaseInterface(QObject *parent) : QObject(parent)
{

    init();
}

void DataBaseInterface::init()
{
    qRegisterMetaType<std::function<void()>>("std::function<void()>");
    initDatabase();
}

void DataBaseInterface::release()
{
    if(m_database)
    {
        m_database->close();
        delete m_database;
        m_database = nullptr;
    }
}

void DataBaseInterface::initDatabase()
{
    qDebug() << "SqlDatabase::drivers: " << QSqlDatabase::drivers();
    m_database = new QSqlDatabase();
    // 设置数据库类型
    *m_database = QSqlDatabase::addDatabase("QSQLITE");
    //    得到实例对象之后需要初始化连接信息
    m_database->setHostName("127.0.0.1");
    m_database->setPort(3306);
    m_database->setDatabaseName("CustomMenuSys.db");
    //  连接数据库
    bool bl = m_database->open();
    if(!bl)
    {
        // 数据库连接失败
        qDebug() << m_database->lastError().text();
    }
}

void DataBaseInterface::dealData(const SqlQueue &queue)
{
    if(!queue.sql.isEmpty())
    {
        ///事务
        m_database->transaction();
        ///执行并回调
        QMutexLocker locker(&m_dbMutex);
        QSqlQuery query(*m_database);
        bool isExex = true;
        if (!query.exec(queue.sql)){
            reportSqlError(query);
            m_database->rollback();
            isExex = false;
        }
        queue.callback(isExex, query);
        m_database->commit();
    }
    else{
        qDebug()<<"DatabaseQueue::dealQueue error:"<<queue.sql;
    }
}

void DataBaseInterface::reportSqlError(QSqlQuery &query)
{
    qDebug()<<"ERROR:"<< query.lastError().text()<<" DataBase: sql:"<<query.lastQuery();
}
