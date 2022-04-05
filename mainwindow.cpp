#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <openssl/ssl.h>
#include <QFile>
#include <QDebug>

#include <QString>
#include <openssl/ssl.h>
#include <openssl/sha.h>
#include <openssl/aes.h>

#include <iostream>
#include <fstream>
#include<string>
#include<QFileDialog>
#include<QDateTime>
#include<QThread>

using namespace std;
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //启动监听但是暂停接收连接
    serverStatus=false;
    tcpServer=new QTcpServer(this);//初始化文件接收服务器
    if(!tcpServer->listen(QHostAddress::Any,6666)){
         qDebug()<<tcpServer->errorString();
        close();
    }else {
        qDebug()<<"listening.....................";
    }
    connect(tcpServer,SIGNAL(newConnection()),this,SLOT(acceptConnection()));
    tcpServer->pauseAccepting();

    rsatool.rsaKeyInit();//初始化非对称密钥
    aestool.keyInit();//初始化对称密钥
    ui->aesview->setText(aestool.key);
    ui->rsapriview->setText(rsatool.priKey);
    ui->rsapubview->setText(rsatool.pubKey);


    ui->recvTable->setColumnCount(6);
    ui->recvTable->setRowCount(0);
    ui->recvTable->setHorizontalHeaderLabels(QStringList()<<"接收方"<<"发送方"<<"文件名"<<"文件大小"<<"时间"<<"进度");
    ui->recvTable->setSelectionBehavior(QAbstractItemView::SelectRows);  //整行选中的方式
    ui->recvTable->setEditTriggers(QAbstractItemView::NoEditTriggers);   //禁止修改
    ui->recvTable->setSelectionMode(QAbstractItemView::SingleSelection);  //设置为可以选中单个
    ui->recvTable->verticalHeader()->setVisible(false);   //隐藏列表头
    ui->recvTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->recvTable->selectRow(0);

    ui->sendTable->setColumnCount(2);
    ui->sendTable->setRowCount(0);
    ui->sendTable->setHorizontalHeaderLabels(QStringList()<<"文件"<<"进度");
    ui->sendTable->setSelectionBehavior(QAbstractItemView::SelectRows);  //整行选中的方式
    ui->sendTable->setEditTriggers(QAbstractItemView::NoEditTriggers);   //禁止修改
    ui->sendTable->setSelectionMode(QAbstractItemView::SingleSelection);  //设置为可以选中单个
    ui->sendTable->verticalHeader()->setVisible(false);   //隐藏列表头
    ui->sendTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

}

void MainWindow::displayError(QAbstractSocket::SocketError)
{
    QTcpSocket *tcpSocket = qobject_cast<QTcpSocket *>(sender());
    qDebug()<<tcpSocket->errorString();
}


void MainWindow::acceptConnection(){

    QTcpSocket *tcpSocket=new QTcpSocket(this);
    tcpSocket=tcpServer->nextPendingConnection();
    connect(tcpSocket,SIGNAL(readyRead()),this,SLOT(revData()));
    connect(tcpSocket,SIGNAL(error(QAbstractSocket::SocketError)),SLOT(displayError(QAbstractSocket::SocketError)));

}
//接收字符串
void MainWindow::revData(){
    QTcpSocket *tcpSocket = qobject_cast<QTcpSocket *>(sender());
    QString  tmp;
    QString re;
    char buf[16]={0},out[16]={0};
    char tou[sizeof(Head)];
    if(tableid.count(tcpSocket)==0){//判断是不是第一次触发。如果是就解析head信息
        int cols=ui->recvTable->columnCount();
        int rows=ui->recvTable->rowCount();

        tranStatus temp;
        Head head;
        QString path="D:/recv/";
        tcpSocket->read(tou,sizeof(Head));
        memcpy(&head, tou, sizeof(head));
        QString mid="";
        QFileInfo info((path+mid+QString(head.name)));
        while(info.exists()){//判断是否出现重名文件
            mid=QString::number(rand()%100000);
            info.setFile(path+mid+QString(head.name));
        }

        tableid[tcpSocket].name=(path+mid+QString(head.name));
        tableid[tcpSocket].size=head.size;
        tableid[tcpSocket].row=rows;
        tableid[tcpSocket].step=0;
        ui->recvTable->insertRow(rows);
        //更新界面表格数据
        ui->recvTable->setItem(rows,0,new QTableWidgetItem(tcpSocket->localAddress().toString()));//接收
        ui->recvTable->setItem(rows,1,new QTableWidgetItem(tcpSocket->peerAddress().toString()));//发送
        ui->recvTable->setItem(rows,2,new QTableWidgetItem(tableid[tcpSocket].name));//文件名
        ui->recvTable->setItem(rows,3,new QTableWidgetItem(QString::number(tableid[tcpSocket].size)));//文件大小
        ui->recvTable->setItem(rows,4,new QTableWidgetItem(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss ddd")));//时间
        ui->recvTable->setItem(rows,5,new QTableWidgetItem(QString::number(tableid[tcpSocket].step/tableid[tcpSocket].size)+"%"));
        strcpy(head.key,rsatool.rsaPubEncrypt(aestool.key,head.key).toUtf8());//将自己的对称密钥用公钥加密发给对方
        tcpSocket->write((char*)&head,sizeof(head));
}

    ofstream outFile(tableid[tcpSocket].name.toLocal8Bit(),ios::binary |ios::app);  //以二进制写模式打开文件
    while(tcpSocket->read(out,16)){//每次读取16个字节
        assert(aestool.aes256_decrypt(out, buf)==0);  //解密

        for(int j=0;j<buf[15];j++){
            tmp+=buf[j];
            outFile.put(buf[j]);
            tableid[tcpSocket].step++;//文件传输进度
        }
    }
    outFile.close();
    //更新进度
    ui->recvTable->setItem(tableid[tcpSocket].row,5,new QTableWidgetItem(QString::number(tableid[tcpSocket].step)+"/"+QString::number(tableid[tcpSocket].size)));

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_addFiles_clicked()
{
    int cols=ui->sendTable->columnCount();
    int rows=ui->sendTable->rowCount();
    QFileDialog *fileDialog = new QFileDialog(this);
    //定义文件对话框标题
    fileDialog->setWindowTitle(QStringLiteral("选中文件"));
    //设置默认文件路径
    fileDialog->setDirectory(".");
    //设置文件过滤器
    fileDialog->setNameFilter(tr("File(*.*)"));
    //设置可以选择多个文件,默认为只能选择一个文件QFileDialog::ExistingFiles
    fileDialog->setFileMode(QFileDialog::ExistingFiles);
    //设置视图模式
    fileDialog->setViewMode(QFileDialog::Detail);
    //打印所有选择的文件的路径
    QStringList fileNames;
    if (fileDialog->exec()) {
        fileNames = fileDialog->selectedFiles();
        for(int i=0;i<fileNames.size();i++)
        {
            sendlist.append({fileNames[i],0});
            ui->sendTable->insertRow(rows);
            qDebug()<<fileNames[i]<<endl;
            ui->sendTable->setItem(rows,i,new QTableWidgetItem("new"+QString::number(rows)));
             ui->sendTable->selectRow(rows);
             //ui->sendTable->setItem(rows,0,new QTableWidgetItem(tcpSocket->localAddress().toString()));//接收


             ui->sendTable->setItem(rows,0,new QTableWidgetItem(fileNames[i]));//文件名
             ui->sendTable->setItem(rows,1,new QTableWidgetItem("null"));

             rows++;
            //sendfile(fileNames[i]);
        }
    }
}
void MainWindow::on_sendFiles_clicked()
{
    for(int i=0;i<sendlist.size();i++){


        if(sendlist[i].second==0){
            ui->sendTable->setItem(i,1,new QTableWidgetItem("正在发送"));

            m_copiers.push_back(new CopyFileThread);

            m_copiers[m_copiers.size()-1]->set(i,sendlist[i].first,ui->Ipinput->text(),6666,rsatool,my.uid);//设置发送文件所需参数
            //m_copiers[m_copiers.size()-1]->initConnect();//建立连接，交换对称密钥
            connect( m_copiers[m_copiers.size()-1], SIGNAL(sendresult(int,int,QString)), this, SLOT(upsendresult(int,int,QString)));//建立发送结果回调

            m_copiers[m_copiers.size()-1]->start();

        }

    }
}





void MainWindow::upsendresult(int row,int re,QString log){
    if(re==1){
        ui->sendTable->setItem(row,1,new QTableWidgetItem("发送完成"));


    }else{
         ui->sendTable->setItem(row,1,new QTableWidgetItem("发送失败"));
    }
     ui->sendlog->append(log);
     sendlist[row].second=1;
}
void MainWindow::on_updateaes_clicked()
{
    aestool.keyInit();
    ui->aesview->setText(aestool.key);

}

void MainWindow::on_updatetrsa_clicked()
{
    rsatool.rsaKeyInit();
    ui->rsapriview->setText(rsatool.priKey);
    ui->rsapubview->setText(rsatool.pubKey);
}

void MainWindow::on_onServer_clicked()
{

    if(serverStatus==false){
        tcpServer->resumeAccepting();
        ui->onServer->setText("正在监听....");
        serverStatus=true;
    }else{
         tcpServer->pauseAccepting();
         ui->onServer->setText("启动监听");
         serverStatus=false;
    }


}
