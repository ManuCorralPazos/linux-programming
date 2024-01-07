#en este script se utiliza un archivo de texto llamado salida.data como apoyo
echo "user       pid  cpu  mem  start     time    comm"  #formateamos la salida para el terminal
echo "" > salida.data #ponemos en blanco el archivo de texto
ps -eo user,pid,%cpu,%mem,start,time,comm | grep $USERNAME >> salida.data #utilizamos el comando ps para saber los procesos activos
                                                                          #las opciones -eo nos permiten elegir qué campos del ps queremos
                                                                          #"pipeamos" esta salida a un filtro en función de nuestro usuario
                                                                          #guardamos dicha salida filtrada al fichero salida.data
awk '{print}' salida.data  #printeamos el fichero salida.data utulizando el comando awk
echo "Número total de procesos: "
awk 'END { print NR }' salida.data #contamos la cantidad de líneas que tiene el archivo
                                   #NR es la cuenta del número de inputs, en este caso, el número de líneas del archivo
echo "%CPU total usada: "
awk -F" " '{x+=$3}END{print x}' salida.data #utilizamos el comando awk para sumar los datos de salida.data por columnas
                                            #en este caso, indicamos que el separados son espacios con -F" "
                                            #vamos almacenando los valores de la columna 3 en x, %cpu, y luego se imprime x una vez se acaba

echo "%Memoria total usada: "
awk -F" " '{x+=$4}END{print x}' salida.data #igual que antes, pero para la columna 4, que es %mem

echo "Tiempo total de CPU acumulado: " #para el tiempo, utilizaremos la función split, puesto que sale formatado de la forma h:m:s
awk '{split($6, a, ":");              
        s += a[3]; 
        min += a[2] + int(s / 60);
        h += a[1] + int(min / 60);
        s %= 60; min %= 60;
    }
    END {
        printf "%d:%d:%d\n", h, min, s;
    }' salida.data                  #Con el comando split, seleccionamos la columna 6, utilizaremos la variable a para almacenar las partes, 
                                    #e indicamos que se separan por ":".
                                    #Una vez dividido el tiempo y almacenado en a, con a[1] horas, a[2] min, a[3] s,
                                    #hacemos los cálculos en función de los módulos (p.e., si s es mayor que 60, se suma cierta cantidad a los min)
                                    #cuando se acaban los cálculos, se imprime lo buscado
