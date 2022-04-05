#ifndef TCPSERVERWINDOW_H
#define TCPSERVERWINDOW_H

#include <QDialog>
#include<QtNetwork/QTcpSocket>
#include<QtNetwork>
namespace Ui {
class TcpServerWindow;
}

class TcpServerWindow : public QDialog
{
    Q_OBJECT

public:
    explicit TcpServerWindow(QWidget *parent = nullptr);
    ~TcpServerWindow();
    QTcpSocket *tcpSocket;
    QTcpServer *tcpServer;
    void setListener();

public slots:
    void acceptConnection();
    void displayError(QAbstractSocket::SocketError);
private slots:
    void on_pushButton_clicked();

private:
    Ui::TcpServerWindow *ui;
};

#endif // TCPSERVERWINDOW_H
