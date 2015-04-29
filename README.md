DESCRIPCION
===========

Implementacion de algoritmos de desencripcion por fuerza bruta, para textos cifrados con 
los algoritmos blowfish o cast5 de la libreria OpenSSL. 

La desencripcion se realiza de 3 maneras: de forma serial, a traves de hilos y memoria 
compartida utilizando la libreria OpenMP, y a traves de procesos y pasos de mensajes con 
la libreria OpenMPI

PREREQUISITOS
=============

	* openmpi-1.6.5: Open MPI lib
	* openssl-1.0.1e: Open SSL crypto lib
	* CUnit-2.1-2: C Unit Testing lib

	nota: libopenmp viene incluida por defecto en el compilador gcc

INSTALACION
===========

* instalacion usando make

        shell$ make install

* instalacion manual

        Descomprimir todas las librerias ubicadas en el directorio libs

        shell$ tar -zxvf CUnit-2.1-2.tar.gz
        shell$ tar -zxvf openmpi-1.6.5.tar.gz
        shell$ tar -zxvf openssl-1.0.1e.tar.gz

* openmpi-1.6.5:

        shell$ cd libs/openmpi-1.6.5
        shell$ ./configure 
        [...lots of output...]
        shell$ sudo make all install

* CUnit-2.1-2:

        shell$ cd libs/CUnit-2.1-2
        shell$ ./configure                    
        shell$ make
        shell$ make check
        shell$ sudo make install

* openssl-1.0.1e:
	
        shell$ cd libs/openssl-1.0.1
        shell$ ./config --prefix=/usr/local
        shell$ make
        shell$ make test
        shell$ sudo make install

Nota: en caso de error, leer los archivos INSTALL ubicados en los directorios de las librerias, e intentar reinstalar

COMPILACION
===========

* para compilar los codigos fuentes:

	   shell$ make all

* se generan 5 ejecutables en la carpeta /bin
	
	* serial: la implementacion serial de el algoritmo de fuerza bruta
	* omp: la implementacion en hilos y memoria usando OpenMP
	* mpi: la implementacion en procesos y paso de mensajes usando OpenMPI
	
	* encrypt: encriptador de archivos
	* decrypt: desencriptador de archivos

	* unit-test: test unitarios de los componentes del algoritmo

TEST
====

* para ejecutar los test:

        ## ejecutar todos los test
        shell$ make test

        ## unicamente test unitarios
        shell$ make test-unit

        ## unicamente test de aplicacion 
        shell$ make test-app  

        ## unicamente test de utilidades encrypt y decrypt
        shell$ make test-utils

nota: todos los test deberian terminar en "passed".


VARIABLES DE AMBIENTE
=====================

* CANT_KEYS: Cantidad de claves para utilizar en la desencripcion por fuerza bruta.
Por defecto, si no se configura la cantidad de claves, se utilizan 10^8 claves en la desencripcion. 
	
		shell$ export CANT_KEYS=[ cantidad de claves ]

* OMP_NUM_THREADS: Cantidad de hilos a utilizar por la implementacion en OpenMP.
Por defecto, si no se configura la cantidad de hilos, se utilizan 4 hilos
para la desencripcion usando OpenMP. Es adecuado configurar la cantidad 
de hilos a utilizar igual al numero de cores fisicos que posee el microprocesador,
si se utiliza un numero mayor, se obtienen peores resultados de performance. Un 
numero menor no aprovecha todas las capacidades del microprocesador. 

		shell$ export OMP_NUM_THREADS=[ hilos hijos a utilizar ]

EJECUCION
=========

## encriptador

El programa "encrypt" encripta un archivo, con una clave y un metodo determinado, 
y guarda el texto encryptado en un archivo de salida. Todos los parametros son 
obligatorios. Las claves deben ser numericas mayores a cero y menores a CANT_KEYS.

	uso: ./encrypt [INPUT FILE] [KEY CODE] [METHOD] [OUTPUT FILE]

	INPUT FILE: archivo para encriptar
	KEY CODE: clave numerica de encripcion ( entre 1 y CANT_KEYS )
	METHOD: metodo de cifrado ( blowfish o cast5 )
	OUTPUT FILE: archivo de salida

Retorna 0 si la encripcion tuvo exito y se genera el archivo de salida. 
En caso de error, retorna un valor distinto de cero, y se imprime en stdout 
la causa del error.

## desencriptador
El programa "decrypt" desencripta un archivo, con una clave y metodo determinado, 
y guarda el texto desencriptado en un archivo de salida. Todos los parametros son
obligatorios. Las claves deben ser numericas mayores a cero y menores a CANT_KEYS.

	uso: ./decrypt [INPUT FILE] [KEY CODE] [METHOD] [OUTPUT FILE]

	INPUT FILE: archivo para desencriptar
    KEY CODE: clave numerica de desencripcion ( entre 1 y CANT_KEYS )
    METHOD: metodo de cifrado ( blowfish o cast5 )
    OUTPUT FILE: archivo de salida

Retorna 0 si la encripcion tuvo exito y se genera el archivo de salida.
En caso de error, retorna un valor distinto de cero, y se imprime en stdout
la causa del error.

## algoritmos de fuerza bruta
Los programas "serial", "omp", "mpi", intentan desencriptar un archivo con todas las 
claves disponibles. Cada uno implementa una forma diferente de dividir el trabajo a 
realizar. 

	uso: ./serial ENCRYPTED_FILE
	uso: ./omp ENCRYPTED_FILE
	uso: mpirun [ -np NUM_PROCESS ] mpi ENCRYPTED_FILE

	ENCRYPTED_FILE: archivo encriptado
	NUM_PROCES: procesos utilizados por la implementacion mpi

Estos archivos desencriptan el primer bloque ( 8 bytes ) del archivo de entrada,
y buscan la palabra "Frase" en los primeros 5 caracteres.

La implementacion serial prueba las claves una a la vez. en caso de tener exito detiene las iteraciones
La implementacion en hilos divide el trabajo entre los hilos creados usando memoria compartida.
La implementacion en procesos divide el trabajo entre los procesos disponibles usando paso de mensajes.

En caso de exito, retornan 0, y se generan 2 archivos de salida:

* report: contiene informacion del resultado ( clave, metodo de cifrado, tiempo consumido aproximado )
* key: contiene la clave que tuvo exito en la desencripcion

De no tener exito, retornan un valor distinto de 0

DESARROLLO DEL PROYECTO
=======================

El desarrollo del proyecto empieza analizando los requerimientos del mismo, en el cual se pedia la utilizacion 
de la libreria OpenSSL para desencriptar por 
fuerza bruta un archivo. El mismo estaba encriptado con los method Blowfish o Cast5, con una clave de 16 
bytes con una sintaxis predeterminada. La desencripcion debia implementarse de forma secuencial o en 
paralelo, usando las librerias OpenMP y OpenMPI.

El primer paso fue explorar las librerias a utilizar. Para ello, se descargaron e instalaron las librerias 
y se estudio su funcionamiento, y se realizaron codigos ejemplos simples para probar su funcionamiento. 
Estos ejemplos pueden encontrarse en el directorio examples, donde se encuentran ejemplos de uso de la 
libcrypto de OpenSSL, OpenMP y OpenMPI. Tambien se estudio la forma de compilar y ejecutar estos programas. 
Durante esta actividad de investigacion, fueron almacenandose las documentaciones consultadas. Las mismas se 
encuentran en el directorio doc.

Una vez comprendida las herramientas a utilizar, se definio la infraestructura a utilizar para el desarrollo 
del proyecto. En primera instancia, se definio la estructura del proyecto en directorios:

	/
	----/bin           // Todos los ejecutables  
	----/obj           // Todos los codigos objetos
	----/doc           // Documentacion 
	----/examples      // Codigos de ejemplo
	----/input         // Archivos de entrada para los algoritmos
	----/output        // Archivos de salida de los algoritmos
	----/libs          // Librerias utilizadas
	----/scripts       // Bash Scripts
	----/include       // Todos los archivos de cabecera
	----/src           // Todos los codigos fuente

Una vez definida la estructura del proyecto, se decidio utilizar una herramienta de versionamiento de codigo. 
Se utilizo Mercurial, y se creo un repositorio local para el proyecto. Esto facilito la recuperacion de archivos 
en caso de errores durante el desarrollo. Se puede visualizar el desarrollo total del proyecto usando:

    shell$ hg history

Luego de un analisis mas detallado de los requerimientos, se define una primera arquitectura del proyecto. Se 
identificaron actividades comunes a todas las implementaciones:

* Lectura de parametros, variables de ambiente y generacion de resultados
* Lectura/Escritura de archivos del File System.
* Generacion de claves utilizadas en la encripcion/desencripcion
* Ejecucion de la encripcion/desencripcion

La arquitectura se dividio en componentes, los cuales implementan las funciones que realizan las actividades comunes.
La division en componentes permitio simplificar el desarrollo y evolucion del software, permitiendo la reutilizacion 
de codigo. Los componentes definidos fueron los siguientes:

* commons:		provee funciones de inicializacion de variables y generacion de resultados
* keygen: 		provee funciones para la generacion de claves de 16 bytes
* fs: 			provee funciones para la lectura/escritura de archivos
* encryptor: 	provee funciones para la encrypcion/desencripcion con blowfish y cast5

Para el desarrollo de los componentes, se decidio utilizar la tecnica TDD (Test Driven Development), para garantizar el 
funcionamiento unitario de cada componente y mejorar la calidad final del software. Por ello, se uso la libreria 
CUnit para la creacion y ejecucion de test unitarios de componentes. Cada componente fue testeado individualmente 
durante su desarrollo antes de realizar la integracion de los mismos en una sola aplicacion.

Para la compilacion del codigo y ejecucion de tests, se utilizo la herramienta make, lo cual facilita la ejecucion 
de tareas repetitivas y de limpieza. Para ello, se genero un Makefile que se encuentra en la carpeta raiz del proyecto. 

Una vez finalizado el desarrollo de los componentes requeridos, se paso a la integracion de los mismos. Para ello, se 
desarrollaron dos utilidades: encrypt y decrypt. Estos usan los compontes desarrollados para realizar la encripcion y 
desencripcion de un archivo. Se desarrollo un test en un script de bash para validar si el funcionamiento era correcto. 
El test encripta un archivo con una clave y metodo determinado, para luego desencriptarlo con la misma clave, y 
valida si el archivo desencriptado es igual al archivo de entrada original.

En esta instancia, se detectaron algunos problemas de funcionamiento. Por ello, se modificaron y extendieron algunos 
componentes. Estas utilidades luego se utilizaron para testear el funcionamiento de las aplicaciones principales.

A continuacion, se desarrollo la primera implementacion del algoritmo de fuerza bruta, la cual realiza la desencripcion 
de forma secuencial. El flujo de la implementacion serial es el siguiente: 
	
		
		 INICIO
		 ------
			|
	-------------------- 
	|VALIDAR PARAMETROS|
	--------------------
			|
	-------------------------------------------------
	|LEER ARCHIVO DE ENTRADA Y VARIABLES DE AMBIENTE|
	-------------------------------------------------
			|
	------------------------------
	|INICIALIZAR DESENCRIPTADORES|
	------------------------------
			|
	--------------------------------
	|DESENCRIPCION POR FUERZA BRUTA|
	--------------------------------
			|
	--------------------
	|GUARDAR RESULTADOS|
	--------------------
			|
		---------

La desencripcion por fuerza bruta se realiza de manera iterativa, generando y probando las claves una a la vez, 
en caso de obtener un resultado favorable, el ciclo se detiene:

	--------------------
    |FOR I IN CANT_KEYS|
    --------------------
            |
			-------------
			|			|
			|	-----------------
			|	|GENERAR CLAVE I|
			|	-----------------
			|			|
			|	-------------------------
			|	|DESENCRIPTAR EL MENSAJE|
			|	-------------------------
			|			|
			|	----------------------
			|	|VALIDAR EL RESULTADO|
			|	----------------------
			|			|
			-------------
			|
		---------

La implementacion con OpenMP mediante hilos y memoria compartida, tiene el mismo flujo, pero se agrega una etapa 
en donde se configura la cantidad de hilos a utilizar en la region paralela y se calcula el trabajo a realizar por 
los hilos participantes. El trabajo a realizar se divide automaticamente entre los hilos, y se utiliza un 
parallel for para ejecutar la desencripcion por fuerza bruta:

		
         INICIO
         ------
            |
    --------------------
    |VALIDAR PARAMETROS|
    --------------------
            |
    -------------------------------------------------
    |LEER ARCHIVO DE ENTRADA Y VARIABLES DE AMBIENTE|
    -------------------------------------------------
            |    
	---------------------------------------
    |CONFIGURAR NUMERO DE HILOS A UTILIZAR|
    ---------------------------------------
            |
    ------------------------------
    |INICIALIZAR DESENCRIPTADORES|
    ------------------------------
            |
    --------------------------------------------
    |DESENCRIPCION POR FUERZA BRUTA EN PARALELO|
    --------------------------------------------
            |
    --------------------
    |GUARDAR RESULTADOS|
    --------------------
            |
        ---------      

La implementacion con OpenMPI mediante procesos y paso de mensajes realiza el trabajo de forma diferente. En esta, 
la utilidad mpirun es la encargada de crear los procesos que van a ejecutar el trabajo. Cada proceso es una copia del 
programa original. Cada uno seleciona una parte del trabajo a realizar y lo ejecuta de la misma manera que en la 
implementacion secuencial. Al finalizar, el proceso que tuvo exito es el encargado de generar los resultados. La 
cantidad de trabajo a realizar por cada proceso se divide de manera equitativa entre todo los procesos disponibles.
El flujo del programa es el siguiente: 

		--------
        |MPIRUN|
        --------
			|       ----------- 
			|-------|PROCESO 1|
			|       -----------
			|			
			|       -----------         
			|-------|PROCESO 2|
			|       -----------
		   ...
           ...
			|       -----------         
			--------|PROCESO N|
				    -----------

Cada proceso iniciado por mpirun realiza el trabajo de la misma manera que sus procesos hermanos, seleccionando 
su porcion de trabajo a traves del id de processo.

        -----------
        |PROCESO I|
        -----------
            |
    --------------------
    |VALIDAR PARAMETROS|
    --------------------
            |
    -------------------------------------------------
    |LEER ARCHIVO DE ENTRADA Y VARIABLES DE AMBIENTE|
    -------------------------------------------------
            |
    -----------------------------
    |CALCULAR TRABAJO A REALIZAR|
    -----------------------------
            |
    ------------------------------
    |INICIALIZAR DESENCRIPTADORES|
    ------------------------------
            |
    --------------------------------
    |DESENCRIPCION POR FUERZA BRUTA|
    --------------------------------
            |
    --------------------
    |GUARDAR RESULTADOS|
    --------------------
            |
        -------
        |FINAL|
        -------

TIEMPOS DE EJECUCION
====================

Para comparar la performance de las diferentes implementaciones de los algoritmos de 
fuerza bruta, se realizo una medicion del tiempo (en segundos) ocupado por cada algoritmo, variando 
la cantidad de claves utilizadas en la desencripcion. Los datos obtenidos se presentan 
en la siguiente tabla:

			
 claves	|serial	|openMP	|openMPI
--------|-------|-------|-------
1000	|0.061	|0.024	|1.072
10000	|0.539	|0.198	|1.339
100000	|5.321	|1.75	|3.864
1000000 |53.175	|17.456	|29.167
			
Queda evidente que con un numero mayor de claves, la implementacion serial tiene un peor desempeño, 
comparado con las implementaciones en paralelo.

En la siguente tabla, se muestra como varian los tiempos de ejecucion de los algoritmos, a medida 
que se aumenta la cantidad de trabajadores disponibles. En el caso de la implementacion serial, 
siempre hay un trabajador. Para OpenMP los trabajadores son los hilos que ejecutan la seccion 
paralela del codigo, y para OpenMPI los trabajadores son los procesos utlizados para la ejecucion.


 hilos/procesos	|openmp	|serial	|openmpi
----------------|-------|-------|-------
		1		|52.977	|53.175	|56.866
		2		|26.611	|53.175	|29.194
		4		|16.646	|53.175	|18.396
		8		|16.501	|53.175	|19.398
		16		|24.372	|53.175	|30.479

El mejor desempeño se logra utilizando una cantidad de trabajadores equivalente a la cantidad de 
nucleos disponibles en el microprocesador. Al utilizar una cantidad superior, los tiempos de ejecucion 
aumentan.

PRUEBA REAL
===========

Se midio el tiempo ocupado para desencriptar uno de los archivos brindados para pruebas. Se utilizo la 
implementacion con OpenMP y OpenMPI para probar la totalidad de las claves, utilizando 4 hilos de 
ejecucion y 4 procesos, respectivamente. Los resultados son los siguientes:

* Desencripcion usando OpenMP

        real    26m55.728s
        user    107m41.000s
        sys     0m0.716s

* Desencripcion usando MPI

        real    28m39.777s
        user    113m37.130s
        sys     0m3.668s


RESULTADOS
==========

* Oliver1:

        Frase: Nunca falta alguien que sobra.
        Metodo: blowfish.
        Clave: 46712343.

* Oliver2:

        Frase: El saber no ocupa lugar.
        Metodo: cast5.
        Clave: 49999913.

* Navarro1: 

        Frase: Quien malandra, mal acaba.
        Metodo: blowfish.
        Clave: 99921223.

* Navarro2:

        Frase: En el reino del reves, los gatos no dicen miau, dicen yes porque estudian mucho ingles.
        Metodo: cast5.
        Clave: 42904536.


