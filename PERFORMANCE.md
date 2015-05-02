# Reporte de performance

## Revision

Nro  | Descripcion
------------- | -------------
1  | Primer analisis de performance

## Ambiente de Test

Sistema Operativo | Procesador | Memoria Ram 
----------------- | ---------- | -----------
Ubuntu 14.04 64-bit LTS | Intel® Core™ i3 CPU M 330 @ 2.13GHz × 4 | 3.7 GiB

## Descripcion de la aplicacion

Implementacion de algoritmos de desencripcion por fuerza bruta, para textos cifrados con 
los algoritmos blowfish o cast5 de la libreria OpenSSL. 

La desencripcion se realiza de 3 maneras: de forma serial, a traves de hilos y memoria 
compartida utilizando la libreria OpenMP, y a traves de procesos y pasos de mensajes con 
la libreria OpenMPI

Los programas "serial", "omp", "mpi", intentan desencriptar un archivo con todas las 
claves disponibles. Cada uno implementa una forma diferente de dividir el trabajo a 
realizar. 

```
uso: ./serial ENCRYPTED_FILE
uso: ./omp ENCRYPTED_FILE
uso: mpirun [ -np NUM_PROCESS ] mpi ENCRYPTED_FILE

ENCRYPTED_FILE: archivo encriptado
NUM_PROCES: procesos utilizados por la implementacion mpi
```

Estos archivos desencriptan el primer bloque ( 8 bytes ) del archivo de entrada,
y buscan la palabra "Frase" en los primeros 5 caracteres.

La implementacion serial prueba las claves una a la vez. en caso de tener exito detiene las iteraciones
La implementacion en hilos divide el trabajo entre los hilos creados usando memoria compartida.
La implementacion en procesos divide el trabajo entre los procesos disponibles usando paso de mensajes.

En caso de exito, retornan 0, y se generan 2 archivos de salida:

* report: contiene informacion del resultado ( clave, metodo de cifrado, tiempo consumido aproximado )
* key: contiene la clave que tuvo exito en la desencripcion

De no tener exito, retornan un valor distinto de 0

El flujo de la implementacion serial es el siguiente: 

```
		
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
```

La desencripcion por fuerza bruta se realiza de manera iterativa, generando y probando las claves una a la vez, 
en caso de obtener un resultado favorable, el ciclo se detiene:

```
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
```

La implementacion con OpenMP mediante hilos y memoria compartida, tiene el mismo flujo, pero se agrega una etapa 
en donde se configura la cantidad de hilos a utilizar en la region paralela y se calcula el trabajo a realizar por 
los hilos participantes. El trabajo a realizar se divide automaticamente entre los hilos, y se utiliza un 
parallel for para ejecutar la desencripcion por fuerza bruta:

```	
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
```

La implementacion con OpenMPI mediante procesos y paso de mensajes realiza el trabajo de forma diferente. En esta, 
la utilidad mpirun es la encargada de crear los procesos que van a ejecutar el trabajo. Cada proceso es una copia del 
programa original. Cada uno seleciona una parte del trabajo a realizar y lo ejecuta de la misma manera que en la 
implementacion secuencial. Al finalizar, el proceso que tuvo exito es el encargado de generar los resultados. La 
cantidad de trabajo a realizar por cada proceso se divide de manera equitativa entre todo los procesos disponibles.
El flujo del programa es el siguiente: 

```
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
                ...         ...
                ...         ...
                |       -----------         
                --------|PROCESO N|
                        -----------
```

Cada proceso iniciado por mpirun realiza el trabajo de la misma manera que sus procesos hermanos, seleccionando 
su porcion de trabajo a traves del id de processo.

```
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
```

## Estadisticas

### Tiempos de ejecucion

Para comparar la performance de las diferentes implementaciones de los algoritmos de 
fuerza bruta, se realizo una medicion del tiempo (en segundos) ocupado por cada algoritmo, variando 
la cantidad de claves utilizadas en la desencripcion. Los datos obtenidos se presentan en la siguiente tabla:

#### Serial

time | 1000 | 10000 | 100000 | 1000000
-----|------|-------|--------|--------
real | 0m0.061s | 0m0.569s | 0m5.648s | 0m56.276s
user | 0m0.060s | 0m0.569s | 0m5.593s | 0m56.068s
sys	 | 0m0.000s | 0m0.000s | 0m0.004s | 0m0.024s


#### OpenMP with 2 threads

time | 1000 | 10000 | 100000 | 1000000
-----|------|-------|--------|--------
real | 0m0.039s | 0m0.289s | 0m2.858s | 0m28.525s
user | 0m0.066s | 0m0.572s | 0m5.700s | 0m56.859s
sys	 | 0m0.004s | 0m0.004s | 0m0.000s | 0m0.008s

#### OpenMPI with 2 process

time | 1000 | 10000 | 100000 | 1000000
-----|------|-------|--------|--------
real | 0m1.102s | 0m1.545s | 0m5.798s | 0m48.600s
user | 0m0.150s | 0m1.023s | 0m9.487s | 1m34.728s
sys	 | 0m0.023s | 0m0.029s | 0m0.051s | 0m0.157s


Queda evidente que con un numero mayor de claves, la implementacion serial tiene un peor desempeño, 
comparado con las implementaciones en paralelo.

### Escalabilidad

En la siguente tabla, se muestra como varian los tiempos de ejecucion de los algoritmos en procesar 500000 claves, a medida 
que se aumenta la cantidad de trabajadores disponibles. Para OpenMP los trabajadores son los hilos que ejecutan la seccion 
paralela del codigo, y para OpenMPI los trabajadores son los procesos utlizados para la ejecucion.

#### Open MP

time | 1 hilo | 2 hilos | 4 hilos | 8 hilos
-----|--------|---------|---------|--------
real | 0m28.106s | 0m14.607s | 0m12.655s | 0m15.364s
user | 0m28.050s | 0m29.092s | 0m46.751s | 0m56.970s
sys	 | 0m0.024s  | 0m0.044s  | 0m0.056s  | 0m0.028s

#### Open MPI

time | 1 proceso | 2 procesos | 4 procesos | 8 procesos
-----|-----------|------------|------------|-----------
real | 0m48.723s | 0m26.446s | 0m18.275s | 0m18.060s
user | 0m48.486s | 0m50.305s | 0m56.223s | 0m56.618s
sys	 | 0m0.133s  | 0m0.079s  | 0m0.172s  | 0m0.275s

El mejor desempeño se logra utilizando una cantidad de trabajadores equivalente a la cantidad de 
nucleos disponibles en el microprocesador. Al utilizar una cantidad superior, los tiempos de ejecucion 
aumentan.

## Metricas

Se utilizan 3 herramientas para la captura de metricas de performance

* [gcov](https://gcc.gnu.org/onlinedocs/gcc/Gcov.html): para el calculo de coverage
* [gprof](https://sourceware.org/binutils/docs/gprof/): para la toma de metricas de profiling
* [valgrind](http://valgrind.org/): para la toma de metricas de uso de memoria

Las metricas son tomadas unicamente para la implementacion serial. Dado que todas las implementaciones
comparten la misma base de codigo, las metricas tomadas para la implementacion serial son representativas 
para las otras implementaciones. Estas metricas serviran para definir un plan de mejoras.

Para la captura de metricas, se utiliza un test con las siguientes caracteristicas:


Descripcion  | Valor 
------------ | -----
Cantidad de claves | 500000
Frase | Frase: Never be led astray onto the path of virtue.
Clave de encriptacion | 499999
Algoritmo | cast5

A la vez. se desactivan todas las opciones de compilacion relativas a la optimizacion de codigo, y se 
utiliza la opcion -g para incorporar informacion de debugging

### Covertura de codigo

El reporte completo puede generarse utilizando make gcov-serial

#### Resumen

Type | Hit | Total | Coverage
---- | --- | ----- | --------
Lines |	171 |226 |75.7 %
Functions | 20 | 20 | 100.0 %

#### Observaciones

En el reporte de cobertura de codigo, no se encontraron posibles porciones de codigo muerto.
Se destaca la porcion de codigo siguiente:

```
22 1000001 : int keygen_itokey ( 
                unsigned char key[ KEY_LENGTH ], 
                long number 
             )
23         : {
24         : 
25 1000001 :   if( number < 0 || number >= keygen_cant_keys ) {
26       0 :     return -1;
27         :   }
28         : 
29 1000001 :   long div = 0, mod = 0;
30         :     //unsigned char genkey[8];
31 1000001 :   int isSpace = 1;
32 1000001 :   int index = 0;
33         : 
34 1000001 :   memcpy( key, "        ", 8);
35         : 
36 9000009 :   for( int i = 7; i >= 0; i--) {
37 8000008 :     div = number / keygen_mults[i];
38 8000008 :     mod = number % keygen_mults[i];
39 8000008 :     index = (i - 7) * -1 ;
40         : 
41 8000008 :     if( isSpace == 1 && div != 0 ) isSpace = 0;
42 8000008 :       key[ 8 + index ] = (div == 0 && isSpace == 1)? 
                        ASCII_SPACE : ASCII_ZERO + div;
43         : 
44 8000008 :       number = mod;
45         :    }
```

Como se puede ver, esta funcion contiene un ciclo for que se ejecuta 9000009 veces durante el test. Como primera 
observacion, cabe destacar que la optimizacion de esta function podria generar mejorar en los tiempos de ejecucion 
de la aplicacion. 

### Profiling

El reporte completo puede generarse utilizando make gprof-serial

#### Resumen

Each sample counts as 0.01 seconds.

% time | cumulative seconds|  self seconds  |   calls |  self ns/call | total ns/call | name 
-------|-------------------|----------------|---------|---------------|---------------|------
 94.19 |     0.47  |   0.47 | 1000000 |   470.97 |  470.97 | keygen_itokey
  2.00 |     0.48  |   0.01 | 1000000 |   10.02  |  10.02  | encryptor_init
  2.00 |     0.49  |   0.01 | 1000000 |   10.02  |  10.02  | encryptor_update
  2.00 |     0.50  |   0.01 |         |          |         | main
  0.00 |     0.50  |   0.00 | 1000000 |    0.00  |   0.00  | encryptor_final
  0.00 |     0.50  |   0.00 | 1000000 |    0.00  |   0.00  | encryptor_set_key
  0.00 |     0.50  |   0.00 |       2 |    0.00  |   0.00  | encryptor_init_data
  0.00 |     0.50  |   0.00 |       2 |    0.00  |   0.00  | encryptor_set_input
  0.00 |     0.50  |   0.00 |       2 |    0.00  |   0.00  | encryptor_set_iv
  0.00 |     0.50  |   0.00 |       2 |    0.00  |   0.00  | fs_write
  0.00 |     0.50  |   0.00 |       2 |    0.00  |   0.00  | init_decryptor
  0.00 |     0.50  |   0.00 |       1 |    0.00  |   0.00  | check_parameters
  0.00 |     0.50  |   0.00 |       1 |    0.00  |   0.00  | fs_read
  0.00 |     0.50  |   0.00 |       1 |    0.00  |   0.00  | generate_key_file
  0.00 |     0.50  |   0.00 |       1 |    0.00  |   0.00  | generate_report
  0.00 |     0.50  |   0.00 |       1 |    0.00  |   0.00  | keygen_getenv
  0.00 |     0.50  |   0.00 |       1 |    0.00  |   0.00  | print_result
  0.00 |     0.50  |   0.00 |       1 |    0.00  |   0.00  | read_parameters

#### Observaciones

Gprof confirma que la mayor cantidad de tiempo es ocupado en la llamada a keygen_itokey. el %94 del tiempo total 
de ejecucion es ocupado en esta function. Por lo tanto es imperativo un plan de mejoras sobre esa porcion del codigo

### Uso de memoria

El reporte completo puede generarse con make memcheck-serial

#### Resumen

```
==4316== HEAP SUMMARY:
==4316==     in use at exit: 5,276 bytes in 10 blocks
==4316==   total heap usage: 1,000,008 allocs, 999,998 frees, 2,150,000,976 bytes allocated
==4316== 
==4316== LEAK SUMMARY:
==4316==    definitely lost: 0 bytes in 0 blocks
==4316==    indirectly lost: 0 bytes in 0 blocks
==4316==      possibly lost: 0 bytes in 0 blocks
==4316==    still reachable: 5,276 bytes in 10 blocks
==4316==         suppressed: 0 bytes in 0 blocks
==4316== Reachable blocks (those to which a pointer was found) are not shown.
==4316== To see them, rerun with: --leak-check=full --show-leak-kinds=all
==4316== 
==4316== For counts of detected and suppressed errors, rerun with: -v
==4316== ERROR SUMMARY: 0 errors from 0 contexts (suppressed: 0 from 0)
```

## Observaciones

Valgrind no reporta problemas en el uso de memoria. Por lo tanto no se tomaran acciones en este campo.
Este primer reporte puede utilizarse para verificar si las posibles mejoras ocasionan la aparicion de 
nuevos problemas de memoria.

## Conclusion

A modo de conclusion, se observa que la performance de la aplicacion puede mejorarse a traves de la
refactorizacion de la funcion keygen_itokey. La misma se usa para la generacion de claves para los 
desencriptadores. El uso de la misma en la implementacion serial se muestra a continuacion:

	// begin the decryption
	for( long i = 0; i < cant_keys && success_key == -1; i++ ) {
		keygen_itokey( key, i );

		encryptor_set_key( &decryptor[0], key );
		encryptor_init( &decryptor[0] );
		encryptor_update( &decryptor[0] );
		encryptor_final( &decryptor[0] );

		if( memcmp( (char *)decryptor[0].output, "Frase", 5 ) == 0 ) {
			success_key = i;
			encryption_method = decryptor[0].type;
		}
	}

	for( long i = 0; i < cant_keys && success_key == -1; i++ ) {
		keygen_itokey( key, i );

		encryptor_set_key( &decryptor[1], key );
		encryptor_init( &decryptor[1] );
		encryptor_update( &decryptor[1] );
		encryptor_final( &decryptor[1] );

		if( memcmp( (char *)decryptor[1].output, "Frase", 5 ) == 0 ) {
			success_key = i;
			encryption_method = decryptor[1].type;
		}
	}
    
Como se puede observar, esta funcion es utilizada en cada ciclo de desencripcion, y cada ciclo se 
ejecuta para cada uno de los algoritmos utilizados para la desencripcion.

## Recomendaciones

* Crear stress tests para la funcion keygen_itokey
* Refactorizar la funcion keygen_itokey:
    - Eliminar el ciclo for en la misma, si es posible.
    - Garantizar el funcionamiento de la misma luego de la refactorizacion, a traves de la ejecucion 
    de test unitarios.
    - Rehacer el analisis de performance luego de la refactorizacion, para verificar resultados.
* Si es posible, reducir la cantidad de llamadas a la funcion keygen_itokey
* Si es posible, implementar keygen_itokey como una funcion inline, para evitar la invocacion de la misma