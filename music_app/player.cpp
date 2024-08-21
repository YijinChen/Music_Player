#include "player.h"
#include "ui_player.h"

Player::Player(QTcpSocket *s, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Player)
{
    ui->setupUi(this);
    socket = s;

    play_flag = SUSPEND_PLAY;

    connect(socket, &QTcpSocket::readyRead, this, &Player::server_reply_slot);

    //radioButton (Mode Button) signal and slots
    connect(ui->seqButton, &QRadioButton::clicked, this, &Player::on_seqButton_clicked);
    connect(ui->randomButton, &QRadioButton::clicked, this, &Player::on_randomButton_clicked);
    connect(ui->circleButton, &QRadioButton::clicked, this, &Player::on_circleButton_clicked);

}

Player::~Player()
{
    delete ui;
}

void Player::server_reply_slot(){
    QByteArray ba = socket->readAll();
    QJsonObject obj = QJsonDocument::fromJson(ba).object();
    QString cmd = obj.value("cmd").toString();

    if(cmd == "app_reply"){         //player operation
        QString result = obj.value("result").toString();
        if(result == "start_success"){
            ui->startButton->setText("||");  //If the player successfully played, change the \>(start) symbol to ||(suspend)
            play_flag = START_PLAY;
        }
        else if(result == "suspend_success"){
            ui->startButton->setText("|>");
            play_flag = SUSPEND_PLAY;
        }
        else if(result == "continue_success"){
            ui->startButton->setText("||");
            play_flag = START_PLAY;
        }
        else if(result == "off_line"){
            QMessageBox::warning(this, "operation message", "music player is offline");
        }
    }
    else if(cmd  == "app_reply_status"){    //player status

    }
    else if (cmd == "app_rely_music"){    //get all music

    }
}

void Player::on_startButton_clicked()
{
    if(play_flag == SUSPEND_PLAY){
        QJsonObject obj;
        obj.insert("cmd", "app_start");
        QByteArray ba = QJsonDocument(obj).toJson();
        socket->write(ba);
    }
    else if(play_flag == START_PLAY){
        QJsonObject obj;
        obj.insert("cmd", "app_suspend");
        QByteArray ba = QJsonDocument(obj).toJson();
        socket->write(ba);
    }
}


void Player::on_priorButton_clicked()
{
    QJsonObject obj;
    obj.insert("cmd", "app_prior");
    QByteArray ba = QJsonDocument(obj).toJson();
    socket->write(ba);

}


void Player::on_nextButton_clicked()
{
    QJsonObject obj;
    obj.insert("cmd", "app_next");
    QByteArray ba = QJsonDocument(obj).toJson();
    socket->write(ba);
}


void Player::on_priorButton_4_clicked()
{
    QJsonObject obj;
    obj.insert("cmd", "app_volume_up");
    QByteArray ba = QJsonDocument(obj).toJson();
    socket->write(ba);
}


void Player::on_downButton_clicked()
{
    QJsonObject obj;
    obj.insert("cmd", "app_volume_down");
    QByteArray ba = QJsonDocument(obj).toJson();
    socket->write(ba);
}


void Player::on_seqButton_clicked()
{
    QJsonObject obj;
    obj.insert("cmd", "app_sequence");
    QByteArray ba = QJsonDocument(obj).toJson();
    socket->write(ba);
}



void Player::on_randomButton_clicked()
{
    QJsonObject obj;
    obj.insert("cmd", "app_random");
    QByteArray ba = QJsonDocument(obj).toJson();
    socket->write(ba);
}


void Player::on_circleButton_clicked()
{
    QJsonObject obj;
    obj.insert("cmd", "app_circle");
    QByteArray ba = QJsonDocument(obj).toJson();
    socket->write(ba);
}

