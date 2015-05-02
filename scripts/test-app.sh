#!/bin/bash

## this encrypt a text with the encryptor, and test the main app if it can find the text
SERIAL=bin/serial
OMP=bin/omp
MPI=bin/mpi
ENCRYPT=bin/encrypt

FILE=tmp/file
ENCRYPTED_FILE=tmp/encrypted
DECRYPTED_FILE=tmp/decrypted
METHOD=blowfish

CANT_KEYS=500000
KEY=250000

##usage: ./encrypt [INPUT FILE] [KEY CODE] [METHOD] [OUTPUT FILE]
echo "Frase: stay hungry, stay foolish." > $FILE
$ENCRYPT $FILE $KEY $METHOD $ENCRYPTED_FILE

echo
echo "Serial bruteforce decryption test..."
## execute the serial app
## usage: ./serial [ENCRYPTED_FILE_PATH]
CANT_KEYS=$CANT_KEYS ./$SERIAL $ENCRYPTED_FILE && echo "passed" || echo "fail"
echo

echo "OpenMP bruteforce decryption test..."
CANT_KEYS=$CANT_KEYS ./$OMP $ENCRYPTED_FILE && echo "passed" || echo "fail"
echo

echo "MPI bruteforce decryption test..."
CANT_KEYS=$CANT_KEYS mpirun -np 2 $MPI $ENCRYPTED_FILE && echo "passed" || echo "fail"
echo

rm -f report key