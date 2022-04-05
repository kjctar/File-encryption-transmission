#include "rsa.h"
#include<QDebug>
Rsatool::Rsatool()
{

}

Rsatool::~Rsatool()
{

}

QString Rsatool::rsaPubEncrypt(const QString &strPlainData, const QString &strPubKey)
{
    QByteArray pubKeyArry = strPubKey.toUtf8();
    uchar* pPubKey = (uchar*)pubKeyArry.data();
    BIO* pKeyBio = BIO_new_mem_buf(pPubKey, pubKeyArry.length());
    if (pKeyBio == NULL) {
        return "";
    }


    RSA* pRsa = RSA_new();
    if (strPubKey.contains(BEGIN_RSA_PUBLIC_KEY)) {
        pRsa = PEM_read_bio_RSAPublicKey(pKeyBio, &pRsa, NULL, NULL);

    }
    else {
        pRsa = PEM_read_bio_RSA_PUBKEY(pKeyBio, &pRsa, NULL, NULL);

    }


    if (pRsa == NULL) {
        BIO_free_all(pKeyBio);
        return "";
    }

    int nLen = RSA_size(pRsa);
    char* pEncryptBuf = new char[nLen];


    //加密
    QByteArray plainDataArry = strPlainData.toUtf8();
    int nPlainDataLen = plainDataArry.length();

    int exppadding=nLen;
    if(nPlainDataLen>exppadding-11)
        exppadding=exppadding-11;
    int slice=nPlainDataLen/exppadding;//片数
    if(nPlainDataLen%(exppadding))
        slice++;

    QString strEncryptData = "";
    QByteArray arry;
    for(int i=0; i<slice; i++)
    {
        QByteArray baData = plainDataArry.mid(i*exppadding, exppadding);
        nPlainDataLen = baData.length();
        memset(pEncryptBuf, 0, nLen);
        uchar* pPlainData = (uchar*)baData.data();
        int nSize = RSA_public_encrypt(nPlainDataLen,
                                       pPlainData,
                                       (uchar*)pEncryptBuf,
                                       pRsa,
                                       RSA_PKCS1_PADDING);
        if (nSize >= 0)
        {
            arry.append(QByteArray(pEncryptBuf, nSize));
        }
    }

    strEncryptData += arry.toBase64();
    //释放内存
    delete pEncryptBuf;
    BIO_free_all(pKeyBio);
    RSA_free(pRsa);

    return strEncryptData;
}

QString Rsatool::rsaPriDecrypt(const QString &strDecryptData, const QString &strPriKey)
{
    QByteArray priKeyArry = strPriKey.toUtf8();
    uchar* pPriKey = (uchar*)priKeyArry.data();
    BIO* pKeyBio = BIO_new_mem_buf(pPriKey, priKeyArry.length());
    if (pKeyBio == NULL) {
        return "";
    }

    RSA* pRsa = RSA_new();
    pRsa = PEM_read_bio_RSAPrivateKey(pKeyBio, &pRsa, NULL, NULL);
    if (pRsa == NULL) {
        BIO_free_all(pKeyBio);
        return "";
    }

    int nLen = RSA_size(pRsa);
    char* pPlainBuf = new char[nLen];

    //解密
    QByteArray decryptDataArry = strDecryptData.toUtf8();
    decryptDataArry = QByteArray::fromBase64(decryptDataArry);
    int nDecryptDataLen = decryptDataArry.length();

    int rsasize=nLen;
    int slice=nDecryptDataLen/rsasize;//片数
    if(nDecryptDataLen%(rsasize))
        slice++;

    QString strPlainData = "";
    for(int i=0; i<slice; i++)
    {
        QByteArray baData = decryptDataArry.mid(i*rsasize, rsasize);
        nDecryptDataLen = baData.length();
        memset(pPlainBuf, 0, nLen);
        uchar* pDecryptData = (uchar*)baData.data();
        int nSize = RSA_private_decrypt(nDecryptDataLen,
                                        pDecryptData,
                                        (uchar*)pPlainBuf,
                                        pRsa,
                                        RSA_PKCS1_PADDING);
        if (nSize >= 0) {
            strPlainData += QByteArray(pPlainBuf, nSize);
            qDebug()<<strPlainData<<endl;
        }
    }

    //释放内存
    delete pPlainBuf;
    BIO_free_all(pKeyBio);
    RSA_free(pRsa);

    return strPlainData;
}

void Rsatool::rsaKeyInit(){

   size_t pri_len;
   size_t pub_len;
   char *pri_key = NULL;
   char *pub_key = NULL;

   // 生成密钥对
   RSA *keypair = RSA_generate_key(KEY_LENGTH, RSA_F4, NULL, NULL);

   BIO *pri = BIO_new(BIO_s_mem());
   BIO *pub = BIO_new(BIO_s_mem());

   PEM_write_bio_RSAPrivateKey(pri, keypair, NULL, NULL, 0, NULL, NULL);
   PEM_write_bio_RSAPublicKey(pub, keypair);

   // 获取长度
   pri_len = BIO_pending(pri);
   pub_len = BIO_pending(pub);

   // 密钥对读取到字符串
   pri_key = (char *)malloc(pri_len + 1);
   pub_key = (char *)malloc(pub_len + 1);

   BIO_read(pri, pri_key, pri_len);
   BIO_read(pub, pub_key, pub_len);

   pri_key[pri_len] = '\0';
   pub_key[pub_len] = '\0';

   // 存储密钥对
   this->pubKey = pub_key;
   this->priKey = pri_key;


}
