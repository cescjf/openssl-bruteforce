#define _XOPEN_SOURCE 700
#define BLOCK_SIZE 8
#define IV_LENGTH 8

#define OPENSSL_NO_ENGINE 1

/** ascii code for space */
#define ASCII_SPACE 32
/** ascii code for character 0 */
#define ASCII_ZERO 48
/** default number of keys to use */
#define CANT_KEYS 100000000
/** length of the generated keys */
#define KEY_LENGTH 16

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <openssl/evp.h>
#include <openssl/err.h>
#include <fcntl.h>
#include <unistd.h>

static int keygen_characters[10] = {'0','1','2','3','4','5','6','7','8','9'};
static unsigned char iv[] = {1,2,3,4,5,6,7,8};
	
typedef struct {
	/** OpenSSL cipher context */
	EVP_CIPHER_CTX ctx;
	/** OpenSSL cipher **/
	const EVP_CIPHER *cipher;

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

void gen_key(unsigned char *pointer, long i){
	for(*pointer = ASCII_ZERO; i > 0; i /= 10, --pointer){
		*pointer = keygen_characters[ i % 10];
	}
}

void get_cant_keys( long* cant_keys )
{
	char *env = getenv("CANT_KEYS");

	if( env != NULL ){
		*cant_keys = atol(env);
	}
	else {
		*cant_keys = CANT_KEYS;
	}
}

void do_decrypt(Encryptor *d){
	EVP_CipherInit_ex( &d->ctx, d->cipher, NULL, d->key, iv, 0 );
	EVP_CipherUpdate( &d->ctx, d->output, &d->output_length, d->input, d->input_size );
	EVP_CipherFinal_ex( &d->ctx, d->output + d->output_length, &d->padding_length );		
}


int fs_read( const char* path, unsigned char *output, int size )
{
	int file = open( path, O_RDONLY );
	int readed;

	if( file == -1 ) {
		perror("fs_read: ");
		return -1;
	}

	readed = read( file, output, size );

	if( readed == -1 ) {
		perror("fs_read: ");
		return -1;
	}

	close(file);

	return readed;
}

int main( int argc, char** argv )
{
	unsigned char encrypted_text[ BLOCK_SIZE ];
	long cant_keys;
	Encryptor blowfish;
	//Encryptor cast5;
	//long success_key = -1;
	//int encryption_method = -1;
	//time_t start_time, end_time;

	// get the time stamp
	//start_time = time( NULL );

	//read_parameters( argv, encrypted_text, &cant_keys );

	// create and init the encryptor
	//Encryptor decryptor[2];
	//init_decryptor( &decryptor[0], DECRYPT, BLOWFISH, iv, encrypted_text );
	//init_decryptor( &decryptor[1], DECRYPT, CAST5, iv, encrypted_text );
	EVP_CIPHER_CTX_init( &blowfish.ctx );
	blowfish.cipher = EVP_bf_cbc();
	blowfish.input = NULL;
	blowfish.output = NULL;
	
	blowfish.input = realloc( (void*) blowfish.input, BLOCK_SIZE );
	blowfish.output = realloc( (void*) blowfish.output, BLOCK_SIZE + BLOCK_SIZE );
	
	fs_read(argv[1],encrypted_text,BLOCK_SIZE);	
	memcpy( blowfish.input, encrypted_text, BLOCK_SIZE );
	get_cant_keys(&cant_keys);
	
	memset(blowfish.key,ASCII_SPACE,KEY_LENGTH);
	
	EVP_CipherInit_ex( &blowfish.ctx, blowfish.cipher, 0, 0, iv,0);
	EVP_CIPHER_CTX_set_key_length( &blowfish.ctx, KEY_LENGTH );
	EVP_CIPHER_CTX_set_padding(&blowfish.ctx, 0);
	
	// begin the decryption
	for( long i = 0; i < cant_keys; i += 1 ) {
		//gen_key(&blowfish.key[KEY_LENGTH-1],i);
		//
		//EVP_CipherInit_ex( &blowfish.ctx, 0, 0, blowfish.key, 0, 0);
		(&blowfish.ctx)->cipher->init(&blowfish.ctx, blowfish.key, iv, 0);
		//bf_init_key(&blowfish.ctx,blowfish.key,iv,0);
		EVP_DecryptUpdate( &blowfish.ctx, blowfish.output, &blowfish.output_length, blowfish.input, BLOCK_SIZE );
		//EVP_DecryptFinal_ex( &blowfish.ctx, blowfish.output, &blowfish.padding_length );		
		/*
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
		*/
	}
	
	//end_time = time( NULL );
	//print_result( success_key, encryption_method, difftime( end_time, start_time ) );

	exit(0);
}
