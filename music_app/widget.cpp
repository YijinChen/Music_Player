#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    socket = new QTcpSocket;
    socket -> connectToHost(IP, PORT);
    connect(socket, &QTcpSocket::connected, [this](){
        QMessageBox::information(this, "connect message", "connect successfully");
    });
}

Widget::~Widget()
{
    delete ui;
}
