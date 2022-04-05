#ifndef AES_H
#define AES_H

#endif // AES_H
#include<openssl/aes.h>
#include<QDebug>
#include<QRandomGenerator>
class Aestool{
public:

    char key[32];
    int aes256_encrypt(char* inr, char* out)   //加密
    {
       char str_in[16];
       for(int i=0;i<16;i++){
           str_in[i]=inr[i];
       }
       int i;
       int len;
       AES_KEY aes;
       unsigned char iv[AES_BLOCK_SIZE] = {0};

       if (!str_in || !out)
            return 0;
        len = strlen(str_in);
        for (i = 0; i < 16; ++i)
             iv[i] = i+32;

       if (AES_set_encrypt_key((unsigned char*)key, 256, &aes) < 0){
           qDebug()<<"加密失败"<<endl;
            return 0;
        }

        AES_cbc_encrypt((unsigned char*)str_in, (unsigned char*)out, 16, &aes, iv, AES_ENCRYPT);
       return 1;
    }
    int aes256_decrypt(char* inr, char* out)   //解密
    {
        char str_in[16];
        for(int i=0;i<16;i++){
            str_in[i]=inr[i];
        }
        int i;
        int len=16;
        AES_KEY aes;
        unsigned char iv[AES_BLOCK_SIZE] = {0};

         if (!str_in || !out)
             return -1;

        //en = strlen(str_in);
         //确保里面的内容加密解密一样
         for (i = 0; i < 16; ++i)
            iv[i] = i+32;

         if (AES_set_decrypt_key((unsigned char*)key, 256, &aes) < 0)
         {
            return -1;
         }

        AES_cbc_encrypt((unsigned char*)str_in, (unsigned char*)out, 16, &aes, iv, AES_DECRYPT);
        return 0;
    }
    void keyInit(){
        for(int i=0;i<32;i++){
            if(QRandomGenerator::global()->bounded(26)&1){
                key[i]='a'+QRandomGenerator::global()->bounded(26);
            }else{
                key[i]='0'+QRandomGenerator::global()->bounded(9);
            }

        }
        key[31]='\0';

       // strcpy(key,"1wradfr4e3fefefad4545454h6thrsf");
    }//秘钥生成
    void setKey(QString newkey){//设置秘钥
        strcpy(key,newkey.toUtf8());
    }


};
