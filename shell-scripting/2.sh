echo $1 #comprobamos qué argumento se ha tomado
if (grep -i $1 /etc/shells) #con este comando se busca que el string de $1 (el introducido por el usuario como parámetro), exista dentro de /etc/shells. Así, sabremos si el shell existe o no. El comando -i es opcional, sirve para buscar con indiferencia con respecto a mayúsculas/minúsculas.
then
	echo "Sí que existe el shell pedido" #si la salida del grep no es nula significa que dicho string está en /etc/shells, por tanto realizamos el filtrado pedido en el enunciado
	echo "Usuarios con dicho shell:"
	cat /etc/passwd | grep $1 | cut -d: -f 1,7 #con el comando cat "se entra" en el contenido del archivo /etc/passwd. Con el pipe, se pasa la salida del comando cat, en este caso el contenido del archivo, al comando grep, que lista únicamente las líneas en las que se encuentra el contenido de $1. Volvemos a utilizar el pipe y ahora utilizamos cut para elegir únicamente las columnas 1 y 7, que son las del nombre de usuario y el path 
else
	echo "No existe" #si la salida del grep es nula, significa que dicho string NO está en /etc/shells, por tanto no existe y se sale del script
fi #se sale del if
exit #se sale del script
