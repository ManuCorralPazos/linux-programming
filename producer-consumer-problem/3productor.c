#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <pthread.h> 
#include <semaphore.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h> 

#define N 10
#define mutexC "MUTEX"
#define emtpyC "EMTPY"
#define fullC "FULL"


int main(int argc, char *argv[]){

    int productores=0;
    if(argc==2){ //se guardan en productores la cantidad de productores a crear
        productores=atoi(argv[1]);
    }
    else{
        printf("Número de argumentos no válido. Prueba ./3productor.out <numero productores>\n");
    }
    sem_unlink(mutexC);
    sem_unlink(emtpyC);
    sem_unlink(fullC);

    sem_t *mutex, *emtpy, *full;
    int intMutex=0, intEmtpy=0, intFull=0;

    mutex=sem_open(mutexC, O_CREAT, 0700, 1);
    full=sem_open(fullC, O_CREAT, 0700, 0);
    emtpy=sem_open(emtpyC, O_CREAT, 0700, N);
    
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
    for(int i=0; i<N; i++){
        texto[i]='0';
    }
    pid_t pids[productores]; //vector de pids de los hijos productores

    for (int i=0;i<productores;i++) { //para cada uno de los productores deseados,se hace un proceso hijo que llevará a cabo dicha función de productor
        if((pids[i]=fork())==0) {
            for(int j=0; j<5; j++){
                dormir=drand48()*4.0; //se genera un numero aleatorio con decimales entre 0.0 y 1.0 (excluidos), y se multiplica por 4
                sleep(dormir);
                printf("%d\tBuffer antiguo: ", getpid());
                for(int i=0; i<N; i++){
                    printf("%c ", texto[i]);
                }
                sem_wait(emtpy);
                sem_wait(mutex);
                sem_getvalue(full, &intFull);
            
                texto[intFull]='1';

                sem_post(mutex);
                sem_post(full);
                printf("\n%d\tBuffer nuevo:   ", getpid());
                for(int i=0; i<N; i++){
                    printf("%c ", texto[i]);
                }
                //sem_getvalue(full, &intFull);
                printf("\n%d\tCuenta actualizada: %d->%d\n", getpid(),intFull+1, intFull);
            }
            exit(i);
        }

    }
    int status;
    pid_t pid;
    int c=productores;
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
