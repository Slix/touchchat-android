#include <stdio.h>
#include <stdlib.h>
#include "openssl/rsa.h"

#include "nativecrypt.h"

const int BITS = 2048;
const unsigned long EXPONENT = 65537;

void Java_net_bytesec_touchchat_Native_helloworld(JNIEnv* env) {
	printf("Hello, world!\n");
	//exit(12);
}

jstring Java_net_bytesec_touchchat_Native_generateKeypair(JNIEnv* env) {
    // Set up exponent
    BIGNUM *e = BN_new();
    BN_set_word(e, EXPONENT);

    RSA *r = RSA_new();
    RSA_generate_key_ex(r, BITS, e, NULL);
}

jstring Java_net_bytesec_touchchat_Native_signMessage(
    JNIEnv* env, jstring message, jstring yourPrivateKey) {
}

jstring Java_net_bytesec_touchchat_Native_verifyMessage(
    JNIEnv* env, jstring signedmessage, jstring otherPublicKey) {
}

jstring Java_net_bytesec_touchchat_Native_verifyMassMessages(
    JNIEnv* env, jobjectArray messages, jobjectArray otherPublicKeys) {
}