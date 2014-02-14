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


//////////////////////// creacion de cuentas /////////////////////
struct creacionCuentas{

	//identificador cuenta
	int id;
	
	
	
	//saldo actual de la cuenta
	double saldo;
};




/////////////////metodos que se realiza a cada una de las cuentas
void transferencia(){
	
	

	
	double valorCuenta;
	int auxElegido=0;
	int auxElegido2=0; 
	double valorRando=0;
	int con_transacciones=0;
	//printf("fui bien llamado\n");		
	
	int rai;
	double rad;
	int w1,w2;
	
	while(con_transacciones<cantidadTransacciones){
		
		rai = rand();
		auxElegido = rai%(cantidadCuentas);
		
		//printf("cuenta seleccionada1 %d\n",auxElegido);
		rai = rand();
		auxElegido2 = rai%(cantidadCuentas);

		//printf("cuenta seleccionada2 %d\n",auxElegido2);
		//srand48(time(NULL));
		rad= drand48();

		if(auxElegido!=auxElegido2){
			w1=pthread_mutex_trylock(&sum_mutex[auxElegido]);	
			//printf("puedo obtener bloqueo para la primer cuenta %d\n",w1);
			w2=pthread_mutex_trylock(&sum_mutex[auxElegido2]);
			//printf("puedo obtener bloqueo para la segunda cuenta %d\n",w2);
			if((w1 == 0) & (w2 == 0)){
	
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
	
	//almacenar  la cantidad de cuentas de las que se dispone
	pthread_t hilos_ids[cantidadHilos];

	struct creacionCuentas  cuentas[cantidadCuentas];
	pthread_mutex_t arrMutex[cantidadCuentas];
	pcuentas = cuentas;	
	sum_mutex = arrMutex;

		for(i=0; i<cantidadCuentas; ++i){
		pthread_mutex_init(&arrMutex[i], NULL);
		}


	for(i=0; i<cantidadCuentas; ++i){

		struct creacionCuentas cuenta = {i+1,valorInicial};
		cuentas[i] = cuenta;
		
	}
	srand(time(NULL)*getpid());
	srand48(time(NULL)*getpid());
	for(i=0;i<cantidadHilos;++i) pthread_create (&hilos_ids[i],NULL,(void *) transferencia,NULL);
	for(i=0;i<cantidadHilos;++i) pthread_join(hilos_ids[i],NULL);

	for(i=0; i<cantidadCuentas; ++i){

		pthread_mutex_destroy(&arrMutex[i]);

		
	}	

	double balance=0;
	double sal;
	for (i=0;i<cantidadCuentas;i++){

	sal = pcuentas[i].saldo;
	printf("la cuenta\t%d tiene un saldo de:\t%f\n",pcuentas[i].id,sal);
	balance = balance + sal;
	
	}
	
	printf("el balance es:\t%f\n",balance);

	if(balance == (valorInicial)*cantidadCuentas){
		printf("*************************PASO***************************\n");

	}else{

		printf("***********************NO PASO***************************\n");
	}



	return 0;
}

