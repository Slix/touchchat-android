#ifndef NATIVECRYPT_H
#define NATIVECRYPT_H

#include <jni.h>

void Java_net_bytesec_touchchat_Native_helloworld(JNIEnv* env);

jstring Java_net_bytesec_touchchat_Native_generateKeypair(JNIEnv* env);

jstring Java_net_bytesec_touchchat_Native_signMessage(
    JNIEnv* env, jstring message, jstring yourPrivateKey);

jstring Java_net_bytesec_touchchat_Native_verifyMessage(
    JNIEnv* env, jstring signedmessage, jstring otherPublicKey);

jstring Java_net_bytesec_touchchat_Native_verifyMassMessages(
    JNIEnv* env, jobjectArray messages, jobjectArray otherPublicKeys);


#endif