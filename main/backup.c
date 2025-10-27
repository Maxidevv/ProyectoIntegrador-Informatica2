#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h> 
#include <ctype.h>
#include <windows.h> 

// --- DEFINICIONES SIMPLIFICADAS ---
#define LONGITUD_MAX_LINEA 1024
#define TAMANIO_BLOQUE_INICIAL 10
#define NOMBREDE_ARCHIVO "Recursos-reservas.csv"

// --- 1. ESTRUCTURAS Y PUNTEROS (REQUISITOS CLAVE) ---

// Estructura para cada fila del CSV
typedef struct {
	char pais[50];
	int anio;
	float recursos;
	float reservas;
} Registro;

// Nodo para la Pila (Punteros)
typedef struct NodoPila {
	Registro* dato; // El nodo de la pila APUNTA a un registro
	struct NodoPila* siguiente;
} NodoPila;

// Estructura de control de la Pila
typedef struct {
	NodoPila* tope;
} Pila;

// Estructura para almacenar TODOS los datos del CSV (Array Dinamico)
typedef struct {
	Registro* registros; // Puntero al array dinamico de todos los datos
	int cantidad;
	int capacidad;
} DatosMaestros;


// --- 2. FUNCIONES DE UTILIDAD Y TIEMPO ---

// Muestra la hora actual (en la que se compila el codigo)
void mostrar_hora_actual() {
	time_t ahora = time(NULL);
	struct tm *info_tiempo = localtime(&ahora); 
	
	char buffer[50]; 
	strftime(buffer, sizeof(buffer), "[%d/%m/%Y %H:%M:%S]", info_tiempo); 
	
	printf("%s ", buffer);
}

// Convierte cadena a float (simplificado)
float obtener_valor_flotante(char *cadena) {
	// Retorna 0.0 si la cadena esta vacia o es un "-"
	if (cadena == NULL || cadena[0] == '\0' || strcmp(cadena, "-") == 0) {
		return 0.0; 
	}
	return atof(cadena);
}

// Asegura que el array tenga espacio (realloc)
void asegurar_espacio(DatosMaestros *datos) {
	if (datos->cantidad == datos->capacidad) {
		datos->capacidad *= 2;
		datos->registros = (Registro*)realloc(datos->registros, datos->capacidad * sizeof(Registro));
		if (datos->registros == NULL) {
			perror("Error de memoria. No se pudo asegurar el espacio.");
			return; 
		}
	}
}


// --- 3. FUNCIONES DE LA PILA (PUNTEROS Y MEMORIA) ---

void inicializar_pila(Pila* p) {
	p->tope = NULL;
}

void push(Pila* p, Registro* r) {
	NodoPila* nuevo_nodo = (NodoPila*)malloc(sizeof(NodoPila));
	if (nuevo_nodo == NULL) {
		perror("Error al asignar memoria para apilar datos");
		return;
	}
	nuevo_nodo->dato = r; 
	nuevo_nodo->siguiente = p->tope;
	p->tope = nuevo_nodo;
}

Registro* pop(Pila* p) {
	if (p->tope == NULL) {
		return NULL;
	}
	
	NodoPila* nodo_a_eliminar = p->tope;
	Registro* dato_retorno = nodo_a_eliminar->dato;
	
	p->tope = p->tope->siguiente;
	free(nodo_a_eliminar); 
	
	return dato_retorno;
}

int esta_vacia(Pila* p) {
	return (p->tope == NULL);
}

void liberar_pila(Pila* p) {
	while (esta_vacia(p) == 0) {
		pop(p);
	}
}

// --- 4. CARGA DE CSV (Uso de Archivos) ---

int cargar_datos_csv(DatosMaestros *datos) {
	FILE *archivo = fopen(NOMBREDE_ARCHIVO, "r");
	if (archivo == NULL) {
		perror("Error al abrir el archivo");
		return 0; 
	}
	
	char linea[LONGITUD_MAX_LINEA];
	if (fgets(linea, LONGITUD_MAX_LINEA, archivo) == NULL) {
		fclose(archivo);
		return 0;
	}
	
	datos->registros = (Registro*)malloc(TAMANIO_BLOQUE_INICIAL * sizeof(Registro));
	datos->cantidad = 0;
	datos->capacidad = TAMANIO_BLOQUE_INICIAL;
	if (datos->registros == NULL) {
		perror("Error al asignar memoria");
		fclose(archivo);
		return 0;
	}
	
	while (fgets(linea, LONGITUD_MAX_LINEA, archivo)) {
		asegurar_espacio(datos);
		
		// strdup no es estándar, pero se mantiene la lógica original
		char *copia_linea = strdup(linea); 
		char *token;
		Registro nuevo_registro;
		
		// Parsing de la linea
		token = strtok(copia_linea, ","); strncpy(nuevo_registro.pais, token ? token : "", 49); nuevo_registro.pais[49] = '\0';
		token = strtok(NULL, ","); nuevo_registro.anio = token ? atoi(token) : 0;
		token = strtok(NULL, ","); nuevo_registro.recursos = obtener_valor_flotante(token);
		token = strtok(NULL, ",\n"); nuevo_registro.reservas = obtener_valor_flotante(token);
		
		// Almacenamiento en el array dinamico creado
		datos->registros[datos->cantidad++] = nuevo_registro;
		free(copia_linea); 
	}
	
	fclose(archivo);
	mostrar_hora_actual();
	printf("Datos cargados. Total de registros: %d\n", datos->cantidad);
	return 1; //Carga exitosa
}


// --- 5. ANALISIS DE TENDENCIA (USO DE PILA) ---

void analizar_tendencia(DatosMaestros *datos) {
	if (datos->cantidad == 0) {
		printf("Error,datos no cargados, el archivo esta vaciO\n");
		return;
	}
	mostrar_hora_actual();
	printf("Iniciando analisis de la Tendencia Historica.\n");
	
	char pais_filtro[50];
	int anio_inicio, anio_fin;
	
	printf("Ingrese el pais a analizar: ");
	// El %s lee la cadena y se detiene en el primer espacio o nueva línea
	scanf("%49s", pais_filtro); 
	
	// Limpieza de buffer después de leer la cadena del país (si el usuario ingresa más caracteres o usa el espacio)
	int c; while ((c = getchar()) != '\n' && c != EOF);
	
	printf("Ingrese Anio de inicio: ");
	scanf("%d", &anio_inicio);
	printf("Ingrese anio de fin: ");
	scanf("%d", &anio_fin);
	
	// Limpieza de buffer después de leer el segundo entero (anio_fin)
	while ((c = getchar()) != '\n' && c != EOF);
	
	Pila pila_analisis;
	inicializar_pila(&pila_analisis);
	
	// Filtrar y Apilar (la pila invierte el orden cronologico)
	for (int i = 0; i < datos->cantidad; i++) {
		Registro *r = &datos->registros[i];
		if (strcmp(r->pais, pais_filtro) == 0 && r->anio >= anio_inicio && r->anio <= anio_fin) {
			push(&pila_analisis, r); // Puntero al registro
		}
	}
	
	if (esta_vacia(&pila_analisis)) {
		printf("No hay datos disponibles ni encontrados para el pais y rango de anios.\n");
		return;
	}
	
	Registro *reciente = pop(&pila_analisis);
	Registro *anterior;
	
	printf("\nTendencia de %s (Cambio Anios a Anios) \n", pais_filtro);

	// Imprimir cabecera tipo tabla (con tabulaciones para alinear)
	printf("Anio_Anterior\t|\tAnio_Reciente\t|\tRecursos_Reciente\t|\tVar_Recursos\t|\tReservas_Reciente\t|\tVar_Reservas\n");
	printf("---------------------------------------------------------------------------------------------\n");

	// Analisis LIFO usando POP
	while (!esta_vacia(&pila_analisis)) {
		anterior = pop(&pila_analisis);

		float cambio_rec = reciente->recursos - anterior->recursos;
		float cambio_res = reciente->reservas - anterior->reservas;

		// Formato tipo tabla con tabulacion alrededor de '|'
		printf("%d\t|\t%d\t|\t%.2f\t|\t%.2f\t|\t%.2f\t|\t%.2f\n",
			anterior->anio, reciente->anio,
			reciente->recursos, cambio_rec,
			reciente->reservas, cambio_res);

		reciente = anterior;
	}
	liberar_pila(&pila_analisis);
}


// --- 6. TOP N ---

// Función auxiliar para intercambiar dos Registros (necesaria para el Bubble Sort)
void intercambiar_registro(Registro *a, Registro *b) {
	Registro temp = *a; // Almacena el contenido de 'a'
	*a = *b;            // Copia el contenido de 'b' en 'a'
	*b = temp;          // Copia el contenido temporal en 'b'
}

// Ordenamiento bubble sort
void ordenar_bubble_sort(DatosMaestros *lista, int opcion_criterio) {
	int i, j;
	int n = lista->cantidad;
	
	for (i = 0; i < n - 1; i++) {
		for (j = 0; j < n - i - 1; j++) {
			Registro *regA = &lista->registros[j];
			Registro *regB = &lista->registros[j + 1];
			int intercambio = 0;
			
			if (opcion_criterio == 1) { // Ordenar por Recursos (Mayor a Menor)
				if (regA->recursos < regB->recursos) {
					intercambio = 1;
				}
			} else { // Ordenar por Reservas (Mayor a Menor)
				if (regA->reservas < regB->reservas) {
					intercambio = 1;
				}
			}
			if (intercambio) {
				intercambiar_registro(regA, regB);
			}
		}
	}
}

void calcular_top_n(DatosMaestros *datos) {
	if (datos->cantidad == 0) {
		printf("Error: Datos no cargados.\n");
		return;
	}
	mostrar_hora_actual();
	printf("Iniciando calculo de Top N.\n");
	
	int anio_seleccionado, n, opcion_criterio;
	printf("Ingrese anio a analizar: ");
	scanf("%d", &anio_seleccionado);
	printf("Ingrese el numero N (1,2...): ");
	scanf("%d", &n);
	printf("Seleccione el criterio que desea usar para el top ordenado (1: Recursos, 2: Reservas): ");
	scanf("%d", &opcion_criterio);
	
	// Limpieza de buffer después de leer el entero (si no se hace, el menú falla al volver)
	int c; while ((c = getchar()) != '\n' && c != EOF); 
	
	// Array auxiliar para ordenar
	DatosMaestros datos_auxiliares = {NULL, 0, TAMANIO_BLOQUE_INICIAL};
	datos_auxiliares.registros = (Registro*)malloc(TAMANIO_BLOQUE_INICIAL * sizeof(Registro));
	if (datos_auxiliares.registros == NULL) return; 
	
	// 1. Filtrar los datos para el año
	for (int i = 0; i < datos->cantidad; i++) {
		Registro r = datos->registros[i];
		if (r.anio == anio_seleccionado) {
			asegurar_espacio(&datos_auxiliares);
			datos_auxiliares.registros[datos_auxiliares.cantidad++] = r;
		}
	}
	
	if (datos_auxiliares.cantidad == 0) {
		printf("No se encontraron datos para el anio %d.\n", anio_seleccionado);
		free(datos_auxiliares.registros);
		return;
	}
	
	char *criterio_elegido = "";
	if (opcion_criterio == 1) {
		criterio_elegido = "Recursos";
	} else if (opcion_criterio == 2) {
		criterio_elegido = "Reservas";
	} else {
		printf("Opcion de criterio invalida.\n");
		free(datos_auxiliares.registros);
		return;
	}
	
	// Ordenamiento con bubble sort
	ordenar_bubble_sort(&datos_auxiliares, opcion_criterio);
	
	// 3. Imprimir el Top N en formato tabular (una fila por item)
	printf("\nEl top %d de %s para el anio %d\n", n, criterio_elegido, anio_seleccionado);
	int limite = (n < datos_auxiliares.cantidad) ? n : datos_auxiliares.cantidad;

	// Cabecera tipo tabla (con tabulaciones)
	printf("Pos\t|\tPais\t|\t%s\n", criterio_elegido);
	printf("------------------------------------\n");

	for (int i = 0; i < limite; i++) {
		Registro *r = &datos_auxiliares.registros[i];
		float valor = (opcion_criterio == 1) ? r->recursos : r->reservas;
		// Imprimir cada fila con tabulaciones alrededor de '|'
		printf("%d\t|\t%s\t|\t%.2f\n", i + 1, r->pais, valor);
	}
	
	free(datos_auxiliares.registros);
}
// --- 7. FUNCION PRINCIPAL Y MENU ---

int main() 
{
	char eleccionUsuario;
	
	printf("+----------------------------------------------------------+\n");
	printf("|                                                          |\n");
	printf("|                  BIENVENIDO/A AL PROGRAMA                |\n");
	printf("|                      Proyecto: Litio                     |\n");
	printf("|                  Reservas a nivel mundial                |\n");
	printf("|                                                          |\n");
	printf("+----------------------------------------------------------+\n");
	
	printf("\n");
	printf("El programa se esta iniciando...\n");
	printf("\n");
	Sleep(5000); // Esperamos 5 segundos.
	mostrar_hora_actual();
	printf("Programa iniciado con exito.\n");
	// La variable que contiene todos los datos cargados.
	DatosMaestros datos_maestros = {NULL, 0, 0}; 
	
	// 1. Cargar datos. Si falla, datos_maestros.cantidad seria = 0.
	if (!cargar_datos_csv(&datos_maestros)) 
	{
		printf("\nERROR: Terminando programa. Revise la ubicacion del archivo.\n");
		return 1; // Retorno de error simple
	}
	printf("\n");
	printf("Para desplegar el menu de opciones presione ENTER.\n");

	int c; while ((c = getchar()) != '\n' && c != EOF);
	
	do 
	{	
		printf("Menu desplegado con exito.\n");
		printf("\n");
		printf("_______________________[MENU DE OPCIONES]__________________________\n");
		printf("\n");
		printf("[A] Analizar tendencia. \n[B] Calcular top. \n[C] Mostrar hora actual.\n[S] Salir.\n");
		printf("___________________________________________________________________\n");
		printf("\n");
		printf("A continuacion ingrese una opcion:\n> ");
		
		if (scanf(" %c", &eleccionUsuario) != 1) { 
			eleccionUsuario = ' '; // Opcion invalida
		}
		
		while ((c = getchar()) != '\n' && c != EOF); 
		
		eleccionUsuario = tolower(eleccionUsuario);
		
		switch(eleccionUsuario) 
		{
		case 'a':
			analizar_tendencia(&datos_maestros);
			break;
		case 'b':
			calcular_top_n(&datos_maestros);
			break;
		case 'c':
			printf("Hora actual: ");
			mostrar_hora_actual();
			printf("\n");
			break;
		case 's':
			printf("Saliendo del programa...\nLiberando memoria...");
			Sleep(3000);
			printf("La memoria se libero con exito.\n");
			break;
		default:
			printf("La opcion no corresponde, intente de nuevo.\n");
		}
	} while(eleccionUsuario != 's');
	
	// Liberacion de memoria principal (Punteros)
	if (datos_maestros.registros != NULL) 
	{
		free(datos_maestros.registros);
	}
	return 0;
}
