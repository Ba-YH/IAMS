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

std::string aes_encrypt(const std::string &plain_text);      //AES加密
std::string aes_decrypt(const std::string &cipher_text);     //AES解密
std::string base64_encode(const std::string &input);         //base64编码
std::string base64_decode(const std::string &input);         //base64解码
std::string my_encrypt(const std::string &plain_text);       //自定义加密
std::string my_decrypt(const std::string &cipher_text);      //自定义解密

// AES 加密函数
std::string aes_encrypt(const std::string &plain_text) {
    EVP_CIPHER_CTX *ctx;
    unsigned char iv[AES_BLOCK_SIZE] = {0};
    unsigned char cipher_text[1024] = {0};
    int len;
    std::string encrypted_text;
    ctx = EVP_CIPHER_CTX_new();
    EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, (const unsigned char *) key.c_str(), iv);
    EVP_EncryptUpdate(ctx, cipher_text, &len, (const unsigned char *) plain_text.c_str(), plain_text.length());
    encrypted_text.append((char *) cipher_text, len);
    EVP_EncryptFinal_ex(ctx, cipher_text + len, &len);
    encrypted_text.append((char *) cipher_text + len);
    EVP_CIPHER_CTX_free(ctx);
    return encrypted_text;
}

// AES 解密函数
std::string aes_decrypt(const std::string &cipher_text) {
    EVP_CIPHER_CTX *ctx;
    unsigned char iv[AES_BLOCK_SIZE] = {0};
    unsigned char plain_text[1024] = {0};
    int len;
    std::string decrypted_text;

    ctx = EVP_CIPHER_CTX_new();
    EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, (const unsigned char *) key.c_str(), iv);

    EVP_DecryptUpdate(ctx, plain_text, &len, (const unsigned char *) cipher_text.c_str(), cipher_text.length());
    decrypted_text.append((char *) plain_text, len);

    EVP_DecryptFinal_ex(ctx, plain_text + len, &len);
    decrypted_text.append((char *) plain_text + len);

    EVP_CIPHER_CTX_free(ctx);

    return decrypted_text;
}

// Base64 编码函数
std::string base64_encode(const std::string &input) {
    BIO *bio, *b64;
    BUF_MEM *bufferPtr;
    b64 = BIO_new(BIO_f_base64());
    bio = BIO_new(BIO_s_mem());
    bio = BIO_push(b64, bio);
    BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL);
    BIO_write(bio, input.c_str(), input.length());
    BIO_flush(bio);
    BIO_get_mem_ptr(bio, &bufferPtr);
    std::string encoded_text(bufferPtr->data, bufferPtr->length);
    BIO_free_all(bio);
    return encoded_text;
}

// Base64 解码函数
std::string base64_decode(const std::string &input) {
    BIO *bio, *b64;
    char buffer[1024] = {0};
    b64 = BIO_new(BIO_f_base64());
    bio = BIO_new_mem_buf(input.c_str(), input.length());
    bio = BIO_push(b64, bio);
    BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL);
    int decoded_length = BIO_read(bio, buffer, input.length());
    BIO_free_all(bio);
    return std::string(buffer, decoded_length);
}

std::string my_encrypt(const std::string &plain_text) {
    std::string encrypted_text = base64_encode(aes_encrypt(plain_text));
    return encrypted_text;
}

std::string my_decrypt(const std::string &cipher_text) {
    std::string decrypted_text = aes_decrypt(base64_decode(cipher_text));
}

#endif //ITEMAUCTIONMANAGER_AESENCRYPTOR_H
