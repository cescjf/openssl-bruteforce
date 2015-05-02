#!/bin/bash

ENCRYPT=bin/encrypt
DECRYPT=bin/decrypt
FILE=tmp/file
ENCRYPTED_FILE=tmp/encrypted
DECRYPTED_FILE=tmp/decrypted
KEY=123


echo
echo "Utils test..."
echo "Frase: stay hungry, stay foolish." > $FILE

METHOD=blowfish
##usage: ./encrypt [INPUT FILE] [KEY CODE] [METHOD] [OUTPUT FILE]
$ENCRYPT $FILE $KEY $METHOD $ENCRYPTED_FILE
##usage: ./decrypt [INPUT FILE] [KEY CODE] [METHOD] [OUTPUT FILE]
$DECRYPT $ENCRYPTED_FILE $KEY $METHOD $DECRYPTED_FILE && echo "$METHOD passed" || echo "$METHOD fail"

METHOD=cast5
##usage: ./encrypt [INPUT FILE] [KEY CODE] [METHOD] [OUTPUT FILE]
$ENCRYPT $FILE $KEY $METHOD $ENCRYPTED_FILE
##usage: ./decrypt [INPUT FILE] [KEY CODE] [METHOD] [OUTPUT FILE]
$DECRYPT $ENCRYPTED_FILE $KEY $METHOD $DECRYPTED_FILE && echo "$METHOD passed" || echo "$METHOD fail"

rm -f $FILE $ENCRYPTED_FILE $DECRYPTED_FILE

echo
