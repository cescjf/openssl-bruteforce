#!/bin/bash

## this encrypt a text with the encryptor, and test the main app if it can find the text
ENCRYPT=bin/encrypt
OMP=bin/omp
MPI=bin/mpi

FILE=tmp/file
ENCRYPTED_FILE=tmp/encrypted
METHOD=blowfish

##usage: ./decrypt [INPUT FILE] [KEY CODE] [METHOD] [OUTPUT FILE]
echo "===== Escalability Metrics ====="
echo

echo "Frase: La bersuit" > $FILE
$ENCRYPT $FILE 99999999 $METHOD $ENCRYPTED_FILE

export CANT_KEYS=500000

for I in 1 2 4 8
do
	echo "OpenMP time $I thread with 500000 keys..."
	export OMP_NUM_THREADS=$I
	time ./$OMP $ENCRYPTED_FILE
	echo
done

for I in 1 2 4 8
do
	echo "OpenMPI time with $I process with 500000 keys.."
	time mpirun -np $I $MPI $ENCRYPTED_FILE
	echo
done

echo "============================================="
echo

rm -f report key