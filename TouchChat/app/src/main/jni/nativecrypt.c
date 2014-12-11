#include <stdio.h>
#include <stdlib.h>
#include "openssl/rsa.h"
#include "openssl/x509.h"
#include "openssl/bio.h"
#include "openssl/sha.h"
#include "openssl/evp.h"
#include <string.h>
#include <math.h>
#include <stdint.h>
#include <assert.h>

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
    JNIEnv* env, jobject this, jstring message, jstring yourPrivateKey) {

    const char *privkey = (*env)->GetStringUTFChars(env, yourPrivateKey, 0);

    BIO *bio = BIO_new_mem_buf((char*)privkey, -1);
    RSA *r = RSA_new();
    PEM_read_bio_RSAPrivateKey(bio, &r, 0, "password");
    //RSA *PEM_read_bio_RSAPrivateKey(BIO *bp, &r, pem_password_cb *cb, void *u);

    const char *msg = (*env)->GetStringUTFChars(env, message, 0);

    unsigned char *digest = malloc(20);
    SHA1((const unsigned char *)msg, strlen(msg) + 1, digest);

    unsigned char *sig = malloc(RSA_size(r));
    unsigned int siglen;
    RSA_sign(NID_sha1, digest, 20, sig, &siglen, r);

    // Encode with base64 so that we don't deal with null bytes.
    char *encoded;
    Base64Encode(sig, siglen, &encoded);

    (*env)->ReleaseStringUTFChars(env, yourPrivateKey, privkey);
    (*env)->ReleaseStringUTFChars(env, message, msg);

    return (*env)->NewStringUTF(env, encoded);
}

jstring Java_net_bytesec_touchchat_Native_verifyMessage(
    JNIEnv* env, jobject this, jstring signedmessage, jstring otherPublicKey) {
}

jstring Java_net_bytesec_touchchat_Native_verifyMassMessages(
    JNIEnv* env, jobject this, jobjectArray messages, jobjectArray otherPublicKeys) {
}

/*
 * From http://doctrina.org/Base64-With-OpenSSL-C-API.html
 * !!! THE FOLLOWING THREE FUNCTIONS ARE NOT WRITTEN BY ME.
 */
size_t calcDecodeLength(const char* b64input) { //Calculates the length of a decoded string
	size_t len = strlen(b64input),
		padding = 0;

	if (b64input[len-1] == '=' && b64input[len-2] == '=') //last two chars are =
		padding = 2;
	else if (b64input[len-1] == '=') //last char is =
		padding = 1;

	return (size_t)len*0.75 - padding;
}

int Base64Decode(char* b64message, uint8_t** buffer, size_t* length) { //Decodes a base64 encoded string
	BIO *bio, *b64;

	int decodeLen = calcDecodeLength(b64message);
	*buffer = (uint8_t*)malloc(decodeLen);

	bio = BIO_new_mem_buf(b64message, -1);
	b64 = BIO_new(BIO_f_base64());
	bio = BIO_push(b64, bio);

	BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL); //Do not use newlines to flush buffer
	*length = BIO_read(bio, *buffer, strlen(b64message));
	assert(*length == decodeLen); //length should equal decodeLen, else something went horribly wrong
	BIO_free_all(bio);

	return (0); //success
}

int Base64Encode(const uint8_t* buffer, size_t length, char** b64text) { //Encodes a binary safe base 64 string
	BIO *bio, *b64;
	BUF_MEM *bufferPtr;

	b64 = BIO_new(BIO_f_base64());
	bio = BIO_new(BIO_s_mem());
	bio = BIO_push(b64, bio);

	BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL); //Ignore newlines - write everything in one line
	BIO_write(bio, buffer, length);
	BIO_flush(bio);
	BIO_get_mem_ptr(bio, &bufferPtr);
	BIO_set_close(bio, BIO_NOCLOSE);
	BIO_free_all(bio);

	*b64text=(*bufferPtr).data;

	return (0); //success
}