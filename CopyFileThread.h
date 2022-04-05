#ifndef FILESENDNODE_H
#define FILESENDNODE_H

#endif // FILESENDNODE_H
#include<QThread>
#include<QTcpSocket>
#include"aes.h"
#include"rsa.h"
class CopyFileThread: public QThread
{
    Q_OBJECT
public:
    CopyFileThread(QObject * parent = 0){

    }
    void set(int i,QString p,QString ipp,int po,Rsatool rsatool,int uid){
        row=i;
        path=p;
        ip=ipp;
        port=po;
        this->rsatool=rsatool;
        this->uid=uid;

    }
    int row;
    QString path;
    QString ip;
    int port;
    QTcpSocket *tcpSocket;
    Aestool aestool;
    Rsatool rsatool;
    QString log;
    void putPubkey();//构造head ，文件信息和rsa公钥，并发送
    bool getAesKey(); //等待接收加密的对称秘钥，并解码
    bool initConnect();//与对方建立socket连接
    int uid;
signals:
    void sendresult(int,int,QString);
protected:
    void run(); // 新线程入口，发送文件数据

// 省略掉一些内容
};
