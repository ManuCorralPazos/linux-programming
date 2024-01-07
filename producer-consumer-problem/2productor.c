#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <pthread.h> 
#include <semaphore.h>
#include <unistd.h>
#include <fcntl.h>

#define N 10 //se define el buffer de tamaño 10, como se indica en el enunciado de la práctica
#define mutexC "MUTEX" //definimos los nombres de los semáforos 
#define emptyC "EMPTY" 
#define fullC "FULL"


int main(){

    sem_unlink(mutexC); //está indicado en el enunciado que es buena práctica borrar los semáforos antes de volver a crearlos
    sem_unlink(emptyC); //de esta forma, nos aseguramos de que no haya semáforos en el kernel con los nombres que elegimos
    sem_unlink(fullC);

    sem_t *mutex, *empty, *full; //creación de las variables de los semáforos
    int intMutex=0, intEmtpy=0, intFull=0; //algunos de estos enteros se utilizarán con la función sem_getvalue;

    //ahora abrimos los semáforos por primera vez con sus diferentes comprobaciones de que hayan sido abiertos correctamente
    //sem_open(nombre del semáforo, opción para crear, permisos, valor incial)
    //en caso de error, la función devuelve la constante SEM_FAILED
    if((mutex=sem_open(mutexC, O_CREAT, 0700, 1))==SEM_FAILED)
        perror("Error al abrir mutex");
    if((full=sem_open(fullC, O_CREAT, 0700, 0))==SEM_FAILED)
        perror("error al abrir full");
    if((empty=sem_open(emptyC, O_CREAT, 0700, N))==SEM_FAILED);
        perror("error al abrir emtpy");

    double dormir=0.0; //variable para dormir a los procesos con un número aleatorio
    struct stat sb; //estructura para obtener los datos del archivo
    srand48(time(NULL)); //se crea la semilla para la obtención de números aleatorios
    int archivo=-1; //variable del archivo a abrir
    if((archivo=open("./2txt.txt", O_RDWR))==-1){ //comprobación y apertura en modo lectura y escritura
        perror("Error al abrir el fichero");
        exit(-1);
    }
    if((fstat(archivo, &sb))==-1){ //obtenemos los datos del archivo abierto
        perror("Error en fstat");
        exit(-1);
    }

    char *texto;
    texto=mmap(NULL, sb.st_size, PROT_WRITE, MAP_SHARED, archivo, 0); //se mapea el contenido del fichero en la variable texto

    if(msync(texto, sb.st_size, MS_SYNC)==-1){ //se sincroniza para hacer que al modificar texto se grabe en disco
        perror("Error al sincronizar");
        exit(-1);
    }
    //mmap(elige el SO, tamaño del buffer, protección de sólo lectura y escritura, copia compartida, archivo a abrir, offset);
    if(texto==MAP_FAILED){ //comprobación del mapeo hecho anteriormente
        perror("Error al abrir el mapa");
        exit(-1);
    }
    for(int i=0; i<N; i++){ //nos aseguramos de que el buffer está lleno de ceros
        texto[i]='0';
    }
    for(int i=0; i<100; i++){ //bucle del programa, sustituyendo el while(1) del ejercicio 1 por un bucle finito de 100 iteraciones
        dormir=drand48()*4.0; //se genera un numero aleatorio con decimales entre 0.0 y 1.0 (excluidos), y se multiplica por 4
        sleep(dormir); //se duerme fuera de la región crítica ese número aleatorio de segundos
        printf("Productor:\n\tBuffer antiguo: "); //se imprime el contenido del buffer antes de producir
        for(int i=0; i<N; i++){
            printf("%c ", texto[i]);
        }
        sem_wait(empty); //se baja el semáforo emtpy, indicando que hay una posición vacía menos en el buffer
        sem_wait(mutex); //se baja el semáforo mutex, indicando que vamos a entrar en la sección crítica
        // de esta forma, cualquier otro proceso con semáforos estará esperando hasta que volvamos a subir el semáforo mutex, obteniendo exclusin mutua
        sem_getvalue(full, &intFull); //se lee el valor del semáfoor full
    
        texto[intFull]='1'; //se indica que el elemento correspondiente a la primera posición vacía/consumida se produce

        sem_post(mutex); //se levanta el semáforo mutex, indicando que se sale de la sección crítica
        sem_post(full); //se aumenta el semáforo full, indicando que se ha creado un nuevo elemento
        printf("\n\tBuffer nuevo:   "); //se imprime el buffer nuevo después de la producción de un elemento
        for(int i=0; i<N; i++){
            printf("%c ", texto[i]);
        }
        printf("\n\tCuenta actualizada: %d->%d\n", intFull, intFull+1); //se imprime el valor actualizado de la cuenta de elementos producidos que hay en el buffer
    }

    munmap(texto, sb.st_size); //se libera el mapeo de memoria
    close(archivo); //se cierra el archívo y los semáforos
    sem_close(mutex);
    sem_close(empty);
    sem_close(full);

    sem_unlink(mutexC); //se eliminan los semáforos
    sem_unlink(emptyC);
    sem_unlink(fullC);

}