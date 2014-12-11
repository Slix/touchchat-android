#ifndef NATIVECRYPT_H
#define NATIVECRYPT_H

#include <jni.h>

void Java_net_bytesec_touchchat_Native_helloworld(JNIEnv* env);

jstring Java_net_bytesec_touchchat_Native_generateKeypair(JNIEnv* env);

jstring Java_net_bytesec_touchchat_Native_signMessage(
    JNIEnv* env, jobject this, jstring message, jstring yourPrivateKey);

jboolean Java_net_bytesec_touchchat_Native_verifyMessage(
    JNIEnv* env, jobject this, jstring message, jstring signature, jstring otherPublicKey);

jboolean Java_net_bytesec_touchchat_Native_verifyMassMessages(
    JNIEnv* env, jobject this, jobjectArray messages, jobjectArray signatures, jobjectArray otherPublicKeys);

// Not written by me
size_t calcDecodeLength(const char* b64input);
int Base64Decode(char* b64message, uint8_t** buffer, size_t* length);
int Base64Encode(const uint8_t* buffer, size_t length, char** b64text);


#endif
