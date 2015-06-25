#define _XOPEN_SOURCE 700

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>
#include "../include/keygen.h"

int init_suite(void)
{
   return 0;
}

int clean_suite(void)
{
    return 0;
}

void test_keygen_itokey(void)
{
	unsigned char key[ KEY_LENGTH ];
	time_t start_time, end_time;

	start_time = time( NULL );	
	for( long i = 0; i < CANT_KEYS; i++ ) {
		keygen_itokey( key,i );
	}
	end_time = time( NULL );
	
	printf("Total time %f ",difftime( end_time, start_time ));
}

void test_keygen_itokey_v1(void)
{
	unsigned char key[ KEY_LENGTH ];
	time_t start_time, end_time;

	start_time = time( NULL );	
	for( long i = 0; i < CANT_KEYS; i++ ) {
		keygen_itokey_v1( key,i );
	}
	end_time = time( NULL );
	
	printf("Total time %f ",difftime( end_time, start_time ));
}

void test_keygen_adhoc(void)
{
	unsigned char key[ KEY_LENGTH ];
	int keygen_characters[10] = {'0','1','2','3','4','5','6','7','8','9'};
	time_t start_time, end_time;

	start_time = time( NULL );
	
	memset(key,ASCII_SPACE,KEY_LENGTH);
	
	for( long i = 0; i < CANT_KEYS; i++ ) {
		key[KEY_LENGTH-1] = keygen_characters[i % 10];
		key[KEY_LENGTH-2] = i/10? keygen_characters[(i/10) % 10] : ASCII_SPACE;
		key[KEY_LENGTH-3] = i/100? keygen_characters[(i/100) % 10] : ASCII_SPACE;
		key[KEY_LENGTH-4] = i/1000? keygen_characters[(i/1000) % 10] : ASCII_SPACE;
		key[KEY_LENGTH-5] = i/10000? keygen_characters[(i/10000) % 10] : ASCII_SPACE;
		key[KEY_LENGTH-6] = i/100000? keygen_characters[(i/100000) % 10] : ASCII_SPACE;
		key[KEY_LENGTH-7] = i/1000000? keygen_characters[(i/1000000) % 10] : ASCII_SPACE;
		key[KEY_LENGTH-8] = i/10000000? keygen_characters[(i/10000000) % 10] : ASCII_SPACE;
	}
	end_time = time( NULL );
	
	
	printf("Total time %f ",difftime( end_time, start_time ));
}


int main()
{
	int error = 0;

	CU_pSuite keygen_suite = NULL;


   	/* initialize the CUnit test registry */
   	if ( CUE_SUCCESS != CU_initialize_registry() )
    	return CU_get_error();

   	/* add suites to the registry */
	keygen_suite = CU_add_suite( "Suite test for keygen", init_suite, clean_suite );

   	if ( NULL == keygen_suite )
	{
      	CU_cleanup_registry();
      	return CU_get_error();
   	}

	error += NULL == CU_add_test( keygen_suite, "test of keygen_itokey", test_keygen_itokey );
	error += NULL == CU_add_test( keygen_suite, "test of keygen_itokey_v1", test_keygen_itokey_v1 );
	error += NULL == CU_add_test( keygen_suite, "test of keygen_adhoc", test_keygen_adhoc );
	
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