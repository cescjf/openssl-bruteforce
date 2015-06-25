#include "../include/encryptor.h"

int encryptor_init_data( Encryptor* data, int action, int type , unsigned char iv[ IV_LENGTH ] )
{
	if( action != BLOWFISH && action != CAST5 ) {
		fprintf( stderr, "\nencryptor: unkwnown cipher" );
		return -1;
	}

	if( type != ENCRYPT && type != DECRYPT ) {
		fprintf( stderr, "\nencryptor: unkwnow type" );
		return -1;
	}

	EVP_CIPHER_CTX_init( &data->ctx );

	data->action = action;
	data->type = type;
	data->input = NULL;
	data->output = NULL;
	data->input_size = -1;

	encryptor_set_iv( data, iv );

	if( data->type == BLOWFISH )
		data->cipher = EVP_bf_cbc();

	if( data->type == CAST5 )
		data->cipher = EVP_cast5_cbc();

	return 0;
}

int encryptor_set_input( Encryptor *data, unsigned char *input, int size )
{
	int error = 0;

	if( input == NULL ) {
		fprintf( stderr, "\nencryptor: bad input" );
		return -1;
	}

	if( size <= 0 ) {
		fprintf( stderr, "\nencryptor: bad input length " );
		return -1;
	}

	if( data->input_size != size ) {
		data->input_size = size;

		data->input = realloc( (void*) data->input, size );
		data->output = realloc( (void*) data->output, size + BLOCK_SIZE );
	}

	data->input = memcpy( data->input, input, size );

	return error;
}

int encryptor_set_key( Encryptor *data, unsigned char key[ KEY_LENGTH ] )
{
	if( key == NULL ) {
		fprintf( stderr, "\nencryptor: bad key" );
		return -1;
	}

	memcpy( data->key, key, KEY_LENGTH );

	return 0;
}

int encryptor_set_iv( Encryptor *data, unsigned char iv[IV_LENGTH] )
{
	if( iv == NULL ) {
		fprintf( stderr, "\nencryptor: bad iv" );
		return 1;
	}

	memcpy( data->iv, iv, IV_LENGTH );

	return 0;
}

int encryptor_clean_up( Encryptor *data )
{
	int error = 0;

	error = EVP_CIPHER_CTX_cleanup( &data->ctx );

	data->input_size = -1;
	free( data->input );
	free( data->output );

	return ( error != 1 ) ;
}

int encryptor_init( Encryptor *data )
{
	int error = 0;

	error = EVP_CipherInit_ex( &data->ctx, data->cipher, NULL, data->key, data->iv, data->action );

	return ( error != 1 );
}

int encryptor_update( Encryptor *data )
{
	int error = 0;

	error = EVP_CipherUpdate( &data->ctx, data->output, &data->output_length, data->input, data->input_size );

	return ( error != 1 );
}

int encryptor_final( Encryptor *data )
{
	int error = 0;

	error = EVP_CipherFinal_ex( &data->ctx, data->output + data->output_length, &data->padding_length );

    return ( error != 1 );
}

void encryptor_execute( Encryptor *data, unsigned char key[ KEY_LENGTH ] )
{
	//memcpy( data->key, key, KEY_LENGTH );
	EVP_CipherInit_ex( &data->ctx, data->cipher, NULL, key, data->iv, data->action );
	EVP_CipherUpdate( &data->ctx, data->output, &data->output_length, data->input, data->input_size );
	EVP_CipherFinal_ex( &data->ctx, data->output + data->output_length, &data->padding_length );
}
