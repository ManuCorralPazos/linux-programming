#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <time.h>
#include <string.h>
#include <signal.h>

#define N 9 //numero de elementos que caben en el buffer
            //en este ejercicio no se especifica que el buffer tenga que ser un número en concreto, por lo tanto lo definimos como 9, por ejemplo

void signalhandler() { //función vacía para la señal de alarma

}
int main(){
    signal (SIGUSR1, signalhandler); //se utiliza para emular el wakeup gracias a la señal enviada del otro proceso
    struct stat sb; //esturctura para los datos del archivo que abriremos
    srand48(time(NULL)); //se crea la semilla del número aleatorio que se empleará en el sleep
    int archivo=-1; //se crea la variable para la apertura del archivo
    if((archivo=open("./1txt.txt", O_RDWR))==-1){ //comprobación y apertura en modo lectura y escritura
        perror("Error al abrir el fichero");
        exit(-1);
    }
    if((fstat(archivo, &sb))==-1){ //obtenemos los datos del archivo abierto que será utilizado como almacenamiento del buffer
        perror("Error en fstat");
        exit(-1);
    }
    //printf("Longitud del archivo: %lu bytes\n", sb.st_size);
    char *texto;
    texto=mmap(NULL, sb.st_size, PROT_WRITE, MAP_SHARED, archivo, 0); //se mapea en memoria y se pone el contenido en el string texto
        //mmap(elige el SO, tamaño del buffer, protección de sólo lectura y escritura, copia compartida, archivo a abrir, offset);


    if(msync(texto, sb.st_size, MS_SYNC)==-1){ //se sincroniza para asegurarnos de que el archivo es grabado cuando se cambia texto
        perror("Error al sincronizar");
        exit(-1);
    }
    if(texto==MAP_FAILED){ //comprobación del mapeo hecho anteriomente
        perror("Error al abrir el mapa");
        exit(-1);
    }
    for(int i=0; i<N; i++){ //nos aseguramos de que el archivo esté lleno de 0, lo que significa que aún no se ha producido ni consumido nada
        texto[i]='0';
    }
    int count=0; //variable que contará la cantidad de elementos que hay en el buffer de objetos a producir/consumir
    double dormir=0.0; //variable que se obtendrá futuramente de forma aleatoria para el sleep de los procesos
    pid_t productor=getpid(), consumidor=0; 
    printf("My pid: %d\n", productor);
    printf("Introduce pid del consumidor\n"); //se obtiene el pid del consumidor para poder despertarlo cuando sea necesario
    scanf("%d", &consumidor);

    while(1){ //se entra en el bucle del programa, se debe comentar esta línea en caso de querer un bucle finito
    //for(int i=0; i<3; i++){  //se debe comentar esta línea en caso de querer un bucle finito
        dormir=drand48()*2.0; //se genera un numero aleatorio con decimales entre 0.0 y 1.0 (excluidos), y se multiplica por 2
        count=(texto[0])-'0'; //se mira cuánto vale la variable cuenta al principio de la iteracion
        if(count==N) sleep(400); //en caso de que cuenta sea N, o sea, que el buffer esté lleno, se duerme y se espera a ser despertado por el consumidor
        count=(texto[0])-'0'; //se vuelve a mirar cuando vale la variable, puesto que ha sido actualizada por el consumidor 
        printf("Productor:\n\tBuffer antiguo: ");
        for(int i=1; i<N; i++){ //se imprime cuánto vale el buffer cuando el productor lo lee
            printf("%c ", texto[i]);
        }
        //insertarObjeto(item);
        count=count+1; //se aumenta la cuenta en uno, puesto que se va a insertar un elemento
        texto[count]='1'; //se inserta el elemento en su sitio correspndiente, con un 1 indicando que el objeto está producido
            //el sitio en el que hay que insertar el elemento, lo da la variable count, que es la cantidad de elementos del buffer
        texto[0]=count + '0'; //se actualiza la variable cuenta en el archivo de texto
        printf("\n\tBuffer nuevo:   "); //se imprime por pantalla el resultado de la producción del elemento
        for(int i=1; i<N; i++){
            printf("%c ", texto[i]);
        }
        printf("\n\tCuenta actualizada: %d->%d\n", count-1, count); //se imprime la cuenta actualizada
        sleep(dormir); //se duerme el valor aleatorio fijado anteriormente
        if(count==1) kill(consumidor, SIGUSR1); //si la cuenta vale uno, se envía una señal al consumidor para que despierte
    }
    //NOTA como es un lazo infinito, nunca se va a salir de él salvo que se aborte el programa
    // por tanto, no tiene sentido invocar aquí la función munmap porque nunca se ejecutará
}