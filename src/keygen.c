#include "../include/keygen.h"

static long keygen_mults[8] = {1,10,100,1000,10000,100000,1000000,10000000};
static long keygen_cant_keys = CANT_KEYS;
int keygen_characters[10] = {'0','1','2','3','4','5','6','7','8','9'};

int keygen_getenv( long* cant_keys )
{
	char *env = getenv("CANT_KEYS");

	if( env != NULL ){
		*cant_keys = atol(env);
		keygen_cant_keys = atol(env);
	}
	else {
		*cant_keys = CANT_KEYS;
		keygen_cant_keys = CANT_KEYS;
	}
	
	return 0;
}

int keygen_itokey( unsigned char key[ KEY_LENGTH ], long number )
{

	if( number < 0 || number >= keygen_cant_keys ) {
		return -1;
	}

	long div = 0, mod = 0;
	//unsigned char genkey[8];
	int isSpace = 1;
	int index = 0;

	memcpy( key, "        ", 8);

	for( int i = 7; i >= 0; i--) {
		div = number / keygen_mults[i];
		mod = number % keygen_mults[i];
		index = (i - 7) * -1 ;

		if( isSpace == 1 && div != 0 ) isSpace = 0;
		key[ 8 + index ] = (div == 0 && isSpace == 1)? ASCII_SPACE : ASCII_ZERO + div;

		number = mod;
	}

	key[ KEY_LENGTH - 1 ] = ( key[ KEY_LENGTH - 1 ] == ASCII_SPACE )? ASCII_ZERO : key[ KEY_LENGTH - 1 ];

	//genkey[7] = ( genkey[7] == ASCII_SPACE )? ASCII_ZERO : genkey[7];

	//memcpy( &key[8], genkey, 8);

	return 0;
}

void keygen_itokey_v1( unsigned char key[ KEY_LENGTH ], long number )
{	
	unsigned char* pointer = &key[KEY_LENGTH-1];
	
	*pointer = ASCII_ZERO;
	
	for( ; number > 0; --pointer )
	{
			*pointer = keygen_characters[number % 10];
			number = number / 10;
	}
}