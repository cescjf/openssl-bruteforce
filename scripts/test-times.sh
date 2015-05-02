#!/bin/bash

## this encrypt a text with the encryptor, and test the main app if it can find the text
ENCRYPT=bin/encrypt
SERIAL=bin/serial
OMP=bin/omp
MPI=bin/mpi

FILE=tmp/file
ENCRYPTED_FILE=tmp/encrypted
METHOD=blowfish

##usage: ./decrypt [INPUT FILE] [KEY CODE] [METHOD] [OUTPUT FILE]
echo "===== Executions Times ====="
echo

echo "Frase: La bersuit" > $FILE
$ENCRYPT $FILE 99999999 $METHOD $ENCRYPTED_FILE

for I in 1000 10000 100000 1000000
do
	echo "Serial time for $I keys"
	export CANT_KEYS=$I
	time ./$SERIAL $ENCRYPTED_FILE
	echo
done

for I in 1000 10000 100000 1000000
do
    echo "OpenMP time for $I keys with 2 threads"
	export CANT_KEYS=$I
	export OMP_NUM_THREADS=2
    time ./$OMP $ENCRYPTED_FILE
    echo
done

for I in 1000 10000 100000 1000000
do
    echo "OpenMPI time for $I keys with 2 proceses"
	export CANT_KEYS=$I
    time mpirun -np 2 $MPI $ENCRYPTED_FILE
    echo
done

echo "============================================="
echo

rm -f $FILE $ENCRYPTED_FILE