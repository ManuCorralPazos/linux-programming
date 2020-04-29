#al ejecutar el script, se pasa como primer y unico parametro el nombre
#del fichero a inspeccionar. Dicho nombre se guardará en $1 por defecto
path=`find /home -iname $1` #asignamos a la variable path la salida del comando entre los acentos agudos
echo "El path es $path" #comprobamos que la variable path está bien definida

if [ -z "$path" ] #comprobamos si el fichero introducido
	#por linea de comandos existe
then    
	echo "No existe"
	echo "$(date +'%B %d %H:%M:%S') $USERNAME $1: Fichero no encontrado" >> ~/error.log
	#la parte que está entre comillas se introduce en el fichero error.log gracias a >>
	#la cadena de texto se compone primero de la fecha formateada como se pide en el enunciado, luego el nombre del usuario, el path creado anteriormente y el texto "fichero no encontrado", completando así el ejercicio
else
 	echo "Si que existe"
    cp  $path /tmp #si existe, compiamos dicho fichero a una carpeta llamada /tmp situada en el mismo directorio que /home
fi #se sale del if
exit #se sale del script               
