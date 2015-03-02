/**
* File "keys.h"
* Created by Sina on Sun Mar  1 19:01:44 2015.
*/

#pragma once

#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/err.h>
#include "Header.h"

#define MAX_MSG_SIZE 16384 

class KeyPair{
public:
    string pub;
    string priv;
    KeyPair(string Pub, string Priv): pub(Pub), priv(Priv) {}
};

KeyPair getKeyPair(int);
RSA * createRSA(const char *,bool);
int public_encrypt(unsigned char * data,int data_len,unsigned char * key, unsigned char *encrypted);

int private_decrypt(unsigned char * enc_data,int data_len,unsigned char * key, unsigned char *decrypted);
 
int private_encrypt(unsigned char * data,int data_len,const unsigned char * key, unsigned char *encrypted);

int private_encrypt(string data,const char * key, unsigned char *encrypted);

int public_decrypt(unsigned char * enc_data,int data_len,unsigned char * key, unsigned char *decrypted);
string public_decrypt(unsigned char * enc_data,int data_len,const char * key);
