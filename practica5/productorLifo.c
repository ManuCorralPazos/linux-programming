#include <stdio.h>
#include <stdlib.h>
#include <mqueue.h>

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
    mq_unlink("/ALMACEN1");
    mq_unlink("/ALMACEN2");

    //Apertura de los buffers
    almacen1=mq_open("/ALMACEN1", O_CREAT|O_WRONLY, 0777, &attr);
    almacen2=mq_open("/ALMACEN2", O_CREAT|O_WRONLY, 0777, &attr);

    if((almacen1==-1) || (almacen2==-1)){
        perror("mq_open");
        exit(EXIT_FAILURE);
    }
    //funcion de producir

    char elemento;
    char mensaje[MAX_BUFFER];
    while(1){
        //producir elemento
        elemento='A' + (rand()%(122-65)); //genera una letra aleatoria
        mq_receive(almacen2, mensaje, attr.mq_msgsize,0);
        
    }
    //acaba la función de producir
    mq_close(almacen1);
    mq_close(almacen2);

    exit(EXIT_SUCCESS);
}
