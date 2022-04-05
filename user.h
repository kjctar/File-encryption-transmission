#pragma once
#ifndef USER_H
#define USER_H

#endif // USER_H
#include <QString>
#include<QTcpSocket>

const QString ROOT="root";
const QString PASSWORD="123456";
struct Head{
    long long size;
    int sender;
    int recver;
    char name[124];
    char key[1024];
};
struct tranStatus{
    QString name;
    long long size;
    long long step;
    int row;
};
struct  User{
    int uid;
    QString name;
    QString password;
    QString email;
    QString number;
    int age;
};

