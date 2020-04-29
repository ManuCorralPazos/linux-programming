ps -eo pid, %cpu, %mem, start, time, comm | grep $USERNAME 
echo "Número total de procesos: "
echo "%CPU total usada: "
echo "%Memoria total usada: "
echo "Tiempo total de CPU acumulado: "
exit
