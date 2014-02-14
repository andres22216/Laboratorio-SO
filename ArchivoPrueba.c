/**
6. Realice un programa de prueba que llame automáticamente el programa anterior pero con diferentes vectores de prueba.
Dicho programa deberá leer la salida producida con cada vector de prueba y comprobar que el resultado es correcto.
Los vectores de prueba deberán estar almacenados en un archivo.

7. Corra el programa de prueba de así:
$./programa_prueba nombre_programa archivo_vectores_prueba

8. Imprima el resultado de cada vector indicando si pasó o no pasó.

9. Los vectores de prueba deberán ser almacenados por líneas, así:

numero_hilos cantidad_tiempo_a_correr numero_cuentas valor_inicial CANTIDAD_DE_REPETICIONES_DE_ESTE_VECTOR 

El parámetro CANTIDAD_DE_REPETICIONES_DE_ESTE_VECTOR indica la cantidad de veces que se deberá repetir este vector.
**/

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <wait.h>
#include <string.h>

int main (int argc, char *argv[]){

FILE *entrada;
char *argumentos[10];
int status;
char texto[60];
char *argumentosPrueba[10];
pid_t p_hijo;

int i;
char nombreArchi [15];

//nombre del programa a llamar
argumentos[0]=argv[1];
// archivo vector de prueba
argumentos[1]=argv[2];

//printf("dsdsds %s",argumentos[0]);
//printf("dsdsds %s",argumentos[1]);

entrada = fopen(argumentos[1],"r");

if(entrada == NULL){printf("error al abrir el archivo");
}else{printf("se pudo abrir el archivo\n\n");}

sprintf(nombreArchi,"./%s",argumentos[0]);
fgets(texto,80,entrada);
while (!feof(entrada)){

char *in = strtok(texto," "); 
i=1;
while(in != NULL){
argumentosPrueba[i] = in;
in = strtok(NULL," ");
i++;
} 

int numeroRepeticiones = atoi(argumentosPrueba[i-1]);
argumentosPrueba[i-1]=NULL;

argumentosPrueba[0] = argumentos[0];
//printf("el viejo %s\n",argumentosPrueba[i]);
//printf("aquientro %d",numeroRepeticiones);


for(i=0;i<numeroRepeticiones;i++){	
	p_hijo = fork(); // funcion para llamada a hijo del proceso presente.
	if(p_hijo==0){
		//printf("soy el hijo: %d me ejecute\n\n", i+1);
		execvp(nombreArchi,argumentosPrueba); //  funcion familia exe para ejecutar un actividad distinta del proceso padre.
		exit(8);
	}else{
		//printf("soy el padre llamo al hijo: %d\n\n", i+1);
		wait(&status);
	}
}

fgets(texto,80,entrada);
}
fclose(entrada);
return 0;

}




