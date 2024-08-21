#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    socket = new QTcpSocket;
    socket -> connectToHost(IP, PORT);

    qDebug() << "Attempting to connect signals...";

    connect(socket, &QTcpSocket::connected, this, [this]() {
        QMessageBox::information(this, "connect message", "connect successfully");
    });

    connect(socket, &QTcpSocket::errorOccurred, this, [this](QAbstractSocket::SocketError socketError) {
        qDebug() << "Socket error:" << socketError;
        QMessageBox::critical(this, "Socket Error", socket->errorString());
    });

    qDebug() << "Signals connected, initiating connection...";

    QTimer::singleShot(5000, this, [this]() {
        qDebug() << "Socket state after 5 second:" << socket->state();
        if (socket->state() == QAbstractSocket::UnconnectedState) {
            qDebug() << "Failed to connect. Error:" << socket->errorString();
        }
    });

    connect(socket, &QTcpSocket::readyRead, this, &Widget::server_reply_slot);
}

Widget::~Widget()
{
    delete ui;
}

void Widget::on_pushButton_clicked()
{
    QString app_id = ui->appLineEdit->text();
    QJsonObject obj;
    obj.insert("cmd", "search_bind");
    obj.insert("appid", app_id);

    //send data to server, check if the app_id already bound
    QByteArray ba = QJsonDocument(obj).toJson();
    socket->write(ba);
}

void Widget::server_reply_slot(){
    QByteArray ba = socket->readAll();
    QJsonObject obj = QJsonDocument::fromJson(ba).object();

    QString cmd = obj.value("cmd").toString();
    if(cmd == "reply_bind"){
        QString result = obj.value("result").toString();
        if(result == "yes"){
            qDebug() << "this app_id is already bound";

            //disconnect socket and widget __connect(socket, &QTcpSocket::readyRead, this, &Widget::server_reply_slot);
            disconnect(socket, 0, this, 0);

            //skip to new page
            Player *p = new Player(socket);
            p->show();
            this->close();
        }
        else if(result == "no"){
            qDebug() << "this app_id is not yet bound";
            bind_operation();
        }
    }
    else if(cmd == "bind_success"){
        qDebug() << "bind successfully";

        //disconnect socket and widget __connect(socket, &QTcpSocket::readyRead, this, &Widget::server_reply_slot);
        disconnect(socket, 0, this, 0);

        //skip to new page
        Player *p = new Player(socket);
        p->show();
        this->close();
    }
}


void Widget::bind_operation(){
    QString app_id = ui->appLineEdit->text();
    QString dev_id = ui->devLineEdit->text();

    QJsonObject obj;
    obj.insert("cmd", "bind");
    obj.insert("appid", app_id);
    obj.insert("deviceid", dev_id);

    //transfer QJsonObject to QByteArray
    QByteArray ba = QJsonDocument(obj).toJson();
    socket->write(ba);
}

