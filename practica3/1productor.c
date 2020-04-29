#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

#define N 10

int main(){
    struct stat sb;
    int archivo=-1;
    if((archivo=open("./1txt.txt", O_RDWR))==-1){ //comprobación y apertura en modo lectura y escritura
        perror("Error al abrir el fichero");
        exit(-1);
    }
    if((fstat(archivo, &sb))==-1){ //obtenemos los datos del archivo abierto
        perror("Error en fstat");
        exit(-1);
    }
    printf("Longitud del archivo: %lu bytes\n", sb.st_size);
    char *texto;
    texto=mmap(NULL, sb.st_size, PROT_WRITE, MAP_SHARED, archivo, 0);

    if(msync(texto, sb.st_size, MS_SYNC)==-1){
        perror("Error al sincronizar");
        exit(-1);
    }
    //mmap(elige el SO, tamaño del buffer, protección de sólo lectura y escritura, copia compartida, archivo a abrir, offset);
    if(texto==MAP_FAILED){
        perror("Error al abrir el mapa");
        exit(-1);
    }

    int item=0, count=0;
    pid_t productor=getpid();
    printf("My pid: %d", pid);
    /*while(1){
        item=1;
        if(count==N) sleep(1);
        insertarObjeto(item);
        count=count+1;
        if(count==1) wakeup(consumidor);
    }*/

}