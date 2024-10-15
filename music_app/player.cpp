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


    //set timer, send app_music after 5 seconds (So the server have enough time to update the status of player)
    timer.start(5000);
    connect(&timer, &QTimer::timeout, this, &Player::timeout_slot);
}

Player::~Player()
{
    delete ui;
}

void Player::timeout_slot(){
    //send app_music to server
    QJsonObject obj;
    obj.insert("cmd", "app_music");
    QByteArray ba = QJsonDocument(obj).toJson();
    socket->write(ba);

    timer.stop(); //close the timer
}

void Player::server_reply_slot(){
    QByteArray ba = socket->readAll();
    QJsonObject obj = QJsonDocument::fromJson(ba).object();
    QString cmd = obj.value("cmd").toString();

    if(cmd == "app_reply"){         //player operation
        QString result = obj.value("result").toString();
        if(result == "start_success"){
            ui->startButton->setText("| |");  //If the player successfully played, change the \>(start) symbol to ||(suspend)
            play_flag = START_PLAY;
            QString music_name = obj.value("music").toString();
            ui->curLabel->setText(music_name);
            int level = obj.value("volume").toInt();
            ui->volumeLabel->setText(QString::number(level) + "%");
        }
        else if(result == "suspend_success"){
            ui->startButton->setText("| >");
            play_flag = SUSPEND_PLAY;
        }
        else if(result == "resume_success"){
            ui->startButton->setText("| |");
            play_flag = START_PLAY;
        }
        else if(result == "volume_success"){
            int level = obj.value("volume").toInt();
            ui->volumeLabel->setText(QString::number(level) + "%");
        }
        else if(result == "previous_success"){
            QString music_name = obj.value("music").toString();
            ui->curLabel->setText(music_name);
        }
        else if(result == "next_success"){
            QString music_name = obj.value("music").toString();
            ui->curLabel->setText(music_name);
        }
        else if(result == "off_line"){
            QMessageBox::warning(this, "operation message", "music player is offline");
        }
    }
    else if(cmd  == "app_reply_status"){    //player status
        //mode, music name, volume
        QString status = obj.value("status").toString();
        if(status == "start"){   //status of music player is "play"
            if(play_flag == SUSPEND_PLAY){   //status of app is "suspend"
                play_flag = START_PLAY;
                ui->startButton->setText("| |");
            }
        }
        else if(status == "suspend" || status == "stop"){ //status of music player is "stop" or "suspend"
            if(play_flag == START_PLAY){   //status of app is "start"
                play_flag = SUSPEND_PLAY;
                ui->startButton->setText("| >");
            }
        }
        QString music_name = obj.value("music").toString();
        if(music_name != "|"){
            ui->curLabel->setText(music_name);
        }

        int level = obj.value("volume").toInt();
        ui->volumeLabel->setText(QString::number(level) + "%");

    }
    else if (cmd == "app_reply_music"){    //get all music
        qDebug() << "Received app_reply_music....";
        QJsonArray arr = obj.value("music").toArray();
        QString result;
        for (int i = 0; i < arr.count(); i++){
            result.append(arr.at(i).toString());
            result.append('\n');
        }
        ui->musicEdit->setText(result);
        ui->curLabel->setText("No music playing ...");
        ui->volumeLabel->setText("0%");
        ui->seqButton->setChecked(true);
    }
}

void Player::on_startButton_clicked()
{
    if(play_flag == SUSPEND_PLAY){
        QJsonObject obj;
        obj.insert("cmd", "app_start");
        QByteArray ba = QJsonDocument(obj).toJson();
        socket->write(ba);
        ui->startButton->setText("| |");
    }
    else if(play_flag == START_PLAY){
        QJsonObject obj;
        obj.insert("cmd", "app_suspend");
        QByteArray ba = QJsonDocument(obj).toJson();
        socket->write(ba);
        ui->startButton->setText("| >");
    }
}


void Player::on_priorButton_clicked()
{
    QJsonObject obj;
    obj.insert("cmd", "app_previous");
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

void Player::closeEvent(QCloseEvent *event){
    QJsonObject obj;
    obj.insert("cmd", "app_off_line");
    QByteArray ba = QJsonDocument(obj).toJson();
    socket->write(ba);
    socket->waitForBytesWritten(); //wait for the socket finished writing then close the page
    event->accept(); //close the page
}

