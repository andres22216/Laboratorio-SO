/**
6. Realice un programa de prueba que llame autom�ticamente el programa anterior pero con diferentes vectores de prueba.
Dicho programa deberá leer la salida producida con cada vector de prueba y comprobar que el resultado es correcto.
Los vectores de prueba deberán estar almacenados en un archivo.

7. Corra el programa de prueba de as�:
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
FILE *entradaresultado;
char *argumentos[10];
int status;
char texto[60];
char *argumentosPrueba[10];
pid_t p_hijo;
int i;
int contador=0;
char nombreArchi [15];
int verificador=0;
char textoaux[60];

//nombre del programa a llamar
argumentos[0]=argv[1];
// archivo vector de prueba
argumentos[1]=argv[2];

entrada = fopen(argumentos[1],"r");
entradaresultado = fopen("archivoaux.dat","r");

if(entrada == NULL){printf("error al abrir el archivo");
}else{printf("se pudo abrir el archivo\n\n");}

sprintf(nombreArchi,"./%s",argumentos[0]);


//obtenemos una linea del archivo (vector de prueba)
fgets(texto,80,entrada);

//recorremos y asignamos los parametros obtenidos de cada vector de prueba
while (!feof(entrada)){

char *in = strtok(texto," "); 
i=1;
while(in != NULL){
argumentosPrueba[i] = in;
in = strtok(NULL," ");
i++;
} 

//numero de repeticiones de cada vector de prueba
int numeroRepeticiones = atoi(argumentosPrueba[i-1]);
argumentosPrueba[i-1]=NULL;

argumentosPrueba[0] = argumentos[0];

// ciclo donde se creara el proceso para cada vector de prueba
for(i=0;i<numeroRepeticiones;i++){	
	p_hijo = fork(); // funcion para llamada a hijo del proceso presente.
	if(p_hijo==0){
		//printf("soy el hijo: %d me ejecute\n\n", i+1);
		//  funcion familia exe para ejecutar un actividad distinta del proceso padre.
		execvp(nombreArchi,argumentosPrueba); 
		exit(8);
	}else{
		//printf("soy el padre llamo al hijo: %d\n\n", i+1);
		//el padre espera
		wait(&status);
	}
}

// verifica si las transacciones pasaron o no;
char *valores[3];
for(i=0;i<numeroRepeticiones;i++){
	fgets(textoaux,80,entradaresultado);



int j=0;

char *in1 = strtok(textoaux," "); 
while(in1 != NULL){
valores[j] = in1;
in1 = strtok(NULL," ");
//printf("vect%f",atof(in1));
i++;
j++;
} 
fgets(textoaux,80,entradaresultado);

//printf("sas%lf\t%lf",atof(valores[0]),atof(valores[1]));
double diferencia;
diferencia =abs(atof(valores[0])-atof(valores[1])); 
//printf("vector%lf %lf\n",atof(valores[0]),atof(valores[1]));
if(diferencia<(0.1)){
	verificador=0;
}else{
	verificador=1;
}

}
if(verificador==0){
	printf("\n\nel vector %d con %d repeticiones\n",(contador+1),numeroRepeticiones);
	printf("*************************PASO***************************\n\n");
}else{
	printf("\n\nel vector %d con %d repeticiones\n\n",(contador+1),numeroRepeticiones);
	printf("*************************NOPASO***************************\n");
}
contador++;
//obtenemos la siguiente linea (vector de prueba)
fgets(texto,80,entrada);
}

//cerramos el archivo
fclose(entrada);
fclose(entradaresultado);
return 0;

}




