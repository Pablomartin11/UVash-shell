/* UVaShell - Practica 2 - Estructuras de Sistemas Operativos - Pablo Martin de Benito - INDat */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdbool.h>
#include <fcntl.h>



/* Variables globales */
size_t MAX_COMMAND_LENGTH = 200;
size_t MAX_ARGS = 200;
char error_message[30] = "An error has occurred\n";
bool ejecutado = false; // Bandera para saber si se ha ejecutado algun comando, o si se debe ejecutar el error

bool redireccion = false; // Bandera para saber si hay que redireccionar la salida a un archivo.
bool coger_salida = false; // Bandera para saber si la siguiente palabra debe ser el archivo de salida.
bool salida_cogida = false; // Bandera para saber si se ha cogido el archivo de salida.

bool entrada_procesada = false; // Bandera para saber si se ha procesado la entrada y se ha guardado en el vector.
int cuenta_argumentos = 0; // Contador de argumentos en el vector
char *file[10]; // Archivo de salida.
int cuenta_comandoH = 0; // Contador de argumentos que va a ejecutar el proceso hijo
char *comandoH[20]; // Almacenar los argumentos que va a ejecutar el proceso hijo.
bool redireccionado = false;
int num_hijos=0;



/*
 * Funcion de error en cualquier caso, imprime el mensaje de error y retorna a donde se la ha llamado;
 */
int ejecutarError(){
	fprintf(stderr,"%s",error_message);
	return(0);
}



/*
 * Funcion que procesa una linea de entrada, ya sea por fichero o por el modo interativo.
 * Se le debe pasar el puntero que apunta a la entrada, y el vector donde se almacenaran los argumentos.
 * Separa la entrada por palabras y las va almacenando en el vector.
 */
int procesarEntrada(char *entrada, char *linea[], char *file[]){
	char *palabra;
	int indice_salidas = 0;
	int j = 0;

	while ( (palabra = strsep(&entrada," \t\n")) != NULL ){
		if (strlen(palabra) > 0){ // Comprobamos que no sea un caracter vacio.
			entrada_procesada = true;

			if (strcmp(palabra,"&") == 0){ // Si encontramos un '&' reseteamos las banderas de redireccion por si hay mas.
				coger_salida = false;
				salida_cogida= false;
			}

			if(strcmp(palabra,">") == 0 && !salida_cogida && cuenta_argumentos > 0){ // Si hay redireccion.
				redireccion = true;
				coger_salida = true; // En la siguiente iteracion
			} else if (coger_salida){
				file[indice_salidas] = palabra; // Guardamos en el vector de archivos.
				indice_salidas++;
				coger_salida = false;
				salida_cogida = true;
			} else{

				if (salida_cogida){ // Si una vez cogida la salida, hay mas argumentos, error.
					ejecutarError();
					return(1);
				}
				linea[j] = palabra;
				j++;
				cuenta_argumentos++;
			}
		}
	}

	if(coger_salida && !salida_cogida){ // Si hay que coger salida y no se ha cogido, error.
			ejecutarError();
			exit(0);
		}
	linea[j] = NULL;

	return(0);
}


/*
 * Funcion que se encarga de ejecutar el comando "cd".
 * Se le debe pasar el vector con los argumentos.
 * Comprueba si el numero de argumentos es exactamente dos (cd  y el dir), si no ejecutarError.
 * Comprueba que el dir no este vacio, si no ejecutarError.
 * Ejecuta el comando y ejecutarError si falla el proceso.
 */
int ejecutarCD(char *comandoH[]){

	if (cuenta_comandoH != 2){ // 'cd' solo puede tener dos argumentos
		ejecutarError();
		return(0);
	}
	if(strlen(comandoH[1]) < 1){ // Si no se especifica el directorio.
		ejecutarError();
		return(0);
	}
	if (chdir(comandoH[1]) == -1){ // Fallo de ejecucion.
		ejecutarError();
		return(0);
	}
	return(0);
}


/*
 * Funcion que se encarga de ejecutar el comando una vez ha sido procesada la entrada.
 * Se le debe pasar el vector con la linea de entrada y el vector con los archivos de salida.
 */
int ejecutarComando(char *linea[], char* file[]){
	int indice_salidas = 0;
	num_hijos=0;
	pid_t pid;

	for(int i = 0; i < cuenta_argumentos; i++){
		if (strcmp(linea[i],"&") != 0){ // Obtenemos el numero de argumentos que debe ejecutar el proceso hijo.
			cuenta_comandoH++;
		}

		if ((strcmp(linea[i],"&") == 0) || i == cuenta_argumentos-1){ // Ejecutamos si encontramos el '&' o si no hay mas argumentos
			if (strcmp(linea[i],"&") == 0 && i == cuenta_argumentos-1 && i == 0){ // Si hay un '&' y no hay mas argumentos, error
				ejecutarError();
				return(0);
			}

			char *comandoH[20]; // Vector con los argumentos que ejecutara el hijo.
			int z = 0;

			// Para obtener los argumentos correctamente debemos cubrir los posibles casos al vector principal
			int k = i-cuenta_comandoH;
			if (i == cuenta_argumentos-1 && (strcmp(linea[i],"&") != 0)){
				k = i-cuenta_comandoH+1;
			}

			// Paso del vector principal de argumentos, al vector que utilizara el proceso hijo.
			while (z < cuenta_comandoH){
				comandoH[z] = linea[k];
				z++;
				k++;
			}
			comandoH[z] = NULL;


			if(strcmp(comandoH[0],"exit") == 0){ // Salida de programa
				if (cuenta_comandoH != 1){
					ejecutarError();
					return(0);
				}
				exit(0);
			}
			if (strcmp(comandoH[0],"cd") == 0){ // Funcion cd
				ejecutado = true;
				ejecutarCD(comandoH);
				return(0);
			}
			if (redireccion && strlen(file[indice_salidas]) == 0){ // Error si no hay archivo de redireccionamiento.
				ejecutarError();
				return(0);
			} else if (redireccion && file[indice_salidas] != NULL){ // Si hay redireccion activamos una bandera.
				redireccionado = true;
			}

			num_hijos++;
			pid = fork(); // Creamos un proceso hijo para ejecutar el comando

			if (pid < 0){ // Fallo al crear el proceso hijo
				ejecutarError();
				return(0);
			} else if (pid == 0){ // Codigo del proceso hijo

				if (redireccion){
					int fd = open(file[indice_salidas], O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
					dup2(fd,STDOUT_FILENO);
					close(fd);
				}

				if (execvp(comandoH[0],comandoH) < 0){
					ejecutarError();
					return(0);
				}

			} else if (pid >0){ // Codigo del proceso padre
				ejecutado = true;
				cuenta_comandoH = 0;
				if (redireccionado){
					indice_salidas++;
					redireccionado = false;
				}
			}
		}
	}

	for(int c=0;c<num_hijos;c++){ // El proceso padre espera a todos los hijos.
		wait(NULL);
	}

	redireccion = false;
	return(0);
}


/*
 * Modo ejecutar comandos por un archivo externo; se le debe pasar el puntero del fichero del que se va a leer
 * y el puntero donde se guardara la entrada de comandos.
 * Mientras la linea que lea del archivo que se le pase sea mayor o igual que uno la separa segun los delimitadores
 * y comprobara si el comando es "exit" o "cd" y ejectara el correspondiente.
 */
int modoArchivo(FILE *fp, char *entrada){
	int size;
	while ( (size = getline(&entrada,&MAX_COMMAND_LENGTH,fp)) != -1){
		// Reseteo de variables
		ejecutado = false;
		char *linea[MAX_ARGS];
		redireccion = false;
		coger_salida = false;
		salida_cogida = false;
		entrada_procesada = false;
		cuenta_argumentos = 0;
		cuenta_comandoH = 0;
		char *file[10]; // Archivo de salida.
		redireccionado = false;


		procesarEntrada(entrada,linea,file);
		if (entrada_procesada){ // Si se ha procesado bien la entrada, se ejecuta el comando
			ejecutarComando(linea,file);
		}
	}
	if (!ejecutado){ // Si no se ha ejecutado ningun comando, salta el error.
		ejecutarError();
		exit(0);
	}

	fclose(fp);
	exit(0);
}



/********************************************************************************************************
 * MAIN
 *******************************************************************************************************/
/**
 * El programa comienza comprobando si se debe ejecutar el modo Batch o Interactivo, los dos funcionan igual
 * aunque recogen la entrada de manera diferente.
 */
int main(int argc, char *argv[]){
    char *entrada = (char *)malloc(MAX_COMMAND_LENGTH * sizeof(char));

	// Modo Batch
	if (argc >= 2){
		if (argc != 2){
			ejecutarError();
			exit(1);
		}
		FILE *fp = fopen(argv[1],"r");
		if (fp == NULL){
			ejecutarError();
			exit(1);
		} else{
			modoArchivo(fp,entrada);
		}
	}

	// Modo Interactivo
    while(1){
		char *linea[MAX_ARGS];
		ejecutado = false;
		redireccion = false;
		coger_salida = false;
		salida_cogida = false;
		entrada_procesada = false;
		cuenta_argumentos = 0;
		cuenta_comandoH = 0;
		char *file[10]; // Archivo de salida.
		redireccionado = false;


		printf("UVash> "); // Prompt

		getline(&entrada,&MAX_COMMAND_LENGTH,stdin);

		procesarEntrada(entrada, linea, file); // Modificamos la entrada para que sea mas facil
		if (entrada_procesada){
			ejecutarComando(linea,file); // Ejecutamos el comando
		}
		if (!ejecutado){
		ejecutarError();
		}
	}

	exit(0);
}



