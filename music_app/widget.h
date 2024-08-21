#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QTcpSocket>
#include <QMessageBox>
#include <QTimer>
#include <QJsonObject>
#include <QJsonDocument>

QT_BEGIN_NAMESPACE
namespace Ui {
class Widget;
}
QT_END_NAMESPACE

//#define IP "127.0.0.1"  //for macos
#define IP "18.185.92.160" //for ubuntu
#define PORT 8000

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private slots:
    void on_pushButton_clicked();
    void server_reply_slot();

private:
    Ui::Widget *ui;
    QTcpSocket *socket;
    void bind_operation();
};
#endif // WIDGET_H
