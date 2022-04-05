#ifndef TCPCLIENTWINDOW_H
#define TCPCLIENTWINDOW_H

#include <QDialog>
#include<QtNetwork/QTcpSocket>
#include<QtNetwork>
namespace Ui {
class TcpClientWindow;
}

class TcpClientWindow : public QDialog
{
    Q_OBJECT

public:
    explicit TcpClientWindow(QWidget *parent = nullptr);
    ~TcpClientWindow();
    QTcpSocket *tcpSocket;
private:
    Ui::TcpClientWindow *ui;
    void newTcpConnection();

private slots:
    void displayError(QAbstractSocket::SocketError);
    void revData();
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
    void sendfile(QString path);
};

#endif // TCPCLIENTWINDOW_H
