#!/bin/bash

## this encrypt a text with the encryptor, and test the main app if it can find the text
ENCRYPT=bin/encrypt
SERIAL=bin/serial-min
OMP=bin/omp
MPI=bin/mpi
CANT_KEYS=500000

##usage: ./decrypt [INPUT FILE] [KEY CODE] [METHOD] [OUTPUT FILE]
echo "===== Report ====="
echo "Keys: $CANT_KEYS"
echo

echo "Frase: La bersuit" > tmp/file
$ENCRYPT tmp/file 99999999 blowfish tmp/file2

for I in 1000 10000 100000 1000000
do
	echo "Serial time for $I keys"
	export CANT_KEYS=$I
	time ./$SERIAL tmp/file2
	echo
done

for I in 1000 10000 100000 1000000
do
    echo "OpenMP time for $I keys with 2 threads"
    export CANT_KEYS=$I
    time ./$OMP tmp/file2
    echo
done

for I in 1000 10000 100000 1000000
do
    echo "OpenMPI time for $I keys with 2 proceses"
    export CANT_KEYS=$I
    time mpirun -np 2 $MPI tmp/file2
    echo
done

echo "============================================="
echo

export CANT_KEYS=1000000
for I in 1 2 4 8
do
	echo "OpenMP time $I thread..."
	export OMP_NUM_THREADS=$I
	time ./$OMP tmp/file2
	echo
done

export CANT_KEYS=1000000
for I in 1 2 4 8
do
	echo "OpenMPI time with $I process.."
	time mpirun -np $I $MPI tmp/file2
	echo
done

echo "============================================="
echo