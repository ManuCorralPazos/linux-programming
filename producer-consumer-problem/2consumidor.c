#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <pthread.h> 
#include <semaphore.h>
#include <unistd.h>
#include <fcntl.h>

#define N 10 //comentarios análogos al productor
#define mutexC "MUTEX" 
#define emptyC "EMPTY" 
#define fullC "FULL"

int main(){

    sem_t *mutex, *empty, *full;
    int intMutex=0, intEmtpy=0, intFull=0; // comentarios análogos al productor
    //en este caso, se le pasan menos argumentos a la función sem_open porque los semáforos ya están creados en el productor
    //simplemente se inicializan en este proceso, porque ya fueron creados
    if((mutex=sem_open(mutexC, 0))==SEM_FAILED)
        perror("Error al abrir mutex");
    if((full=sem_open(fullC, 0))==SEM_FAILED)
        perror("error al abrir full");
    if((empty=sem_open(emptyC, 0))==SEM_FAILED)
        perror("error al abrir empty");
    
    double dormir=0.0;
    struct stat sb;
    srand48(time(NULL));
    int archivo=-1;
    if((archivo=open("./2txt.txt", O_RDWR))==-1){ //comprobación y apertura en modo lectura y escritura
        perror("Error al abrir el fichero");
        exit(-1);
    }
    if((fstat(archivo, &sb))==-1){ //obtenemos los datos del archivo abierto
        perror("Error en fstat");
        exit(-1);
    }
    //printf("Longitud del archivo: %lu bytes\n", sb.st_size);
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

    for(int i=0; i<100; i++){
        dormir=drand48()*4.0; //se genera un numero aleatorio con decimales entre 0.0 y 1.0 (excluidos), y se multiplica por 4
        sleep(dormir);
        printf("Consumidor:\n\tBuffer antiguo: ");
        for(int i=0; i<N; i++){
            printf("%c ", texto[i]);
        }
        sem_wait(full); //en este caso, se disminuye el semáforo de la cuenta de elementos, ya que se consumirá un elemento
        sem_wait(mutex); //se baja el semáforo del mutex, indicando que se entra en la región crítica de igual modo que el productor
        sem_getvalue(full, &intFull);
        texto[intFull]='2'; //se actualiza el último valor disponible para consumir del buffer, indicando con un 2 que ha sido consumido
        sem_post(mutex); //se levanta mutex
        sem_post(empty); //se aumenta el semáforo que indica la cantidad de elementos vacíos/consumidos en el buffer
        printf("\n\tBuffer nuevo:   "); //se imprime de nuevo el buffer tras la consumición
        for(int i=0; i<N; i++){
            printf("%c ", texto[i]);
        }
        printf("\n\tCuenta actualizada: %d->%d\n", intFull+1, intFull); //se imprime el valor actualizado de los elementos que hay en el buffer
    }

    munmap(texto, sb.st_size); //análogo al productor
    close(archivo);
    sem_close(mutex);
    sem_close(empty);
    sem_close(full);

    sem_unlink(mutexC);
    sem_unlink(emptyC);
    sem_unlink(fullC);


}