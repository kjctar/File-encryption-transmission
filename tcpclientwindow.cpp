#include "tcpclientwindow.h"
#include "ui_tcpclientwindow.h"
#include "aes.h"
#include "user.h"
#include<fstream>
#include<QFileDialog>
using namespace std;
TcpClientWindow::TcpClientWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TcpClientWindow)
{
    ui->setupUi(this);




}

    //接收字符串
void TcpClientWindow::revData(){

//    QString data=tcpSocket->readAll();
    ui->textBrowser->append("接到到文件信息");
    ui->textBrowser->append("正在解析文件");
    int k=0;
    QString  tmp;
    QString re;
    Aestool aestool;
    char buf[16]={0},out[16]={0};
    ofstream outFile("D:/e.mp4",ios::binary |ios::app);  //以二进制写模式打开文件
    while(tcpSocket->read(out,16)){
        assert(aestool.aes256_decrypt(out, buf)==0);  //解密

        for(int j=0;j<buf[15];j++){
            tmp+=buf[j];
            outFile.put(buf[j]);
        }
    }
    outFile.close();
    ui->textBrowser->append("传输完成");
    //正在接收文件

}

//新建连接
void TcpClientWindow::newTcpConnection(){
    tcpSocket->abort();
    tcpSocket->connectToHost("127.0.0.1",6666);
    qDebug()<<"client conn server"<<endl;


}

void TcpClientWindow::displayError(QAbstractSocket::SocketError){
    qDebug()<<tcpSocket->errorString();
    qDebug()<<"client conn server Fail"<<endl;
    tcpSocket->close();
}


TcpClientWindow::~TcpClientWindow()
{

    delete ui;
}

void TcpClientWindow::on_pushButton_clicked()
{


    QString data="555";
    QTcpSocket *tcpSocket=new QTcpSocket(this);

    tcpSocket->abort();
    tcpSocket->connectToHost("127.0.0.1",6666);
    //newTcpConnection();
    /**信号与槽**/

    //readyRead()表示服务端发送数据过来即发动信号，接着revData()进行处理。
    connect(tcpSocket,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(displayError(QAbstractSocket::SocketError)));
   // tcpSocket->write(ui->filepath->text().toUtf8());//发送文件数据
    Head head;

    char in[16]={0}, out[16]={0}, buf[16]={0};
    Aestool aestool;

    QFile file("D:/a.mp4");
    if (file.open(QIODevice::ReadOnly)) {
        qDebug() << file.size()<<endl;
        qDebug() << file.fileName()<<endl;
        strcpy(head.name,"a.mp4");

        head.size=file.size();
        qDebug() << head.name<<endl;
        qDebug() << head.size<<endl;
        file.close();
    } else {
        qDebug() << "文件打开失败！";
        return;
    }
    assert(sizeof(head)==128);
    tcpSocket->write((char*)&head,sizeof(head));
    char c;
    int k=0;
    ifstream inFile("D:/a.mp4",ios::binary |ios::in);  //以二进制读模式打开文件

    while (inFile.get(c))  //每次读取一个字符
    {


        in[k]=c;
        k++;
        if(k==15){
            in[15]=k;
            aestool.aes256_encrypt(in, out); //加密

            tcpSocket->write(out,16);

            k=0;
        }
    }
    if(k>0){
        in[15]=k;
        aestool.aes256_encrypt(in, out); //加密
        tcpSocket->write(out,16);
        k=0;
    }
     qDebug()<<"k=：" <<k<<endl;
    inFile.close();
    tcpSocket->close();
}
void TcpClientWindow::sendfile(QString path){


    QTcpSocket *tcpSocket=new QTcpSocket(this);

    tcpSocket->abort();
    tcpSocket->connectToHost("127.0.0.1",6666);
    //newTcpConnection();
    /**信号与槽**/
    //readyRead()表示服务端发送数据过来即发动信号，接着revData()进行处理。
    connect(tcpSocket,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(displayError(QAbstractSocket::SocketError)));
   // tcpSocket->write(ui->filepath->text().toUtf8());//发送文件数据
    Head head;

    char in[16]={0}, out[16]={0}, buf[16]={0};
    Aestool aestool;
    QFileInfo appInfo(path);
    QFile file(path);
    if (file.open(QIODevice::ReadOnly)) {
        qDebug() << file.size()<<endl;
        qDebug() << file.fileName()<<endl;


        strcpy(head.name,(appInfo.baseName()+"."+appInfo.suffix()).toUtf8());

        head.size=file.size();
        qDebug() << head.name<<endl;
        qDebug() << head.size<<endl;
        file.close();
    } else {
        qDebug() << "文件打开失败！";
        return;
    }
    assert(sizeof(head)==128);
    tcpSocket->write((char*)&head,sizeof(head));
    char c;
    int k=0;
    qDebug()<<path.toUtf8();
    qDebug()<<(appInfo.baseName()+"."+appInfo.suffix()).toLatin1();
    ifstream inFile(path.toUtf8(),ios::binary |ios::in);  //以二进制读模式打开文件

    while (inFile.get(c))  //每次读取一个字符
    {


        in[k]=c;
        k++;
        if(k==15){
            in[15]=k;
            aestool.aes256_encrypt(in, out); //加密

            tcpSocket->write(out,16);

            k=0;
        }
    }
    if(k>0){
        in[15]=k;
        aestool.aes256_encrypt(in, out); //加密
        tcpSocket->write(out,16);
        k=0;
    }
     qDebug()<<"k=：" <<k<<endl;
    inFile.close();
    tcpSocket->close();
}
void TcpClientWindow::on_pushButton_2_clicked()
{
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

            qDebug()<<fileNames[i]<<endl;

            sendfile(fileNames[i]);
        }
    }

}
