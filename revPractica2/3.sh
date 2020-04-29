if test $# -eq 0 #comprobamos la cantidad de argumentos que hay, la variable $# devuelve la cantidad de argumentos 
				#(a parte del nombre del script), y el -eq compara lo que hay antes con lo que hay después
then #si el numero de argumentos es 0
	sort /etc/passwd #utilizamos la ordenación por defecto
	echo "Ordenada con la opcion por defecto" 
elif test $# -eq 1 #si el numero de argumentos es 1. Como solo hay un argumento, este deberá ser la columna 
				   #por la cuál queremos ordenar la lista de passwd. De no ser así, se mostrará un error.
then
	#echo "Hay un argumento"
	case $1 in #utilizamos el comando case para seleccionar la opción en función del primer comando
		login)
			sort -k 1 /etc/passwd #la opción de sort -k permite ordenar el archivo en función de la columna indicada después.
								  #En este caso, ordenamos por la primera columna ya que el parámetro pasado es login
			;; #salimos del primer caso
		uid) 
			sort -k 3 /etc/passwd #ordenamos por la tercera columna
			;;
		gid)
			sort -k 4 /etc/passwd #ordenamos por la cuarta columna
			;;
		*) #este asterisco indica la opción por defecto en caso de no entrar en ninguno de los casos anteriores
			echo "$0: parámetro inválido '$1'" #se muestra el mensaje de error
			echo "Intenta: $0 [-a|-z] [login|uid|gid]." #sugerencia de sintaxis para futuras ocasiones
			exit 2 #se sale con código de error 2, ya que el error está en el parámetro 
			;;
	esac #salimos del case
elif test $# -eq 2 #si el numero de argumentos es 2. Si hay dos argumentos, la sintaxis debe ser [-a|-z] [login|uid|gid]. 
				   #De no ser así y si los argumentos están cambiados de orden, se dará un error. 
				   #Este caso se reduce a revisar si el primer argumentos es -a o -z y en otro caso, salir con error 1.
then
	#echo "Hay dos argumentos"
	case $1 in #primer case, correspondiente a la opción del script
		-a)
			case $2 in #segundo case, corresondiente al parámetro de la columna por la cual queremos ordenar
				login)
					sort -k 1 /etc/passwd #comportamiento similar al descrito anteriormente
					;;
				uid) 
					sort -k 3 /etc/passwd
					;;
				gid)
					sort -k 4 /etc/passwd
					;;
				*)
					echo "$0: parámetro inválido '$2'"
					echo "Intenta: $0 [-a|-z] [login|uid|gid]."
					exit 2
					;;
			esac
			;;
		-z)
			case $2 in
				login)
					sort -r -k 1 /etc/passwd #nótese que en este sort utilizamos la opción -r, para ordenar de forma inversa, 
										     #puesto que la opción elegida fue -z
					;;
				uid) 
					sort -r -k 3 /etc/passwd
					;;
				gid)
					sort -r -k 4 /etc/passwd
					;;
				*)
					echo "$0: parámetro inválido '$2'"
					echo "Intenta: $0 [-a|-z] [login|uid|gid]."
					exit 2
					;;
			esac
			;;
		*)
			echo "$0: parámetro inválido '$1'" #en este caso, el error estaría en la opción, por tanto se muestra
			echo "Intenta: $0 [-a|-z] [login|uid|gid]."
			exit 1 #se sale con código de error 1, puesto que el error está en la opción
			;;
	esac

else #si el numero de argumentos es mayor que 2
	echo "Error, hay muchos argumentos"
	echo "Intenta: $0 [-a|-z] [login|uid|gid]."
	exit 3 #se crea el error 3 indicando que la cantidad de argumentos pasados al script supera el número permitido
fi 
exit #se sale del script y se finaliza 
