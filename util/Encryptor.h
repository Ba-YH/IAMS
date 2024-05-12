//
// Created by 巴云浩 on 2024/4/30.
//

#ifndef ITEMAUCTIONMANAGER_AESENCRYPTOR_H
#define ITEMAUCTIONMANAGER_AESENCRYPTOR_H

#include <bits/stdc++.h>
#include <openssl/evp.h>
#include <openssl/aes.h>
#include <openssl/bio.h>
#include <openssl/buffer.h>

std::string key = "0123456789abcdef0123456789abcdef";

std::string aes_encrypt(const std::string& plaintext);              //AES加密明文后使用Base64编码后得到密文
std::string aes_decrypt(const std::string& ciphertext);             //Base64解码密文后使用AES解密得到明文

std::string aes_encrypt(const std::string& plaintext) {
    EVP_CIPHER_CTX *ctx;
    int len;
    int ciphertext_len;
    unsigned char ciphertext[plaintext.size() + AES_BLOCK_SIZE]; // 加密后的数据缓冲区
    if(!(ctx = EVP_CIPHER_CTX_new())) {
        return "";
    }
    if(1 != EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, (const unsigned char*)key.c_str(), (const unsigned char*)"0000000000000000")) {
        EVP_CIPHER_CTX_free(ctx);
        return "";
    }
    if(1 != EVP_EncryptUpdate(ctx, ciphertext, &len, (const unsigned char*)plaintext.c_str(), plaintext.size())) {
        EVP_CIPHER_CTX_free(ctx);
        return "";
    }
    ciphertext_len = len;

    // 结束加密操作
    if(1 != EVP_EncryptFinal_ex(ctx, ciphertext + len, &len)) {
        EVP_CIPHER_CTX_free(ctx);
        return "";
    }
    ciphertext_len += len;

    EVP_CIPHER_CTX_free(ctx);

    // 使用 Base64 编码加密后的数据
    BIO *bio, *b64;
    BUF_MEM *bptr;

    b64 = BIO_new(BIO_f_base64());
    bio = BIO_new(BIO_s_mem());
    bio = BIO_push(b64, bio);

    BIO_write(bio, ciphertext, ciphertext_len);
    BIO_flush(bio);
    BIO_get_mem_ptr(bio, &bptr);

    std::string encoded_data(bptr->data, bptr->length);

    BIO_free_all(bio);

    return encoded_data;
}

// AES 解密函数
std::string aes_decrypt(const std::string& ciphertext) {
    EVP_CIPHER_CTX *ctx;
    int len;
    int plaintext_len;
    unsigned char plaintext[ciphertext.size()];

    // 使用 Base64 解码密文
    BIO *bio, *b64;

    b64 = BIO_new(BIO_f_base64());
    bio = BIO_new_mem_buf(ciphertext.c_str(), ciphertext.size());
    bio = BIO_push(b64, bio);

    len = BIO_read(bio, plaintext, ciphertext.size());
    plaintext_len = len;

    BIO_free_all(bio);

    // 创建并初始化解密上下文
    if(!(ctx = EVP_CIPHER_CTX_new())) {
        return "";
    }

    // 初始化解密操作，使用 AES-256-CBC 模式
    if(1 != EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, (const unsigned char*)key.c_str(), (const unsigned char*)"0000000000000000")) {
        EVP_CIPHER_CTX_free(ctx);
        return "";
    }

    // 执行解密操作
    if(1 != EVP_DecryptUpdate(ctx, plaintext, &len, plaintext, plaintext_len)) {
        EVP_CIPHER_CTX_free(ctx);
        return "";
    }
    plaintext_len = len;

    // 结束解密操作
    if(1 != EVP_DecryptFinal_ex(ctx, plaintext + len, &len)) {
        EVP_CIPHER_CTX_free(ctx);
        return "";
    }
    plaintext_len += len;

    EVP_CIPHER_CTX_free(ctx);

    // 返回解密后的数据
    return std::string(reinterpret_cast<char*>(plaintext), plaintext_len);
}


#endif //ITEMAUCTIONMANAGER_AESENCRYPTOR_H