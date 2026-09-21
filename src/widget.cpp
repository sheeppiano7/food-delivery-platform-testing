#include "widget.h"
#include "ui_widget.h"
#include "databaseinterface.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    DataBaseInterface::getInstance()->createTable();

    m_pCommMsgManager =  new CommMsgManager();
}

Widget::~Widget()
{
    delete ui;
}
