/* 
 * Copyright (C) 2017 iData Inc. - All Rights Reserved
 * Developed by William Zhang
 */

#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/buffer.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>

void ID_Base64Encode(const unsigned char* buffer, size_t length, char** b64text, int * b64length) {
    BIO *bio, *b64;
    BUF_MEM *bufferPtr;

    b64 = BIO_new(BIO_f_base64());
    bio = BIO_new(BIO_s_mem());
    bio = BIO_push(b64, bio);

    BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL); //Ignore newlines - write everything in one line
    BIO_write(bio, buffer, (int)length);
    BIO_flush(bio);
    BIO_get_mem_ptr(bio, &bufferPtr);

    *b64length = (int)(*bufferPtr).length;
    *b64text = malloc(sizeof(char) * (*b64length));
    memset(*b64text, 0x00, sizeof(char) * (*b64length));
    memcpy(*b64text, (*bufferPtr).data, sizeof(char) * (*bufferPtr).length);
    
    BIO_free_all(bio);
}

size_t ID_calcDecodeLength(const char* b64input) { //Calculates the length of a decoded string
    size_t len = strlen(b64input), padding = 0;

    if (b64input[len-1] == '=' && b64input[len-2] == '=') //last two chars are =
            padding = 2;
    else if (b64input[len-1] == '=') //last char is =
            padding = 1;

    return (len*3)/4 - padding;
}

void ID_Base64Decode(char* b64message, unsigned char** buffer, size_t* length) {
    BIO *bio, *b64;

    int decodeLen = (int)ID_calcDecodeLength(b64message);
    *buffer = (unsigned char*)malloc(decodeLen + 1);
    memset(*buffer, 0x00, decodeLen + 1);
    (*buffer)[decodeLen] = '\0';

    bio = BIO_new_mem_buf(b64message, -1);
    b64 = BIO_new(BIO_f_base64());
    bio = BIO_push(b64, bio);

    BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL); //Do not use newlines to flush buffer
    *length = BIO_read(bio, *buffer, (int)strlen(b64message));
    BIO_free_all(bio);
    //assert(*length == (size_t)decodeLen); //length should equal decodeLen, else something went horribly wrong
}