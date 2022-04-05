#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include"user.h"
#include <QMainWindow>
QT_BEGIN_NAMESPACE
#include<QVector>
#include<QString>
#include<QPair>
#include<QTcpServer>
#include"CopyFileThread.h"
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    //QVector<QTcpSocket *> skts;
    QTcpServer *tcpServer;
    void setListener();
    QMap<QTcpSocket *,tranStatus> tableid;
    QVector<QPair<QString,int>> sendlist;

    QVector<CopyFileThread*> m_copiers;
    Rsatool  rsatool;
    Aestool aestool;
    User my;
    bool serverStatus;
public slots:
    void acceptConnection();
    void revData();
    void displayError(QAbstractSocket::SocketError);
private slots:
    void on_addFiles_clicked();
    void on_sendFiles_clicked();
    void on_updateaes_clicked();
    void on_updatetrsa_clicked();
    void on_onServer_clicked();
    void upsendresult(int row,int re,QString log);

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
