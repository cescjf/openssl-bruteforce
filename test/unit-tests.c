#define _XOPEN_SOURCE 700

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>
#include "../include/encryptor.h"
#include "../include/fs.h"
#include "../include/keygen.h"

int init_suite(void)
{
   return 0;
}

int clean_suite(void)
{
    return 0;
}

/**
 * encryptor tests declaration
 */
void test_encryptor_init(void);
void test_encryptor_blowfish(void);
void test_encryptor_cast5(void);
void test_encryptor_phrase(void);
void test_encryptor_validation(void);

/**
 * fs tests declaration
 */
void test_fs_validation(void);
void test_fs_write_read(void);

/**
 * keygen test declaration
 */
void test_keygen_getenv(void);
void test_keygen_keys(void);
void test_keygen_validation(void);
void test_keygen_keys_adhoc(void);

int main()
{
	int error = 0;

	CU_pSuite encryptor_suite = NULL;
	CU_pSuite fs_suite = NULL;
	CU_pSuite keygen_suite = NULL;


   	/* initialize the CUnit test registry */
   	if ( CUE_SUCCESS != CU_initialize_registry() )
    	return CU_get_error();

   	/* add suites to the registry */
   	encryptor_suite = CU_add_suite( "Suite test for encryptor", init_suite, clean_suite );
	fs_suite = CU_add_suite( "Suite test for fs", init_suite, clean_suite );
	keygen_suite = CU_add_suite( "Suite test for keygen", init_suite, clean_suite );

   	if ( NULL == encryptor_suite || NULL == fs_suite || NULL == keygen_suite )
	{
      	CU_cleanup_registry();
      	return CU_get_error();
   	}

   	/* add the tests to the encryptor suite */
	error += NULL == CU_add_test( encryptor_suite, "test of encryptor initialization", test_encryptor_init );
	error += NULL == CU_add_test( encryptor_suite, "test of blowfish encryption", test_encryptor_blowfish );
	error += NULL == CU_add_test( encryptor_suite, "test of cast5 encryption", test_encryptor_cast5 );
	error += NULL == CU_add_test( encryptor_suite, "test of phrase encryption", test_encryptor_phrase );
	error += NULL == CU_add_test( encryptor_suite, "test of encryption validation", test_encryptor_validation );

	/* add fs suite tests */
	error += NULL == CU_add_test( fs_suite, "test of fs validation", test_fs_validation );
	error += NULL == CU_add_test( fs_suite, "test of fs write and read", test_fs_write_read );

	/* add keygen tests */
	error += NULL == CU_add_test( keygen_suite, "test of keygen getenv", test_keygen_getenv );
	error += NULL == CU_add_test( keygen_suite, "test of keygen key generation", test_keygen_keys );
	error += NULL == CU_add_test( keygen_suite, "test of keygen key generation adhoc", test_keygen_keys_adhoc );
	error += NULL == CU_add_test( keygen_suite, "test of keygen validation", test_keygen_validation );

	if( error != 0 )
   	{
      	CU_cleanup_registry();
      	return CU_get_error();
   	}

   	/* Run all tests using the CUnit Basic interface */
   	CU_basic_set_mode(CU_BRM_VERBOSE);
   	CU_basic_run_tests();
   	CU_cleanup_registry();
   	return CU_get_error();
}

/**
 * encryptor tests definitions
 */
void test_encryptor_init(void)
{
	Encryptor enc;
	unsigned char key[ KEY_LENGTH ] = "               1";
	unsigned char input[8] = "input";
	unsigned char iv[ IV_LENGTH ] = {1,2,3,4,5,6,7,8};

	CU_ASSERT( encryptor_init_data( &enc, ENCRYPT, BLOWFISH, iv ) == 0 );
    CU_ASSERT( enc.action == ENCRYPT );
    CU_ASSERT( enc.type == BLOWFISH );
    CU_ASSERT( memcmp( enc.iv, iv, IV_LENGTH ) == 0 );

	CU_ASSERT( encryptor_init_data( &enc, DECRYPT, BLOWFISH, iv ) == 0 );
    CU_ASSERT( enc.action == DECRYPT );
    CU_ASSERT( enc.type == BLOWFISH );
    CU_ASSERT( memcmp( enc.iv, iv, IV_LENGTH ) == 0 );

	CU_ASSERT( encryptor_init_data( &enc, ENCRYPT, CAST5, iv ) == 0 );
    CU_ASSERT( enc.action == ENCRYPT );
    CU_ASSERT( enc.type == CAST5 );
    CU_ASSERT( memcmp( enc.iv, iv, IV_LENGTH ) == 0 );

	CU_ASSERT( encryptor_init_data( &enc, DECRYPT, CAST5, iv ) == 0 );
    CU_ASSERT( enc.action == DECRYPT );
    CU_ASSERT( enc.type == CAST5 );
    CU_ASSERT( memcmp( enc.iv, iv, IV_LENGTH ) == 0 );

	CU_ASSERT( encryptor_set_iv( &enc, iv ) == 0 );
	CU_ASSERT( memcmp( enc.iv, iv, IV_LENGTH ) == 0 )

	CU_ASSERT( encryptor_set_key( &enc,key ) == 0);
    CU_ASSERT( memcmp( enc.key, key, KEY_LENGTH ) == 0 );

    CU_ASSERT( encryptor_set_input( &enc, input, sizeof(input) ) == 0);
    CU_ASSERT( enc.input_size == sizeof(input) );
    CU_ASSERT( memcmp( enc.input, input, sizeof(input) ) == 0 );
}

void test_encryptor_blowfish(void)
{
	unsigned char input[7] = "----";
	unsigned char output[20];
	unsigned char key[ KEY_LENGTH ] = "               0";
	unsigned char iv[ IV_LENGTH ] = {1,2,3,4,5,6,7,8};

	Encryptor enc;
	encryptor_init_data( &enc, ENCRYPT, BLOWFISH, iv );
	encryptor_set_key( &enc, key );
	encryptor_set_input( &enc, input, sizeof(input) );

	CU_ASSERT( encryptor_init( &enc ) == 0 );
	CU_ASSERT( encryptor_update( &enc ) == 0 );
	CU_ASSERT( encryptor_final( &enc ) == 0 );

	int len = enc.output_length + enc.padding_length;
	memcpy( output, enc.output, len );

	CU_ASSERT( encryptor_clean_up( &enc ) == 0 );

	encryptor_init_data( &enc, DECRYPT, BLOWFISH, iv );
    encryptor_set_input( &enc, output, len );
	encryptor_set_key( &enc, key );

    CU_ASSERT( encryptor_init( &enc ) == 0 );
    CU_ASSERT( encryptor_update( &enc ) == 0 );
   	CU_ASSERT( encryptor_final( &enc ) == 0 );
	CU_ASSERT( memcmp( input, enc.output, sizeof(input) ) == 0 );
	CU_ASSERT( encryptor_clean_up( &enc ) == 0 );
}

void test_encryptor_cast5(void)
{
    unsigned char input[7] = "----";
    unsigned char output[20];
    unsigned char key[ KEY_LENGTH ] = "               0";
    unsigned char iv[ IV_LENGTH ] = {1,2,3,4,5,6,7,8};

	Encryptor enc;
    encryptor_init_data( &enc, ENCRYPT, CAST5, iv );
    encryptor_set_key( &enc, key );
    encryptor_set_input( &enc, input, sizeof(input) );

    CU_ASSERT( encryptor_init( &enc ) == 0 );
    CU_ASSERT( encryptor_update( &enc ) == 0 );
    CU_ASSERT( encryptor_final( &enc ) == 0 );

    int len = enc.output_length + enc.padding_length;
    memcpy( output, enc.output, len );

    CU_ASSERT( encryptor_clean_up( &enc ) == 0 );

    encryptor_init_data( &enc, DECRYPT, CAST5, iv );
    encryptor_set_input( &enc, output, len );
    encryptor_set_key( &enc,key );

    CU_ASSERT( encryptor_init( &enc ) == 0 );
    CU_ASSERT( encryptor_update( &enc ) == 0 );
    CU_ASSERT( encryptor_final( &enc ) == 0 );
    CU_ASSERT( memcmp( input, enc.output, sizeof(input) ) == 0 );
	CU_ASSERT( encryptor_clean_up( &enc ) == 0 );
}

void test_encryptor_phrase(void)
{
	unsigned char *text = (unsigned char *) "Frase: Stay hungry, Stay foolish";
	unsigned char key[ KEY_LENGTH ] = "               0";
    unsigned char iv[ IV_LENGTH ] = {1,2,3,4,5,6,7,8};
	int len = (int) strlen( (char *) text ) + 1 ;
	unsigned char *output;

	Encryptor enc;
	encryptor_init_data( &enc, ENCRYPT, BLOWFISH, iv );
	encryptor_set_key( &enc, key );
	encryptor_set_input( &enc, text, len );

	CU_ASSERT( encryptor_init( &enc ) == 0 );
    CU_ASSERT( encryptor_update( &enc ) == 0 );
    CU_ASSERT( encryptor_final( &enc ) == 0 );

	int total_length = enc.output_length + enc.padding_length;
	output = (unsigned char *) malloc( total_length );
	memcpy( output, enc.output, total_length );

	CU_ASSERT( encryptor_clean_up( &enc ) == 0 );

	Encryptor dec;
	encryptor_init_data( &dec, DECRYPT, BLOWFISH, iv );
    encryptor_set_key( &dec, key );
    encryptor_set_input( &dec, output , total_length );

	CU_ASSERT( encryptor_init( &dec ) == 0 );
    CU_ASSERT( encryptor_update( &dec ) == 0 );
    CU_ASSERT( encryptor_final( &dec ) == 0 );
	CU_ASSERT( memcmp( text, dec.output, len ) == 0 );
	CU_ASSERT( encryptor_clean_up( &dec ) == 0 );
}

void test_encryptor_validation(void)
{
	Encryptor enc;

	CU_ASSERT( encryptor_set_key(&enc,NULL) != 0 );
}

/**
 * fs tests definitions
 */
void test_fs_validation(void)
{
    CU_ASSERT( fs_write( "", NULL, 8 ) == -1 );
}

void test_fs_write_read(void)
{
	char* path = "./tmp/testfile";
	unsigned char input[8] = "1234567";
	unsigned char output[8];

    CU_ASSERT( fs_write( path, input, 8 ) == 8 );
    CU_ASSERT( fs_read( path, output, 8 ) == 8 );
    CU_ASSERT( memcmp( output, input, 8 ) == 0 );
}

/**
 * keygen tests definitions
 */
void test_keygen_getenv(void)
{
	long cant_keys;

	setenv( "CANT_KEYS", "10", 1 );

	CU_ASSERT( keygen_getenv( &cant_keys ) == 0 );
    CU_ASSERT( cant_keys == 10 );

	unsetenv("CANT_KEYS");

	CU_ASSERT( keygen_getenv( &cant_keys ) == 0 );
	CU_ASSERT( cant_keys == CANT_KEYS );

}

void test_keygen_keys(void)
{
	long cant_keys;
	int error = 0;
	unsigned char key[ KEY_LENGTH + 1 ];
	
	keygen_getenv( &cant_keys );
	key[ KEY_LENGTH ] = '\0';

	for( int i = 0; i < cant_keys; i++ ) {
		if( error == 0 ) {

			keygen_itokey( key,i );

			if( strtol( ( char* ) key, NULL, 0 ) != i ) {
				error = i;
			}
		}
	}

	if( error ) {
		printf("fail to generate key %d ",error);
		CU_FAIL();
	}
		
}

void test_keygen_keys_adhoc(void)
{
	long cant_keys;
	int error = 0;
	int keygen_characters[10] = {'0','1','2','3','4','5','6','7','8','9'};
	unsigned char key[ KEY_LENGTH + 1 ];
	
	keygen_getenv( &cant_keys );
	memset(key,ASCII_SPACE,KEY_LENGTH);
	key[ KEY_LENGTH ] = '\0';
	
	for( int i = 0; i < cant_keys; i++ ) {
		if( error == 0 ) {
			key[KEY_LENGTH-1] = keygen_characters[i % 10];
			key[KEY_LENGTH-2] = i/10? keygen_characters[(i/10) % 10] : ASCII_SPACE;
			key[KEY_LENGTH-3] = i/100? keygen_characters[(i/100) % 10] : ASCII_SPACE;
			key[KEY_LENGTH-4] = i/1000? keygen_characters[(i/1000) % 10] : ASCII_SPACE;
			key[KEY_LENGTH-5] = i/10000? keygen_characters[(i/10000) % 10] : ASCII_SPACE;
			key[KEY_LENGTH-6] = i/100000? keygen_characters[(i/100000) % 10] : ASCII_SPACE;
			key[KEY_LENGTH-7] = i/1000000? keygen_characters[(i/1000000) % 10] : ASCII_SPACE;
			key[KEY_LENGTH-8] = i/10000000? keygen_characters[(i/10000000) % 10] : ASCII_SPACE;
			
			if( strtol( ( char* ) key, NULL, 0 ) != i ) {
				error = i;
			}
		}
	}

	if( error )
		CU_FAIL();
}

void test_keygen_validation(void)
{
	unsigned char key[KEY_LENGTH];
	long cant_keys;

	keygen_getenv( &cant_keys );

	CU_ASSERT( keygen_itokey( key, -1 ) != 0 );
	CU_ASSERT( keygen_itokey( key, cant_keys + 1 ) != 0 );
}



