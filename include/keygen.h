#define _XOPEN_SOURCE 700

#ifndef KEYGEN_H
#define KEYGEN_H

#include <stdlib.h>
#include <string.h>

/** ascii code for space */
#define ASCII_SPACE 32
/** ascii code for character 0 */
#define ASCII_ZERO 48
/** default number of keys to use */
#define CANT_KEYS 100000000
/** length of the generated keys */
#define KEY_LENGTH 16

/**
 * Generate key from number
 * @param [key]		buffer to save the generated key
 * @param [number]	number to convert to key
 * @return error
 */
int keygen_itokey( unsigned char key[KEY_LENGTH], long number );


/**
 * Generate key from number
 * @param [key]		buffer to save the generated key
 * @param [number]	number to convert to key
 * @return error
 */
void keygen_itokey_v1( unsigned char key[KEY_LENGTH], long number );

/**
 * Read enviroment variables and get the number of keys
 * @param [cant_keys]	where to save the number of keys
 * @return error
 */
int keygen_getenv( long* cant_keys );

#endif
