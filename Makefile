CC=gcc
MPICC=mpicc
LDFLAGS?=-std=c99 -Wall -O3
LDLIBS=-lcrypto -ldl
CUNITLIB=-lcunit

OBJ=obj
BIN=bin
SRC=src
INCLUDE=include
TEST=test

SOURCES=$(SRC)/fs.c $(SRC)/keygen.c $(SRC)/encryptor.c $(SRC)/commons.c
HEADERS=$(INCLUDE)/fs.h $(INCLUDE)/keygen.h $(INCLUDE)/encryptor.h $(INCLUDE)/commons.h
OBJECTS=$(OBJ)/keygen.o $(OBJ)/fs.o $(OBJ)/encryptor.o $(OBJ)/commons.o
TARGETS=$(BIN)/serial $(BIN)/omp $(BIN)/mpi $(BIN)/encrypt $(BIN)/decrypt $(BIN)/unit-tests

all: $(TARGETS)

test: test-unit test-utils test-app 

test-unit: $(BIN)/unit-tests
	@./$(BIN)/unit-tests 2> /dev/null
	@rm -fr output/testfile

test-app: $(TARGETS)
	@bash ./scripts/test-app.sh

test-utils: $(TARGETS)
	@bash ./scripts/test-utils.sh

test-times: $(TARGETS)
	@bash ./scripts/test-times.sh

test-scalability: $(TARGETS)
	@bash ./scripts/test-scalability.sh

install: 
	@bash ./scripts/install-libs.sh

report:
	@bash ./scripts/make-report.sh

## binary files

$(BIN)/serial: $(OBJ)/serial.o $(OBJECTS)
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)

$(BIN)/omp: $(OBJ)/omp.o $(OBJECTS)
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS) -fopenmp

$(BIN)/mpi: $(OBJ)/mpi.o  $(OBJECTS)
	$(MPICC) $(LDFLAGS) $^ -o $@ $(LDLIBS)

$(BIN)/encrypt: $(OBJ)/encrypt.o $(OBJECTS)
	$(CC) $(LDFLAGS) $^ -o $@  $(LDLIBS)

$(BIN)/decrypt: $(OBJ)/decrypt.o $(OBJECTS)
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)

## tests files

$(BIN)/unit-tests: $(OBJ)/unit-tests.o $(OBJECTS)
	$(CC) $(LDFLAGS) -L/usr/local/lib $^ -o $@ $(LDLIBS) $(CUNITLIB)

## object files

$(OBJ)/serial.o: $(SRC)/serial.c $(HEADERS)
	$(CC) $(LDFLAGS) -c $(SRC)/serial.c -o $@ $(LDLIBS)

$(OBJ)/omp.o: $(SRC)/omp.c $(HEADERS)
	$(CC) $(LDFLAGS) -c $(SRC)/omp.c -o $@ $(LDLIBS) -fopenmp

$(OBJ)/mpi.o: $(SRC)/mpi.c $(HEADERS)
	$(MPICC) $(LDFLAGS) -c $(SRC)/mpi.c -o $@

$(OBJ)/unit-tests.o: $(TEST)/unit-tests.c $(HEADERS)
	$(CC) $(LDFLAGS) -c $(TEST)/unit-tests.c -o $@ $(LDLIBS)

$(OBJ)/encrypt.o: $(SRC)/encrypt.c $(HEADERS)
	$(CC) $(LDFLAGS) -c $(SRC)/encrypt.c -o $@ $(LDLIBS) 

$(OBJ)/decrypt.o: $(SRC)/decrypt.c $(HEADERS)
	$(CC) $(LDFLAGS) -c $(SRC)/decrypt.c -o $@ $(LDLIBS)

$(OBJ)/keygen.o: $(SRC)/keygen.c $(INCLUDE)/keygen.h
	$(CC) $(LDFLAGS) -c $(SRC)/keygen.c -o $@

$(OBJ)/fs.o: $(SRC)/fs.c $(INCLUDE)/fs.h
	 $(CC) $(LDFLAGS) -c $(SRC)/fs.c -o $@

$(OBJ)/encryptor.o: $(SRC)/encryptor.c $(INCLUDE)/encryptor.h
	$(CC) $(LDFLAGS) -c $(SRC)/encryptor.c -o $@ $(LDLIBS)

$(OBJ)/commons.o: $(SRC)/commons.c $(INCLUDE)/commons.h
	$(CC) $(LDFLAGS) -c $(SRC)/commons.c -o $@ $(LDLIBS)

# commands

gcov: clean-bin
	LDFLAGS="-std=c99 -Wall -O0 --coverage -g" $(MAKE) all

lcov:
	lcov -o cov.info -c -d obj/
	genhtml -o cov cov.info

gprof: clean-bin
	LDFLAGS="-std=c99 -Wall -O0 -pg -g" $(MAKE) all

memcheck: clean-bin
	LDFLAGS="-std=c99 -Wall -O0 -g" $(MAKE) all

gcov-serial: gcov
	echo "Frase: Never be led astray onto the path of virtue." > tmp/testfile
	./bin/encrypt tmp/testfile 499999 cast5 tmp/encryptedfile
	CANT_KEYS=500000 ./bin/serial tmp/encryptedfile
	$(MAKE) lcov

gprof-serial: gprof
	echo "Frase: Never be led astray onto the path of virtue." > tmp/testfile
	./bin/encrypt tmp/testfile 499999 cast5 tmp/encryptedfile
	CANT_KEYS=500000 ./bin/serial tmp/encryptedfile
	gprof bin/serial gmon.out  > gprof.out

memcheck-serial: memcheck
	echo "Frase: Never be led astray onto the path of virtue." > testfile
	./bin/encrypt testfile 499999 cast5 encryptedfile
	CANT_KEYS=500000 valgrind --leak-check=yes bin/serial encryptedfile
	
clean: clean-bin clean-gprof clean-gcov

clean-bin:
	rm -f $(BIN)/* $(OBJ)/* 

clean-gprof:
	rm -fr gmon.out gprof.out

clean-gcov:
	rm -fr cov cov.info
