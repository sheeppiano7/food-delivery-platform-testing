#ifndef PARSEDATA_H
#define PARSEDATA_H

#include <QObject>
#include "PublicDefines.h"
#include "CommDefines.h"
class parsedata : public QObject
{
    Q_OBJECT
public:
    explicit parsedata(QObject *parent = nullptr);

        static bool execute(const char *data, int len, UserInfo & info);

        static bool execute(const char *data, int len, QList<UserInfo> & userinfolist);

        static bool execute(const char *data, int len, CustomMenu & info);

        static bool execute(const char *data, int len, QList<CustomMenu> & customMenuList);

        static bool execute(const char *data, int len, QList<MenuOrderInfo> & customMenuList);

signals:

};

#endif // PARSEDATA_H
