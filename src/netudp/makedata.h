#ifndef MAKEDATA_H
#define MAKEDATA_H

#include <QObject>
#include "CommDefines.h"
#include "PublicDefines.h"

class makedata : public QObject
{
    Q_OBJECT
public:
    explicit makedata(QObject *parent = nullptr);

    static QByteArray makeCustomMsg(const CommCustomMsg & info);

    static QByteArray makeSignalUserinfoData(const UserInfo & info);

    static QByteArray makeUserinfoListData(const QList<UserInfo> & infoList);

    static QByteArray makeCustomMenuListData(const QList<CustomMenu> & infoList);

    static QByteArray makeCustomMenuOrderListData(const QList<MenuOrderInfo> & infoList);


};

#endif // MAKEDATA_H
