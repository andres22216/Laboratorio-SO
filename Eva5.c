/*El objetivo de esta práctica es familiarizarse con el uso de las primitivas de sincronización (semáforos) 
y los problemas de concurrencia que pueden presentarse (condiciones de carrera, deadlocks) al construir 
aplicaciones multihuiladas. También se pretende explorar herramientas de control de versiones y de trabajo en
equipo.

Implemente un programa, en C, considerando

1. El programa se ejecuta así:
$./nombre_programa numeroDeHilos cantidad_tiempo_a_correr numero_cuentas valor_inicial

2. El hilo princial debe crear tantas cuentas como indique el parámetro numero_cuentas. 
El valor inicial de las cuentas estará especificado por valor_inicial.

3. En una cuenta se pueden realizar depósitos, retiros y transferencias.

4. El hilo principal creará la cantidad de hilos definida en numeroDeHilos. 
Cada hilo seleccionará de manera aleatoria dos cuentas y realizará transferencias de una cantidad o monto 
igualmente aleatorio. Cada hilo repetirá esta operación durante la cantidad de tiempo especificada en cantidad_tiempo_a_correr.

5. Al finalizar cantidad_tiempo_a_correr, el hilo principal deberá imprimir el valor de cada una de las cuentas y el balance total
de la sumatoria de todas las cuentas.

6. Realice un programa de prueba que llame automáticamente el programa anterior pero con diferentes vectores de prueba.
Dicho programa deberá leer la salida producida con cada vector de prueba y comprobar que el resultado es correcto.
Los vectores de prueba deberán estar almacenados en un archivo.

7. Corra el programa de prueba de así:
$./programa_prueba nombre_programa archivo_vectores_prueba

8. Imprima el resultado de cada vector indicando si pasó o no pasó.

9. Los vectores de prueba deberán ser almacenados por líneas, así:

numero_hilos cantidad_tiempo_a_correr numero_cuentas valor_inicial CANTIDAD_DE_REPETICIONES_DE_ESTE_VECTOR 

El parámetro CANTIDAD_DE_REPETICIONES_DE_ESTE_VECTOR indica la cantidad de veces que se deberá repetir este vector.

NOTAS:

1. Escriba la estrategia de solución INICIAL, explicando la estructura del programa y las ideas para resolver la práctica.
Explique cómo piensa utilizar las primitivas ofrecidas por el sistemas operativo para evitar condiciones de carrera y posibles interbloqueos.

2. Describa los problemas que encontró durante la solución de la práctica mostrando cómo se manifestaban, cómo los detectó y cómo los solucionó.

3. Describa como fue la solución final, explicando la estructura del programa y las ideas finalmente implementadas y funcionales.

4. Compare la solución inicial y la solución final.

5. EL TRABAJO DEBERÁ SER REALIZADO EN PAREJAS!

6. Suba el trabajo realizado a GitHub, incluyendo el código del programa, el programa de prueba y archivos de prueba.

7. TODA LA DOCUMENTACIÓN SOLICITADA deberá estar también en github, utilizando una página de presentación o la wiki.

8. NO SE RECIBIRÁ NADA POR CORREO ELECTRÓNICO.
*/


/*

INTEGRANTES: 

YAMIT CARDOZO
ANDRES ORTIZ

*/




#include <pthread.h>
#include <stdio.h>
#include <stdlib.h> 
#include <time.h>
#include <sys/types.h>
#include <unistd.h>

void deposito(double,int);
void retiro(double,int);
int cantidadTransacciones;
int cantidadCuentas;
int valorActual;

pthread_mutex_t *sum_mutex;
struct creacionCuentas  *pcuentas;


//////////////////////// creacion estructura de cuentas /////////////////////
struct creacionCuentas{

	//identificador cuenta
	int id;
	//saldo actual de la cuenta
	double saldo;
};


///////////////// metodo que realiza los hilos ///////////////////////
void transferencia(){
	
	double valorCuenta; //se usa para guardar el saldo que esta en la cuenta
	int auxElegido=0; //usado para guardar el primer numero de cuenta elegido
	int auxElegido2=0; //usado para guardar el segundo numero de cuenta elegido
	double valorRando=0; //usado para guardar la cantidad que sera restada de la cuenta
	int con_transacciones=0; //usado para contar el numero de transacciones

	//printf("fui bien llamado\n");		
	
	int rai; //usado para guardar el valor del random
	double rad;//usado para guardar el valor del random calculado con la funcion drand48
	int w1,w2; //usados para guardar el valor de los bloqueos
	
	//ciclo para realizar el numero de transacciones pasado como parametro
	while(con_transacciones<cantidadTransacciones){
		
		rai = rand();
		auxElegido = rai%(cantidadCuentas);
		
		//printf("cuenta seleccionada1 %d\n",auxElegido);
		rai = rand();
		auxElegido2 = rai%(cantidadCuentas);

		//printf("cuenta seleccionada2 %d\n",auxElegido2);
		//srand48(time(NULL));
		rad= drand48();

		//verificamos que las cuentas escogidas sean diferentes
		if(auxElegido!=auxElegido2){
			w1=pthread_mutex_trylock(&sum_mutex[auxElegido]);	
			//printf("puedo obtener bloqueo para la primer cuenta %d\n",w1);
			w2=pthread_mutex_trylock(&sum_mutex[auxElegido2]);
			//printf("puedo obtener bloqueo para la segunda cuenta %d\n",w2);

			//verificamos si podemos obtener el bloqueo para las dos cuentas
			if((w1 == 0) && (w2 == 0)){
	
				valorCuenta = pcuentas[auxElegido].saldo;
				valorRando = rad*(valorCuenta);
				//printf("valor random %f random %lf valor cuenta (saldo)%lf\n",valorRando,rad,valorCuenta);

				///// si logra entrar bloquea
				//printf("bloqueo hilo para hacer transacciones %d\n", getpid());
				retiro(valorRando,auxElegido);
				deposito(valorRando,auxElegido2);	
				
				pthread_mutex_unlock(&sum_mutex[auxElegido]);	
				pthread_mutex_unlock(&sum_mutex[auxElegido2]);
				//printf("desbloquio hilo %d\n", getpid());
			}else{
				printf("transaccion no realizada\n");
				if(w1 == 0){
					pthread_mutex_unlock(&sum_mutex[auxElegido]);
				}
				if(w2 == 0){
					pthread_mutex_unlock(&sum_mutex[auxElegido2]);	
				}
			}

		} 


		sleep(0.01);
		con_transacciones++;

	} 

	pthread_exit (NULL);

}


//////////////// FUNCION DEPOSITO ////////////////////////////////////

void deposito(double valor,int elegido){

		

	double sumatoria = pcuentas[elegido].saldo+valor;
	pcuentas[elegido].saldo = sumatoria;
}



//////////////// FUNCION RETIRO //////////////////////////////////////

void retiro(double valor,int elegido){

	

	double sumatoria = pcuentas[elegido].saldo-valor;
	pcuentas[elegido].saldo = sumatoria;
}



///////////////metodo principal con el que comienza el programa////////////////


int main(int argc, char *argv[]){
	
	// declaración de variables
	
	
	
	
	int cantidadHilos = atoi(argv[1]);
	cantidadCuentas = atoi(argv[3]);
	double valorInicial = atof(argv[4]);
	cantidadTransacciones = atoi(argv[2]);

	printf("se crearan:\t%d\t hilos\n", cantidadHilos);
	printf("corridas:\t%d\t veces\n", cantidadTransacciones);
	printf("se crearan:\t%d\t cuentas\n",cantidadCuentas);
	printf("valor inicial de cada cuenta:\t%f\n\n",valorInicial);

	

	int i=0;	
	
	//arreglo para guardar los idenficadores de los hilos
	pthread_t hilos_ids[cantidadHilos];

	//arreglo del tipo creacionCuentas usado para almacenar las cuentas
	struct creacionCuentas  cuentas[cantidadCuentas];

	//arreglo para guardar los mutex
	pthread_mutex_t arrMutex[cantidadCuentas];
	pcuentas = cuentas;	
	sum_mutex = arrMutex;

		//inicializamos los mutex
		for(i=0; i<cantidadCuentas; ++i){
		pthread_mutex_init(&arrMutex[i], NULL);
		}

	//asignamos los el valor inicial a cada cuenta
	for(i=0; i<cantidadCuentas; ++i){

		struct creacionCuentas cuenta = {i+1,valorInicial};
		cuentas[i] = cuenta;
		
	}


	srand(time(NULL)*getpid()); // semillas para los random
	srand48(time(NULL)*getpid()); // semillas para los random

	//loop para crear y lanzar los hilos, para que ejecuten la funcion transferencia
	for(i=0;i<cantidadHilos;++i) pthread_create (&hilos_ids[i],NULL,(void *) transferencia,NULL);

	//loop para esperar por todos los hilos
	for(i=0;i<cantidadHilos;++i) pthread_join(hilos_ids[i],NULL);

	//loop para destruir todos los mutex
	for(i=0; i<cantidadCuentas; ++i){

		pthread_mutex_destroy(&arrMutex[i]);

		
	}	

	double balance=0;
	double sal;
	
	//loop para imprimir los saldos de las cuentas
	for (i=0;i<cantidadCuentas;i++){

	sal = pcuentas[i].saldo;
	printf("la cuenta\t%d tiene un saldo de:\t%f\n",pcuentas[i].id,sal);
	balance = balance + sal;
	
	}
	
	//imprimimos el valor del balance
	printf("el balance es:\t%f\n",balance);

	
	//verificamos si el balance obtenido es igual al balance inicial
	if(balance == (valorInicial*cantidadCuentas)){
		printf("*************************PASO***************************\n");

	}else{

		printf("***********************NO PASO***************************\n");
	}



	return 0;
}

