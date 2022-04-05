#ifndef RAS_H
#define RAS_H

#endif // RAS_H
#ifndef RSA_H
#define RSA_H

#include <QString>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/err.h>

#define BEGIN_RSA_PUBLIC_KEY    "BEGIN RSA PUBLIC KEY"
#define BEGIN_RSA_PRIVATE_KEY   "BEGIN RSA PRIVATE KEY"
#define BEGIN_PUBLIC_KEY        "BEGIN PUBLIC KEY"
#define BEGIN_PRIVATE_KEY       "BEGIN PRIVATE KEY"
#define KEY_LENGTH              1024

class Rsatool
{
    public:
        Rsatool();
        ~Rsatool();

        QString rsaPubEncrypt(const QString &strPlainData, const QString &strPubKey);//通过strPubKey加密strPlainData，并返回加密后的数据

        QString rsaPriDecrypt(const QString &strDecryptData, const QString &strPriKey);//通过strPriKey解密strPlainData，返回解密后的数据
        QString pubKey;//公钥
        QString priKey;//私钥
        void rsaKeyInit();//公钥私钥生成
};

#endif // RSA_H
