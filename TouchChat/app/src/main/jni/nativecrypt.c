#include <stdio.h>
#include <stdlib.h>
#include "openssl/rsa.h"
#include "openssl/x509.h"
#include "openssl/bio.h"

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

    // Convert to strings

    BIO *pubbio = BIO_new(BIO_s_mem());
    PEM_write_bio_RSAPublicKey(pubbio, r);
    char *buf;
    long len = BIO_get_mem_data(pubbio, &buf);
    char* pubkey = malloc(len + 1);
    memcpy(pubkey, buf, len);
    pubkey[len] = '\0';
    BIO_free(pubbio);

    BIO *privbio = BIO_new(BIO_s_mem());
    PEM_write_bio_RSAPrivateKey(privbio, r, EVP_des_ede3_cbc(), NULL, 0, 0, "password");
    long len2 = BIO_get_mem_data(privbio, &buf);
    char* privkey = malloc(len2 + 1);
    memcpy(privkey, buf, len2);
    privkey[len2] = '\0';
    BIO_free(privbio);

    long totallen = len + len2 + 100;
    char *result = malloc(totallen + 1);
    // Pipe separator
    snprintf(result, totallen, "%s|%s", pubkey, privkey);

    free(pubkey);
    free(privkey);

    return (*env)->NewStringUTF(env, result);
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