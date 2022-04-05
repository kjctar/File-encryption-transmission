#include"CopyFileThread.h"
#include<QTcpSocket>
#include"user.h"
#include<QFileInfo>
#include<fstream>
using namespace  std;
bool CopyFileThread::getAesKey(){
    Head msg;
    char buffer[sizeof(Head)];
    tcpSocket->waitForReadyRead(2000);
    tcpSocket->read(buffer,sizeof(Head));
    memcpy(&msg, buffer, sizeof(msg));
   // qDebug()<<"client recv rsaPub deal of Aeskey : "<<msg.key<<endl;
    log+="recive "+ip+"  Aeskey by rsaPub dealed : "+QString(msg.key)+"\n";
    QString Aeskey=rsatool.rsaPriDecrypt(msg.key,rsatool.priKey);
    if(Aeskey.size()==0){
        return false;
    }
    // qDebug()<<"pri deal Aes key:"<<Aeskey<<endl;
    log+="decoded "+ip+" of Aeskey : "+Aeskey+"\n";
     aestool.setKey(Aeskey);
     return true;
}
void CopyFileThread::putPubkey(){
    Head head;
    qDebug()<<"tcpSocket"<<tcpSocket->state();
    if(tcpSocket->state()!=3){
       sendresult(row,0,log);
    }


    QFileInfo appInfo(path);
    QFile file(path);
    if (file.open(QIODevice::ReadOnly)) {
        qDebug() << file.size()<<endl;
        qDebug() << file.fileName()<<endl;


        strcpy(head.name,(appInfo.baseName()+"."+appInfo.suffix()).toUtf8());
        // strcpy(head.name,(appInfo.baseName()+"."+appInfo.suffix()).toLatin1());
        head.size=file.size();
        qDebug() << head.name<<endl;
        qDebug() << head.size<<endl;
        file.close();
    } else {
        qDebug() << "文件打开失败！";
         sendresult(row,0,log);
         return;
    }
    //assert(sizeof(head)==sizeof(Head));
  //  tcpSocket->write((char*)&head,sizeof(head));
    head.sender=uid;
    qDebug()<<path.toUtf8();
    qDebug()<<(appInfo.baseName()+"."+appInfo.suffix()).toLatin1();

    strcpy(head.key,rsatool.pubKey.toUtf8());
    tcpSocket->write((char*)&head,sizeof(Head));
    qDebug()<<"client send pub key : "<<head.key<<endl;


}
bool CopyFileThread::initConnect(){
    tcpSocket=new QTcpSocket;
    tcpSocket->abort();
    tcpSocket->connectToHost(ip,port);

    if(tcpSocket->waitForConnected(3000)){
        putPubkey();

        if( getAesKey()){
            qDebug()<<"key change ok"<<endl;
        }else {

            return false;
        }

    }else{
        return false;
    }



}
void CopyFileThread::run()
{

    char in[16]={0}, out[16]={0}, buf[16]={0};
   if(initConnect()==false){
        sendresult(row,0,log);
       return;
    }//初始化
   long long k=0;
   char c;
   string pt=path.toStdString();
    // setlocale(LC_ALL,"Chinese-simplified");
    ifstream inFile(path.toLocal8Bit(),ios::binary|ios::in);  //以二进制读模式打开文件
    if(!inFile)
    {
       qDebug()<<"文件打开失败";
    }
    long long sum=0;
    while (inFile.get(c))  //每次读取一个字符
    {
        in[k]=c;
        k++;
        if(k==15){//没凑齐15个字节，第16个字节存入字节数，然后加密，发送
            in[15]=k;
            aestool.aes256_encrypt(in, out); //加密

            tcpSocket->write(out,16);
            tcpSocket->flush();
           // tcpSocket->waitForReadyRead();
            sum+=k;
            k=0;
            if(tcpSocket->state()!=3){
                sendresult(row,0,log);
                return;
            }
        }
    }
    if(k>0){
        in[15]=k;
        aestool.aes256_encrypt(in, out); //加密
        tcpSocket->write(out,16);
        tcpSocket->flush();
         sum+=k;
         k=0;
    }
    inFile.close();
    tcpSocket->close();
    qDebug()<<sum<<endl;
    sendresult(row,1,log);

}
