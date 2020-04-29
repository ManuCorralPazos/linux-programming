#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <pthread.h> 
#include <unistd.h>
#include <fcntl.h>

#define N 10 //tamaño del buffer
pthread_mutex_t mutex; //variable mtuex para controlar la exclusión mutua
pthread_cond_t lleno, vacio;  //variables condicionales para hacer esperar/desbloquear hilos
int elementos=0; //cantidad de elementos que hay actualmente en el buffer
int pos=0; //posición en la que se encuentra el elemento a consumir
char *texto; //cadena de texto que simulará el buffer donde se produce/consume

static void * producir(void *tid){
    for(;;){
        double dormir=0;
        dormir=drand48()*4.0;
        sleep(dormir);
        //se pone al hilo a dormir durante un tiempo aleatorio entre 0 y 4
        //destacar que este sleep se hace fuera de la región crítica
        pthread_mutex_lock(&mutex);
        //se bloquea la variable mutex, indicando que se entra en la región crítica
        //de esta forma, ningún otro hilo puede entrar en ella
        printf("\n\tP%d Buffer antiguo: ", (int)tid);
        for(int i=0; i<N; i++){
            printf("%c ", texto[i]);
        }    
        //se imprime el buffer antiguo antes de algún posible bloqueo por razón de que el buffer esté lleno
        //de esta forma, si el hilo se bloquea, lo sabremos porque imprimirá buffer antiguo y no buffer nuevo
        while(elementos==N){
            pthread_cond_wait(&lleno, &mutex);
        }
        //en caso de que el buffer esté lleno, se sale del mutex y se espera a que algún consumidor mande desbloquearse
        for(int i=N; i>0; i--){
            texto[i]=texto[i-1]; //se mueven todos los objetos una posicion a la derecha
        }
        texto[0]='1';   
        //simula producir un elemento
        elementos++;
        //se aumenta el número de elementos
        if(pos<N){
            pos=(pos+1);
        }
        //se aumenta la variable que indica la posición en la que se encuentra el elemento a consumir
        //pos=(pos+1)%N;
        if(elementos==1){
            pthread_cond_signal(&vacio);
        }
        //en caso de que solo haya un elemento, significa que el buffer estaba vacío antes de producir
        //por tanto, los consumidores que hayan intentando consumir estarán bloqueados, y por tanto se desbloquean
        printf("\n\tP%d Buffer nuevo:   ", (int)tid); //se imprime de nuevo el buffer tras la consumición
        for(int i=0; i<N; i++){
            printf("%c ", texto[i]);
        }
        pthread_mutex_unlock(&mutex);
        //se indica que salimos de la región crítica después de hacer la operación de producir
    }
}

static void * consumir(void *tid){
    for(;;){
        double dormir=0;
        dormir=drand48()*4.0;
        sleep(dormir);
        //análogo a producir
        pthread_mutex_lock(&mutex);
        printf("\n\tC%d Buffer antiguo: ", (int) tid);
        for(int i=0; i<N; i++){
            printf("%c ", texto[i]);
        } 
        //análogo a producir
        while(elementos==0){
            pthread_cond_wait(&vacio, &mutex);
        }
        //en este caso, el hilo se pone en espera si el buffer está vacío, ya que no hay  nada que consumir
        texto[pos-1]='2';
        //esta es la posición en la que está el primer objeto producido en orden temporal
        //hay que consumir este para conseguir el comportamiento FIFO que buscamos
        if(pos>0){
            pos=pos-1;
        }
        //se disminuye la variable que indica la posición del siguiente elemento a consumir
        //pos=(pos-1)%N;
        elementos--;
        //se disminuye la cantidad de elementos en el buffer, puesto que se consumió uno
        if(elementos==(N-1)){
            pthread_cond_signal(&lleno);
        }
        //en caso de que el buffer tenga N-1 elementos, significará que estaba lleno antes de consumir
        //por tanto, los productores que hayan intentado producir, estarán esperando, y por tanto se desbloquean
        printf("\n\tC%d Buffer nuevo:   ", (int) tid); //se imprime de nuevo el buffer tras la consumición
        for(int i=0; i<N; i++){
            printf("%c ", texto[i]);
        }
        //printf("\thay %d elementos", elementos);
        pthread_mutex_unlock(&mutex);
        //análogo a producir
    }
}

int main (int argc, char** argv){
    //antes de nada, creamos el mutex y las variables de condición con sus respectivas comprobaciones
    //en caso de error, la función init de los mutex devuelve un valor distinto de cero
    if(pthread_mutex_init(&mutex,NULL)!=0){
        return -1;
    }
    if(pthread_cond_init(&lleno,NULL)!=0){
        return -1;
    }
    if(pthread_cond_init(&vacio,NULL)!=0){
        return -1;
    }
    srand(time(NULL)); //se pone la semilla para la generación de consumidores y productores
    int ncons=0;
    int nprod=0;
    nprod=rand()%5+1;
    //se elige un número aleatorio entre el 1 y el 5 (incluidos) para la cantidad de productores
    ncons=rand()%4+1; 
    //se elige un número aleatorio entre el 1 y el 4 (incluidos) para la cantidad de consumidores
    printf("Número de productores: %d\nNúmero de consumidores: %d\n", nprod, ncons);
    //abrimos el archivo
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
    //printf("Longitud del archivo: %lu bytes\n", sb.st_size);
    //char *texto;
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
    for(int i=0; i<N; i++){ //nos aseguramos de que el buffer está lleno de ceros
        texto[i]='0';
    }
    
    pthread_t productores[nprod]; //vector de hilos de productores
    int comprobacion=0; //esta variable se utiliza para comprobar que se crean los hilos correctamente
    for(int i=0; i<nprod; i++){ //creación de nprod productores
        if((comprobacion=pthread_create(&productores[i], NULL, producir, (void *)i))!=0){
           //se crean los hilos de producción y se le asigna la función de producir
            return -1; //en caso de que la función falle, se aborta el programa
        }
    }

    pthread_t consumidores[ncons];
    for(int i=0; i<ncons; i++){ //creación de nprod productores
        if((comprobacion=pthread_create(&consumidores[i], NULL, consumir, (void *)i))!=0){
           //se crean los hilos de producción y se le asigna la función de producir
            return -1; //en caso de que la función falle, se aborta el programa
        }
    }
    //se utiliza la función pthread_join para asegurarse de que el proceso no acaba
    //antes de que los hilos acaben. 
    for(int i=0; i<nprod; i++){
        if(pthread_join(productores[i], NULL)!=0){
            printf("Error en pthread_join\n");
            exit(0);
	    }
    }
    for(int i=0; i<ncons; i++){
        if(pthread_join(consumidores[i], NULL)!=0){
            printf("Error en pthread_join\n");
            exit(0);
	    }
    }

    //al ser un programa en el que los hilos tienen bucles infinitos, esto nunca acaba

}