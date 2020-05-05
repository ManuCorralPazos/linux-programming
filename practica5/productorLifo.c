#include <stdio.h>
#include <stdlib.h>
#include <mqueue.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#define MAX_BUFFER 6 //tamaño del buffer
#define DATOS_A_PRODUCIR 100 //numero de datos a producir/consumir

//cola de entrada de mensajes para el productor
mqd_t almacen1;
//cola de entrada de mensajes para el consumidor
mqd_t almacen2;


int main(){
    struct mq_attr attr; //atributos de la cola
    attr.mq_maxmsg=MAX_BUFFER; //numero maximo de mensajes
    attr.mq_msgsize=sizeof(char); //tamaño maximo de mensajes

    //Borrado de los buffers de entrada por si existían de una ejecución previa
    //mq_unlink("/ALMACEN1");
    //mq_unlink("/ALMACEN2");

    //Apertura de los buffers
    almacen1=mq_open("/ALMACEN1", O_CREAT|O_WRONLY, 0777, &attr);
    almacen2=mq_open("/ALMACEN2", O_CREAT|O_RDONLY, 0777, &attr);

    if((almacen1==-1) || (almacen2==-1)){
        perror("mq_open");
        exit(EXIT_FAILURE);
    }
    //funcion de producir
    srand(time(NULL));
    int posicion=MAX_BUFFER-1; //indice del array en el que hay que producir
    char elemento; //el elemento que se producirá 
    char entrada; //servirá para recibir los mensajes vacíos del consumidor
    double dormir=0.0; //variable para el sleep
    unsigned int prio=0; //utilizada en la prioridad del send para hacer cola LIFOo87
    char mensaje[MAX_BUFFER];
    for(int i=0; i<DATOS_A_PRODUCIR; i++){
        //producir elemento
        elemento='A' + (rand()%(90-65)); //genera una letra aleatoria
        //printf("%c\n", elemento);
        dormir=drand48()*4.0;
        sleep(dormir);
        mq_receive(almacen2, &entrada, attr.mq_msgsize,0);
        //mensaje[posicion]=entrada;
        //posicion--;
        printf("Productor\n\tMensaje recibido: %c\n", entrada);
        //en este caso se indica la cola de la que se hace el recieve, dónde se almacena, el tamaño, y la prioridad
        //mensaje[posicion]=elemento;
        printf("\tElemento introducido: %c\n", elemento);
        mq_send(almacen1, &elemento, sizeof(elemento), prio);
        printf("\tMensaje enviado: %c\n", elemento);
        prio++;
    }
    //acaba la función de producir
    mq_close(almacen1);
    mq_close(almacen2);

    exit(EXIT_SUCCESS);
}
