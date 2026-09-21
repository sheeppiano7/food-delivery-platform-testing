QT       += core gui sql network xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    commmsgmanager.cpp \
    databaseinterface.cpp \
    main.cpp \
    netudp/makedata.cpp \
    netudp/netudp.cpp \
    netudp/parsedata.cpp \
    netudp/udpcommmanager.cpp \
    widget.cpp

HEADERS += \
    CommDefines.h \
    PublicDefines.h \
    PublicDefines.h \
    commmsgmanager.h \
    databaseinterface.h \
    netudp/makedata.h \
    netudp/netudp.h \
    netudp/parsedata.h \
    netudp/udpcommmanager.h \
    widget.h

FORMS += \
    widget.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DESTDIR = build/
