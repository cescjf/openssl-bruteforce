#ifndef ENCRYPTOR_H
#define ENCRYPTOR_H

#define _XOPEN_SOURCE 700

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <openssl/evp.h>
#include <openssl/err.h>

/** iv vector length **/
#define IV_LENGTH 8
/** key for cipher length **/
#define KEY_LENGTH 16
/** size of encryption block **/
#define BLOCK_SIZE 8

/** encryption and decyprion actions **/
enum ENCRYPTOR_ACTION { DECRYPT = 0 , ENCRYPT = 1 };
/** available cipher types **/
enum CIPHER_TYPE { BLOWFISH, CAST5 };

/** Encryptor definition **/
typedef struct {
	/** OpenSSL cipher context */
	EVP_CIPHER_CTX ctx;
	/** OpenSSL cipher **/
	const EVP_CIPHER *cipher;

	/** action to execute */
	int action;
	/** type of cipher to use */
	int type;
	/** iv init vector*/
	unsigned char iv[ IV_LENGTH ];
	/** encryptor key */
	unsigned char key[ KEY_LENGTH ];
	/** encryptor input */
	unsigned char *input;
	/** encryptor output */
	unsigned char *output;
	/** size of the input */
	int input_size;
	/** number of bytes writed to output */
	int output_length;
	/** number of bytes writed as padding */
	int padding_length;
} Encryptor;

/**
 * initialize a Encryptor
 * @param [data] 	a valid encryptor pointer
 * @param [action] 	ENCRYPT || DECRYPT
 * @param [type]	BLOWFISH || CAST5
 * @param [iv]		an initialization vector
 * @return error
 */
int encryptor_init_data( Encryptor *data, int action, int type, unsigned char iv[ IV_LENGTH ] );

/**
 * set Encryptor input
 * @param [data]    a valid encryptor pointer
 * @param [input]	a input text
 * @param [size]    input length in bytes
 * @return error
 */
int encryptor_set_input( Encryptor *data, unsigned char *input, int size );

/**
 * set Encryptor key
 * @param [data]    a valid encryptor pointer
 * @param [key]		a valid key
 * @return error
 */
int encryptor_set_key( Encryptor *data, unsigned char key[ KEY_LENGTH ] );

/**
 * set Encryptor iv vector
 * @param [data]    a valid encryptor pointer
 * @param [iv]	    a valid iv vector
 * @return error
 */
int encryptor_set_iv( Encryptor *data, unsigned char iv[ IV_LENGTH ] );

/**
 * dealocate and clean un Encryptor
 * @param [data] 	a valid encryptor pointer
 * @return error
 */
int encryptor_clean_up( Encryptor *data );

/**
 * init Encryptor execution
 * @param [data]    a valid encryptor pointer
 * @return error
 */
int encryptor_init( Encryptor *data );

/**
 * execute the encryptor cipher and action over input
 * @param [data]    a valid encryptor pointer
 * @return error
 */
int encryptor_update( Encryptor *data );

/**
 * finalize the encryptor execution over input
 * @param [data]    a valid encryptor pointer
 * @return error
 */
int encryptor_final( Encryptor *data );

/**
 * execute set_key, init, update and final in one step
 * @param [data]    a valid encryptor pointer
 */
void encryptor_execute( Encryptor *data, unsigned char key[ KEY_LENGTH ] );

#endif
