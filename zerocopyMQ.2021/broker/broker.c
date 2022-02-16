#include <sys/socket.h>
#include <sys/stat.h>
#include <time.h>
#include <sys/times.h>
#include <netinet/in.h>
#include <stdio.h>
#include <unistd.h>
#include <limits.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/uio.h>
#include "diccionario.h"
#include "cola.h"
#include <stdbool.h>// Booleanos
#include "comun.h"
#include "comun.c"

/** auxiliares declaradas **/
char *nameCola(int socket, char *nCola, int tam);//función que devuelve el nombre de la cola, al usar recv(), previamente se ha reservado espacio en memoria para guardar el nombre 
int tamanioCola(int socket, int *tam);// función que realiza un read, para guardarse el tamanio de la cola
int msjColaVacia(int s_conec);//funcion para el envío de mensaje de cola vacía(se usa en get)
int msjNoexisteCola(int s_conec);//funcion para el envío del mesaje cuando la cola no exista(se usa en get)
int msjyTam(int s_conec, uint32_t tamMsg, void *buffMsg);//envio del mensaje: enviando el tam y el mensaje
void imprime_cola(char *c);//función de apoyo, para imprimir la cola
void eliminaCola(void *nColaa, void *colaa);//función que elimna la cola usando destroy y libero el espacio de memoria del nombre de la cola
struct cola *colaInDicc(struct diccionario *d, char *name_cola);//función que sirve para comprobar que el nombre de la cola existe en el diccionario

int main(int argc, char *argv[]) {
    /* Variables que se declaran en el código de apoyo */
	int s, fd, s_conec;
    unsigned int tam_dir;
    int opcion=1;
    struct sockaddr_in dir, dir_cliente;
    int leeDatos;
    char *tamChar;
    char linea[8];

    /* Declaramos cola y diccionario de colas*/
    struct cola *cola;//cola
    struct diccionario *dcola;//diccionario
    int tamanCola;//variable que se usará para guardar los bytes leidos
    char *nCola;//variable con la que se reservará espacio en memoria para a continuación guardarse el nombre de la cola

    /* Declaramos cola y dicconario de colas de la segunda fase ) */
    //struct diccionario *dcolaBloc;// no me dio tiempo a llegar a usarlas :(
    //struct cola *colaBloqueante;// no me dio tiempo a llegar a usarlas

    /* manejador de errores(son simples variables para guardar los errores al llamar a funciones) */
    int error;
    int error2;// para get al usar 2 veces pop y comprobar si la cola está vacía

    /* iov para el uso de writev */
	struct iovec iovm[2];

    /* tam mensaje */
    uint32_t tamMsg;//rango 0- 2³²-1 **importante

    /* buffer para guardar el mensaje, del tamaño de uint32_t */
    void *buffMsg; 
    
    if (argc!=2) {
		fprintf(stderr, "Uso: %s puerto\n", argv[0]);
		return 1;
	}

    /****************************** Crea socket ******************************/
	if ((s=socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
		perror("error creando socket");
		return 1;
	}
	/* Para reutilizar puerto inmediatamente */
    if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &opcion, sizeof(opcion))<0){
        perror("error en setsockopt");
        return 1;
    }
	dir.sin_addr.s_addr=INADDR_ANY;
	dir.sin_port=htons(atoi(argv[1]));
	dir.sin_family=PF_INET;


    /****************************** bind ******************************/
	if (bind(s, (struct sockaddr *)&dir, sizeof(dir)) < 0) {
		perror("error en bind");
		close(s);
		return 1;
	}

    /****************************** listen ******************************/
	if (listen(s, 5) < 0) {
		perror("error en listen");
		close(s);
		return 1;
	}

    /*********** inicia estructura de datos de las colas y diccionario*****************/
    dcola= dic_create();//siguiendo la guía
    
    /*****************************  Estructura de colas de segunda fase y cola***************/
    //dcolaBloc= dic_create();// no se llega a usaR:(  sin tiempo para la segunda fase
    

	while (1) {
        
        tam_dir=sizeof(dir_cliente);

        /********************************* accept ********************************************/
        if ((s_conec=accept(s, (struct sockaddr *)&dir_cliente, &tam_dir))<0){
            perror("error en accept");
            close(s); 
            return 1;// ** si se usa -1 provoca una caída del broker 
        }

        /******************************** Recibe petición **********************************/

        if((fd= read(s_conec, linea, sizeof(tamChar)))<0){//para leer el tipo de operación
            perror("error al recibir petición");
            return 1;// evito poner -1 por caída del broker 
        }

/////////////////////////////////////////hasta aquí es código proporcionado por código de apoyo////////////////////////////////
        
        /********************************************* Determina el tipo de operación: es decir tendremos que leer el fichero ********************************************************** /
         * Tipo createMQ
         * Tipo destroyMQ
         * Tipo put
         * tipo get 
         **/
        // printf("Esta es la operacion que ha seleccionado %c\n", linea[0]);

        switch(linea[0]){
            //***para mi: Recordar longitud 2¹6 incluyendo el nulo de cola***//

            /* CreateMQ(const char *cola):
             añade una nueva entrada a la estructura de datos "cola" */
            case 'C':
                /** usamos reacv, y seguimos cumplimiento requisitos de zerocpy como en la especificacion de la guia */    
                if ((tamanCola=tamanioCola(s_conec, &tamanCola))<0)
                {
                    //Si hay error cerramos conexiones del socket y del accept()
                    close(s);
                    close(s_conec);
                    return 1;
                }
                
                nCola= malloc(tamanCola);//reservamos espacio para guardar el nombre de la cola usando nuevamente revc

                /*obtenemos el nombre de la cola usando recv */
                if ((nCola=nameCola(s_conec, nCola, tamanCola))== '\0')
                {
                    //Si hay error cerramos conexiones del socket y del accept()
                    close(s);
                    close(s_conec);
                    return 1;
                }
        
                printf("He creado la cola %s de tamano %d \n", nCola, tamanCola - 1);
                 
                cola= cola_create();

                /** --> si no está se añade la cola al dicc*/
                if(dic_put(dcola, nCola, cola)<0){
                    //envíamos mensaje de error con write
                    iovm[0].iov_base= "httpError"; iovm[0].iov_len= strlen("httpError")+1;//+1 ya que hay que incluir el caracter nulo
					writev(s_conec, iovm,1);
                    continue;//he quitado el break ya que me daba error en las pruebas y he usado esto ya que me comentaste que no debía salirse del bucle sino continuar.
                }//Error en dic_put

                //Enviamos el mensaje de protocolo correcto(nose si es necesario que sea un String o un entero el mensaje de protocolo)
				iovm[0].iov_base= "httpCorrecto"; iovm[0].iov_len= strlen("httpCorrecto")+1;
				writev(s_conec, iovm,1);
                // dic_visit(dcola, imprime_cola);//pequeña comprobación para ver la cola como en demo_cola.c
                break;
                /** CreateMq**/

            /* DestroyMQ(const char *cola){*/
            //eliminar entrada de la estructura de datos de las colas, borrando todos los mensajes presentes en la misma.
            case 'D':
                /** mismo procedimiento **/
                /** usamos reacv, y seguimos cumplimiento requisitos de zerocpy **/
                if ((tamanCola=tamanioCola(s_conec, &tamanCola))<0)
                {
                    //Si hay error cerramos conexiones del socket y del accept()
                    close(s);
                    close(s_conec);
                    return 1;
                }

                nCola= malloc(tamanCola);// reserva espacio en memoria

                /*obtenemos el nombre de la cola usando recv */
                if ((nCola=nameCola(s_conec, nCola, tamanCola))== '\0')// si obtiene caracter nulo cerramos conexiones
                {
                    //Si hay error cerramos conexiones del socket y del accept()
                    close(s);
                    close(s_conec);
                    return 1;
                }
                
                /* Eliminamos la cola(llamando a elimnaCola) y después de las entradas del diccionario*/
                if(dic_remove_entry(dcola, nCola, eliminaCola) < 0) {
                    iovm[0].iov_base= "httpError"; iovm[0].iov_len= strlen("httpError")+1;//+1 por el nulo sino da error
					writev(s_conec, iovm,1);// y se envía esta información
                    continue;
                }

                //exito al borrar entrada de la cola
                iovm[0].iov_base= "httpCorrecto"; iovm[0].iov_len= strlen("httpCorrecto")+1;
				writev(s_conec, iovm,1);// y se envía esta información
                break;
            /** DestroyMQ **/

            /*put (const char *cola, const void *mensaje, uint32_t tam)*/
            //guardar el mensaje en la cola correspondiente.
            case 'P':
                /** mismo procedimiento **/
                /** usamos reacv, y seguimos cumplimiento requisitos de zerocpy **/
                if ((tamanCola=tamanioCola(s_conec, &tamanCola))<0)//la funcion devolvera los bytes leidos
                {
                    //Si hay error cerramos conexiones del socket y del accept()
                    close(s);
                    close(s_conec);
                    return 1;
                }
                
                nCola= malloc(tamanCola);// obtenemos nombre de cola usando el tamanio de cola
                /*obtenemos el nombre de la cola usando recv */
                if ((nCola=nameCola(s_conec, nCola, tamanCola))== '\0')
                {
                    //Si hay error cerramos conexiones del socket y del accept()
                    close(s);
                    close(s_conec);
                    return 1;
                }
                

                if((leeDatos= read(s_conec, &tamMsg, sizeof(uint32_t))) <0){//devolvera el nºB leidos
                    perror("error en read");
					iovm[0].iov_base= "httpError"; iovm[0].iov_len= strlen("httpError")+1;//+1 por el nulo sino da error
					writev(s_conec, iovm,1);
					close(s);
					close(s_conec);
					return 1;
                }

                // printf("He seleccionado la cola %s \n", nCola);			
				// printf("Tamaño del mensaje a introducir %d\n", tamMsg);
                cola= colaInDicc(dcola, nCola);
                if(cola==NULL)
                {
                    iovm[0].iov_base= "httpError"; iovm[0].iov_len= strlen("httpError")+1;//+1 por el nulo sino da error
					writev(s_conec, iovm,1);
                    free(nCola);
                    continue;
                }

                //obtendremos el tamaño del mnsaje 
                buffMsg= malloc(tamMsg);// obtenemos el mensaje gracias al tamanio del mensaje
                if(recv(s_conec, buffMsg, tamMsg, MSG_WAITALL)<0){
                    perror("error en read");
					iovm[0].iov_base= "httpError"; iovm[0].iov_len= strlen("httpError")+1;//+1 por el nulo sino da error
					writev(s_conec, iovm,1);
					close(s);
					close(s_conec);
					return 1;
                }

                //sola queda guarda el mensaje en la cola
                cola_push_back(cola, tamMsg);// se envía primero el tamaño de cola
                cola_push_back(cola, buffMsg);// se envía después el mensaje de la cola

                
                free(nCola);//liberamos el tamaño de la cola como se indica en la guía
                iovm[0].iov_base= "httpCorrecto"; iovm[0].iov_len= strlen("httpCorrecto")+1;
				writev(s_conec, iovm,1);//envío mensaje de correcto
                break;
            /** PUT **/


            /* get(const char *cola, void **mensaje, uint32_t *tam, bool blocking):
             * obtendrá el primer mensaje en orden de llegada de la cola correspondiente. 
             * En caso de no haber ninguno, se responde indicando esta circunstancia.
             * 
             *  free cuando temine de procesar el mensaje
             * -1 si intenta leer de cola la cual no existe
             *  último parámetro indica si es bloqueante,(No se usa aun)
             * mensaje nulo o tam nulo --> se seguirá la lectura pero sin asignar valor al parámetro
             */
            case 'G'://
             /* volvemos a realiza los mismo pasos */ 
                /** mismo procedimiento **/
                /** usamos reacv, y seguimos cumplimiento requisitos de zerocpy **/
                
                if ((tamanCola=tamanioCola(s_conec, &tamanCola))<0)
                {
                    //Si hay error cerramos conexiones del socket y del accept()
                    close(s);
                    close(s_conec);
                    return 1;
                }
                nCola= malloc(tamanCola);

                /*obtenemos el nombre de la cola usando recv */
                if ((nCola=nameCola(s_conec, nCola, tamanCola))== '\0')
                {
                    //Si hay error cerramos conexiones del socket y del accept()
                    close(s);
                    close(s_conec);
                    return 1;
                }

                tamMsg=NULL;
                buffMsg=NULL;//limpio buffer de mensaje en caso de que la cola exista pero esté vacía
                //primero se comprobará si la cola existe, usando el nombre de la cola
                cola= dic_get(dcola, nCola, &error);// tendrá el valor asociado al nombre
                if (error ==0 )
                {//existe cola
                    // fprintf("cola= %p", cola);
                    // si cola existe, se extrae el primer elemento de la cola
                    //--tamaño y despues el mensaje
                    tamMsg= cola_pop_front(cola, &error);
                    buffMsg= cola_pop_front(cola, &error2);
                    if(error ==0)//con comprobar si ha dado error en "tamMsg"es suficiente
                    {
                        //enviamos el tam y el contenido del mensaje al cliente 
                        msjyTam(s_conec, tamMsg, buffMsg);

                    }// cola contiene mensaje 

                    if (error <0)
                    {
                        // printf("\n------cola vacía crack-------------\n");
                        //si cola existe pero es vacía, envío un 0, como entero 
                        msjColaVacia(s_conec);
                    }// cola no contiene mensaje(vacía)
                    


                }//cola existe 

                else if(error <0)
                {// no existe cola
                    // printf("\nCola no existe---Paso por AQUI--------\n");
                    // Si la cola no esiste directamente envío un -1
                    msjNoexisteCola(s_conec);
                }//cola no existe 
                                
                //si todo va bien 
                free(nCola);// cierro cola
                break;
            /** GET **/
                

            // /** GET bloqueante SIN TERMINAR **/
            // case 'B':  
            //     if ((tamanCola=tamanioCola(s_conec, &tamanCola))<0)
            //     {
            //         //Si hay error cerramos conexiones del socket y del accept()
            //         close(s);
            //         close(s_conec);
            //         return 1;
            //     }
            //     nCola= malloc(tamanCola);
            //     /*obtenemos el nombre de la cola usando recv */
            //     if ((nCola=nameCola(s_conec, nCola, tamanCola))== '\0')
            //     {
            //         //Si hay error cerramos conexiones del socket y del accept()
            //         close(s);
            //         close(s_conec);
            //         return 1;
            //     }
            //     tamMsg=NULL;
            //     buffMsg=NULL;
            //     cola= dic_get(dcola, nCola, &error);
            //     if (error ==0 )
            //     {
            //         tamMsg= cola_pop_front(cola, &error);
            //         buffMsg= cola_pop_front(cola, &error2);
            //         if(error ==0)
            //         {
            //             msjyTam(s_conec, tamMsg, buffMsg);
            //         }
            //         if (error <0)
            //         {
            //             msjColaVacia(s_conec);
            //         }
            //     }//cola existe 
            //     else if(error <0)
            //     {// no existe cola
            //         msjNoexisteCola(s_conec);
            //     }//cola no existe 
            //     free(nCola);
            //     break;
            // /** GET bloqueante**/


        }// SWITCH
        
        close(s_conec);

    }// WHILE
    close(s);
	return 0;
}//MAIN


/**************************************** Auxiliar *****************************************/


///////////////////////// Axiliares de uso general //////////////////////////////
int tamanioCola(int socket, int *tam)
{
    int fd;
    if( read(socket, &tam, sizeof(int)) <0){//devolvera el nºB leidos
        perror("error en read");
        fd= -1;
    }
    fd= tam;
    return fd;
}

char *nameCola(int socket, char *nCola, int tam)
{
    char *nmeCola;
    if (recv(socket, nCola,tam, MSG_WAITALL)<0)//devolver en nCola el nombre de la cola
    {
        perror("error en recv");// si hay eerror deolverá un vacio para que lo tome como error ya que tiene que devolver un char 
        nmeCola='\0';
        return nmeCola;
    }

    nmeCola= nCola;
    return nmeCola;  
}

/**
 * Solo lo uso para ver la cola
 * es decir, para comprobar que hace lo que debe, como el ejemplo de demoCola.
 * Esto se ha copiado de los fichero proporcionados por la asignatura de demo_cola.c
 **/
void imprime_cola(char *c){

    printf("clave %s\n",c);
}

/**
 * Se encarga de liberar las estructuras reservadas, este caso el nombre de cola, función parecida a la demo_cola.c
 * Se encarga de destuir la cola
 * Se usa en MQdestroy()
 **/
void eliminaCola(void *nColaa, void *colaa){//importante
    //Se libera la estructura de nombre de cola reservada con malloc() como en elimina_persona() de demo_ci.c
    free(nColaa);

    // Se aprovecha que se tiene la cola para destruirla directamente
    struct cola *colaRmv= colaa;
    cola_destroy(colaRmv, NULL);//Null porque no hay nada que liberar como en demo_cola.c
}

/*
*se comprueba si existe en el diccionario el nombre de cola
*/
struct cola *colaInDicc(struct diccionario *diccionarioC, char *nombreCola)//importante
{
	struct cola *busqueda;
	int errorColainDicc;
	busqueda = dic_get(diccionarioC, nombreCola, &errorColainDicc);
	if (errorColainDicc < 0)
	{   
		return NULL;
	}
	else
		return busqueda;
}


///////////////////////// Auxiliares para GET /////////////////////
/* envía 0 si la cola existe pero está vacía */
int msjColaVacia(int s_conec)
{

    int msj=0;
    struct iovec iovmsj[1];
    
    iovmsj[0].iov_base = &msj;
    iovmsj[0].iov_len =sizeof(msj);
    writev(s_conec, iovmsj,1); 
    return 1;

}

/* envía mensaje de -1 sino existe la cola*/
int msjNoexisteCola(int s_conec)
{

    int msj=-1;
    struct iovec iovmsj[1];
    
    iovmsj[0].iov_base = &msj;
    iovmsj[0].iov_len =sizeof(msj);
    writev(s_conec, iovmsj,1); 
    return 1;
}

/* envía al cliente el tamaño del mensaje y el contenido del mensaje */
int msjyTam(int s_conec, uint32_t tamMsg, void *buffMsg)
{
    struct iovec iov[2];
    printf("paso por aquí");
    uint32_t a = tamMsg;
	iov[0].iov_base = &a;
	iov[0].iov_len = sizeof(a);
	iov[1].iov_base = buffMsg;
	iov[1].iov_len = a;
    writev(s_conec, iov,2); 
    return 1;
}
