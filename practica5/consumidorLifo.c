#include <stdio.h>
#include <stdlib.h>
#include <mqueue.h>
#include <unistd.h>

#define MAX_BUFFER 6 //tamaño del buffer
#define DATOS_A_PRODUCIR 100 //numero de datos a producir/consumir

//cola de entrada de mensajes para el productor
mqd_t almacen1;
//cola de entrada de mensajes para el consumidor
mqd_t almacen2;


int main(){
    struct mq_attr attr; //atributos de la cola
    attr.mq_maxmsg=MAX_BUFFER; //numero maximo de mensajes
    attr.mq_msgsize=sizeof(char); //tamaño maximo de mensaje
    mq_unlink("/ALMACEN1");
    mq_unlink("/ALMACEN2");
    //Borrado de los buffers de entrada por si existían de una ejecución previa
    /*mq_unlink("/ALMACEN1");
    mq_unlink("/ALMACEN2");*/

    //Apertura de los buffers
    almacen1=mq_open("/ALMACEN1", O_CREAT|O_RDONLY, 0777, &attr);
    almacen2=mq_open("/ALMACEN2", O_CREAT|O_WRONLY, 0777, &attr);

    if((almacen1==-1) || (almacen2==-1)){
        perror("mq_open");
        exit(EXIT_FAILURE);
    }
    //funcion de producir

    char elemento='_';
    char entrada='A';
    double dormir=0.0;    

    for(int i=0; i<MAX_BUFFER; i++){
        mq_send(almacen2, &elemento, sizeof(elemento), 0);
        printf("Consumidor\n\tMensaje vacio enviado: %c\n", elemento);
    }

    for(int i=0; i<DATOS_A_PRODUCIR-MAX_BUFFER; i++){
    //en este caso, el bucle va desde 0 hasta datosAProducir-maxBuffer
    //la razon es porque el consumidor y el productor tienen que
    // enviar y recibir el mismo número de mensajes
    //el consumidor, ya mandó un total de maxBuffer mensajes al productor
    //por tanto, ha de enviar menos mensajes en el siguiente bucle for
        mq_receive(almacen1, &entrada, attr.mq_msgsize, 0);
        printf("Consumidor\n\tMensaje recibido: %c\n", entrada);
        //elemento=mensaje[1];
        mq_send(almacen2, &elemento, sizeof(elemento), 0);
        printf("\tElemento consumido: %c\n", entrada);
        dormir=drand48()*4.0;
        sleep(dormir);
    }
    //acaba la función de consumir
    mq_close(almacen1);
    mq_close(almacen2);

    exit(EXIT_SUCCESS);
}
