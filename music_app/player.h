#ifndef PLAYER_H
#define PLAYER_H

#include <QWidget>
#include <QTcpSocket>
#include <QJsonObject>
#include <QJsonDocument>
#include <QMessageBox>
#include <QJsonArray>
#include <QTimer>
#include <QCloseEvent>

namespace Ui {
class Player;
}

#define START_PLAY 1
#define SUSPEND_PLAY 2

class Player : public QWidget
{
    Q_OBJECT

public:
    explicit Player(QTcpSocket *s, QWidget *parent = nullptr);
    ~Player();
    void closeEvent(QCloseEvent *event);

private slots: //slots function
    void server_reply_slot();


    void on_startButton_clicked();

    void on_priorButton_clicked();

    void on_nextButton_clicked();

    void on_priorButton_4_clicked();

    void on_downButton_clicked();

    void on_seqButton_clicked();

    void on_randomButton_clicked();

    void on_circleButton_clicked();

    void timeout_slot();

private:
    Ui::Player *ui;
    QTcpSocket *socket;
    int play_flag;
    QTimer timer;


};

#endif // PLAYER_H
