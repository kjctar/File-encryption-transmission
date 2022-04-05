#ifndef DBDEAL_H
#define DBDEAL_H

#endif // DBDEAL_H
#include <QtSql/qsql.h>
#include <QtSql/qsqldatabase.h>
#include <QtSql/qsqlquery.h>
#include<QDebug>
#include"user.h";
class DB{
public:
    QSqlDatabase conn;
    DB(){
            conn = QSqlDatabase::addDatabase("QODBC");

            conn.setHostName("localhost");    //数据库主机名
            conn.setDatabaseName("filetransys");    //数据库名
            conn.setUserName("root");        //数据库用户名
            conn.setPassword("123456");        //数据库密码
            conn.setPort(3306);
            bool bisOpenn = conn.open();          //打开数据库连接
            qDebug()<<bisOpenn;

    }

    ~DB(){

    }
    bool login(QString name,QString password,User &user){
       QSqlQuery* query = new QSqlQuery(conn);
       QString strCmd;

       strCmd = QString("select * from user where name='%1' and password='%2'").arg(name).arg(password);
       qDebug()<<strCmd;

       query->prepare(strCmd);
       if (query->exec())
       {

           while (query->next()){
               QString info = QString(" %1 %2 %3 %4 %5")
                   .arg(query->value(1).toString())
                   .arg(query->value(2).toString())
                   .arg(query->value(3).toUInt())
                   .arg(query->value(4).toString())
                   .arg(query->value(5).toString());
               user.age=query->value(3).toUInt();
               user.name=query->value(1).toString();
               user.password=query->value(2).toString();
               user.email=query->value(4).toString();
               user.number=query->value(5).toString();
           }
           return true;


       }else{
           return false;

       }
    }

};
