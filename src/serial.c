#define _XOPEN_SOURCE 700

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "../include/fs.h"
#include "../include/keygen.h"
#include "../include/encryptor.h"
#include "../include/commons.h"

int main( int argc, char** argv )
{
	check_parameters( argc, argv );

	unsigned char encrypted_text[ BLOCK_SIZE ];
	unsigned char iv[ IV_LENGTH ] = {1,2,3,4,5,6,7,8};
	unsigned char key[ KEY_LENGTH ];
	int keygen_characters[10] = {'0','1','2','3','4','5','6','7','8','9'};
	long cant_keys;
	long success_key = -1;
	int encryption_method = -1;
	time_t start_time, end_time;

	// get the time stamp
	start_time = time( NULL );

	read_parameters( argv, encrypted_text, &cant_keys );

	// create and init the encryptor
	Encryptor decryptor[2];
	init_decryptor( &decryptor[0], DECRYPT, BLOWFISH, iv, encrypted_text );
	init_decryptor( &decryptor[1], DECRYPT, CAST5, iv, encrypted_text );

	memset(key,ASCII_SPACE,KEY_LENGTH);
	
	// begin the decryption
	for( long i = 0; i < cant_keys; i++ ) {
		
		key[KEY_LENGTH-1] = keygen_characters[i % 10];
		key[KEY_LENGTH-2] = i/10? keygen_characters[(i/10) % 10] : ASCII_SPACE;
		key[KEY_LENGTH-3] = i/100? keygen_characters[(i/100) % 10] : ASCII_SPACE;
		key[KEY_LENGTH-4] = i/1000? keygen_characters[(i/1000) % 10] : ASCII_SPACE;
		key[KEY_LENGTH-5] = i/10000? keygen_characters[(i/10000) % 10] : ASCII_SPACE;
		key[KEY_LENGTH-6] = i/100000? keygen_characters[(i/100000) % 10] : ASCII_SPACE;
		key[KEY_LENGTH-7] = i/1000000? keygen_characters[(i/1000000) % 10] : ASCII_SPACE;
		key[KEY_LENGTH-8] = i/10000000? keygen_characters[(i/10000000) % 10] : ASCII_SPACE;
		
		encryptor_execute(&decryptor[0], key);
		
		if( memcmp( (char *)decryptor[0].output, "Frase", 5 ) == 0 ) {
			success_key = i;
			encryption_method = decryptor[0].type;
			break;
		}
		
		encryptor_execute(&decryptor[1], key);		
		
		if( memcmp( (char *)decryptor[1].output, "Frase", 5 ) == 0) {
			success_key = i;
			encryption_method = decryptor[1].type;
			break;
		}
	}
	
	end_time = time( NULL );
	print_result( success_key, encryption_method, difftime( end_time, start_time ) );

	exit(0);
}
