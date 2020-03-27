#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <mysql.h>
int err;
char consulta[100];//Lo usamos para preparar las conultas
char cuestionador[100];
MYSQL *conn;//Conector para acceder al servidor de bases de datos
MYSQL_RES *resultado;//variable para comprobar errores
MYSQL_ROW row;

//SOCKETS//
int sock_conn, sock_listen, ret;
char buff[512];
char buff2[512];

int LogIN (char nombre[40], char password[40])
{
		int a;
		int comparador;
		int logueado = 0;
		char *p;
		p = strtok (NULL, "/");
		strcpy(nombre, p); //almacenamos el nombre escrito por el ususario
		p = strtok (NULL, "/");
		strcpy (password, p); //almacenamos la password dada por el usuario
		
		// Ahora recibimos sus credenciales, que dejamos en buff
		printf ("Credenciales recibidas\n");
		
		
		//preparamos la consulta en modo SQL
		strcpy(consulta,"SELECT JUGADOR.pass FROM JUGADOR WHERE JUGADOR.nombre   = '");
		strcat(consulta,nombre);
		strcat(consulta,"'");
		err = mysql_query(conn,consulta); //almaceno el resultado de la consulta en modo SQL dentro de "conn"
		if (err!=0) {
			
			printf ("Error al consultar datos de la base %u %s\n",
					mysql_errno(conn), mysql_error(conn));
			exit (1);
		}
		
		resultado = mysql_store_result(conn);
		row = mysql_fetch_row (resultado);
		if (row == NULL) 
		{
			printf("This e-mail isn't in our database");
		}
		
		
		comparador = strcmp(password, row[0]); //comparamos la password dada con la real
		if (comparador == 0) //Password correcta
		{
			printf("Correctly Logged\n");
			logueado = 1; 
			write(sock_conn,"0", sizeof("0"));
			return 0;
		}
		if (comparador != 0) //Password incorrecta
		{
			printf("Password error\n");
			write(sock_conn,"1", sizeof("1"));
			return 1;
		}
		
		
	

}
void DameNombre_MayorEdad(char mayores[80])
{
	
	printf("Pregunta recibida\n");
	
	strcpy(consulta, "SELECT JUGADOR.nombre FROM JUGADOR WHERE JUGADOR.edad >=18;");
	mysql_query(conn,consulta);
	resultado = mysql_store_result(conn);
	row = mysql_fetch_row (resultado);
	
	printf("Cuestion realizada sin errores\n");
	printf("NOmbre: %s\n",row[0]);
	strcpy(mayores,row[0]);
	/*while(row != NULL)
	{
	sprintf (mayores, "%s/",mayores);
	} */
	if (row == NULL)
		printf ("No se han obtenido datos en la consulta\n");
}
void DameID_MayorPuntaje(char id[40])
{
	printf("Pregunta recibida\n");
	
	strcpy(consulta,"SELECT JUGADOR FROM JUGADOR, HISTORIAL WHERE HISTORIAL.puntuacion=");
	//strcat(consulta, cuestionador);
	strcat(consulta,"(SELECT MAX(HISTORIAL.puntuacion) FROM HISTORIAL);");
	
	printf("consulta  = %s\n",consulta);
	
	///
	//err=mysql_query (conn, "SELECT * FROM HISTORIAL");
	err=mysql_query (conn,consulta);
	if (err!=0)
	{
		printf ("Error al consultar datos de la base %u %s\n",
				mysql_errno(conn), mysql_error(conn));
		exit (1);
	}
	
	resultado = mysql_store_result (conn);
	row = mysql_fetch_row (resultado);
	if (row == NULL)
		printf ("No se han obtenido datos en la consulta\n");
	else
	{
		printf ("el ID del jugador con mayor puntaje es : %s\n",
				row[0]);
		//row = mysql_fetch_row (consulta);
		printf(printf("id: %s\n",row[0]));
		strcpy (id,row[0]);
		
	}
	mysql_close(conn);
	
	
}
void DameElo_jugador (char division [40])
{
	char*p;
	p = strtok (NULL, "/");
	strcpy(cuestionador, p); //almacenamos el nombre escrito por el ususario
	printf("Pregunta recibida\n");
	
	strcpy(consulta, "SELECT JUGADOR.elo FROM JUGADOR WHERE JUGADOR.nombre = '");
	strcat(consulta, cuestionador);
	strcat (consulta,"';");
	
	mysql_query(conn,consulta);
	resultado = mysql_store_result(conn);
	row = mysql_fetch_row (resultado);
	
	printf("Cuestion realizada sin errores\n");
	printf(printf("division: %s\n",row[0]));
	strcpy (division,row[0]);
	if (row == NULL)
		printf ("No se han obtenido datos en la consulta\n");
}
	
int main(int argc, char *argv[])
{

	//MYSQL//
	//Creamos una conexion al servidor MYSQL 
	conn = mysql_init(NULL);
	if (conn==NULL) {
		printf ("Error al crear la conexion: %u %s\n", 
				mysql_errno(conn), mysql_error(conn));
		exit (1);
	}
	
	//inicializar la conexion, indicando nuestras claves de acceso
	// al servidor de bases de datos (user,pass)
	conn = mysql_real_connect (conn, "localhost","root", "mysql", NULL, 0, NULL, 0);
	if (conn==NULL)
	{
		printf ("Error al inicializar la conexion: %u %s\n", 
				mysql_errno(conn), mysql_error(conn));
		exit (1);
	}
	
	//entramos en la base de datos "juego"
	err=mysql_query (conn, "use juego");
	if (err!=0)  //por si la base de datos no existe
	{
		printf("Error al seleccionar la base de datos\n");
		exit (1);
	}
	
	//SOCKETS//
	struct sockaddr_in serv_adr;
	
	
	// INICIALITZACIONS
	// Obrim el socket
	if ((sock_listen = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		printf("Error creant socket");
	// Fem el bind al port
	
	
	memset(&serv_adr, 0, sizeof(serv_adr));// inicialitza a zero serv_addr
	serv_adr.sin_family = AF_INET;
	
	// asocia el socket a cualquiera de las IP de la m?quina. 
	//htonl formatea el numero que recibe al formato necesario
	serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
	// escucharemos en el port 9050 o aprecidos en caso de error de blind
	serv_adr.sin_port = htons(9062);
	if (bind(sock_listen, (struct sockaddr *) &serv_adr, sizeof(serv_adr)) < 0)
		printf ("Error al bind");
	
	//La cola de peticiones pendientes no podr? ser superior a 4
	if (listen(sock_listen, 2) < 0)
		printf("Error en el Listen");
	
	int select; //selecciona que esta haciendo el usuario
	char nombre[20];
	char password[20];
	char cuestionador[20];
	char mayores[40];
	char identificador[40];
	char division [40];
	for(;;)
	{
		
		printf ("Escuchando\n");
		//sock_conn es el socket que usaremos para este cliente
		sock_conn = accept(sock_listen, NULL, NULL);
		printf ("Conexion establecida\n");
		
		ret=read(sock_conn,buff, sizeof(buff));
		
		char *p = strtok(buff, "/");
		select = atoi(p);
		
		if(select == 0) //Log In
		{
			int login = LogIN(nombre,password); // fucion login
		if (login == 0) 
			 printf("logueado correctamente");// se loguea correctamente
		else if (login == 1)
		printf("se ha puesto mal la contraseña");
		// error al poner la contraseña
		}
		if (select == 1) //Dar el nombre de jugador(es)que sea mayor de edad
		{ printf("Este es la primera pregunta\n");
			DameNombre_MayorEdad(mayores);
			write(sock_conn,mayores, sizeof(mayores));
			
			
		}
		
		if (select == 2) //Dar el  id del jugador con mayor puntaje//
		{  
			printf("Este es la segunda pregunta\n");
			DameID_MayorPuntaje(identificador);
			write(sock_conn,identificador, sizeof(identificador));
		}
		
		if (select == 3) //Dar la clasificacion(elo) del jugador
		{
			printf("Este es la tercera pregunta\n");
			DameElo_jugador(division);
			write(sock_conn,division, sizeof(division));
			
		}
		
		
		close (sock_conn);
	} 
}


