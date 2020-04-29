#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <pthread.h> 
#include <semaphore.h>
#include <unistd.h>
#include <fcntl.h>

#define N 10
#define mutexC "MUTEX"
#define emtpyC "EMTPY"
#define fullC "FULL"


int main(int argc, char *argv[]){

    int consumidores=0;
    if(argc==2){
        consumidores=atoi(argv[1]);
    }
    else{
        printf("Número de argumentos no válido. Prueba ./3consumidor.out <numero consumidores>\n");
    }
    
    sem_t *mutex, *emtpy, *full;
    int intMutex=0, intEmtpy=0, intFull=0;
    if((mutex=sem_open(mutexC, 0))==SEM_FAILED)
        perror("Error al abrir mutex");
    if((full=sem_open(fullC, 0))==SEM_FAILED)
        perror("error al abrir full");
    if((emtpy=sem_open(emtpyC, 0))==SEM_FAILED)
        perror("error al abrir emtpy");
    
    double dormir=0.0;
    struct stat sb;
    srand48(time(NULL));
    int archivo=-1;
    if((archivo=open("./3txt.txt", O_RDWR))==-1){ //comprobación y apertura en modo lectura y escritura
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

    pid_t pids[consumidores];

    for (int i=0;i<consumidores;i++) {
        if((pids[i]=fork())==0) { //comentarios análogos al productor
            for(int i=0; i<5; i++){
                dormir=drand48()*4.0; //se genera un numero aleatorio con decimales entre 0.0 y 1.0 (excluidos), y se multiplica por 4
                sleep(dormir);
                printf("%d:\n\tBuffer antiguo: ", gepid());
                for(int i=0; i<N; i++){
                    printf("%c ", texto[i]);
                }
                sem_wait(full);
                sem_wait(mutex);
                sem_getvalue(full, &intFull);
                texto[intFull]='2';
                sem_post(mutex);
                sem_post(emtpy);
                printf("\n%dtBuffer nuevo:   ", getpid());
                for(int i=0; i<N; i++){
                    printf("%c ", texto[i]);
                }
                printf("\n%d\tCuenta actualizada: %d->%d\n",getpid(), intFull, intFull-1);
            }
            exit(i);
        }

    }
    int status;
    pid_t pid;
    int c=consumidores;
    while (c > 0) {
        pid = wait(&status);
         c--;  
    }

    munmap(texto, sb.st_size);
    close(archivo);
    sem_close(mutex);
    sem_close(emtpy);
    sem_close(full);

    sem_unlink(mutexC);
    sem_unlink(emtpyC);
    sem_unlink(fullC);


}