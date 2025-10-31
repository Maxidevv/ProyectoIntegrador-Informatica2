#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <unistd.h>
#include <strings.h> // Para strcasecmp()

#define LONGITUD_MAX_LINEA 1024
#define TAMANIO_BLOQUE_INICIAL 10
#define NOMBREDE_ARCHIVO "Recursos-reservas.csv"

// --- ESTRUCTURAS ---
typedef struct {
    char pais[50];
    int anio;
    float recursos;
    float reservas;
} Registro;

typedef struct NodoPila {
    Registro* dato;
    struct NodoPila* siguiente;
} NodoPila;

typedef struct {
    NodoPila* tope;
} Pila;

typedef struct {
    Registro* registros;
    int cantidad;
    int capacidad;
} DatosMaestros;

// --- UTILIDADES ---
void limpiar_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void mostrar_hora_actual() {
    time_t ahora = time(NULL);
    struct tm *info_tiempo = localtime(&ahora);
    char buffer[50];
    strftime(buffer, sizeof(buffer), "[%d/%m/%Y %H:%M:%S]", info_tiempo);
    printf("%s ", buffer);
}

float obtener_valor_flotante(char *cadena) {
    if (cadena == NULL || cadena[0] == '\0' || strcmp(cadena, "-") == 0)
        return 0.0;
    return atof(cadena);
}

void limpiar_espacios(char *cadena) {
    if (!cadena) return;
    int inicio = 0;
    while (isspace((unsigned char)cadena[inicio])) inicio++;
    int fin = strlen(cadena) - 1;
    while (fin >= inicio && isspace((unsigned char)cadena[fin])) fin--;
    memmove(cadena, cadena + inicio, fin - inicio + 1);
    cadena[fin - inicio + 1] = '\0';
    if (cadena[0] == '"' && cadena[strlen(cadena) - 1] == '"') {
        memmove(cadena, cadena + 1, strlen(cadena) - 2);
        cadena[strlen(cadena) - 2] = '\0';
    }
}

int asegurar_espacio(DatosMaestros *datos) {
    if (datos->cantidad == datos->capacidad) {
        datos->capacidad *= 2;
        Registro* nuevo = realloc(datos->registros, datos->capacidad * sizeof(Registro));
        if (!nuevo) {
            datos->capacidad /= 2;
            perror("Error de memoria al ampliar array");
            return 0;
        }
        datos->registros = nuevo;
    }
    return 1;
}

// --- PILA ---
void inicializar_pila(Pila* p) { p->tope = NULL; }

void push(Pila* p, Registro* r) {
    NodoPila* nuevo = malloc(sizeof(NodoPila));
    if (!nuevo) { perror("Error al apilar"); return; }
    nuevo->dato = r;
    nuevo->siguiente = p->tope;
    p->tope = nuevo;
}

Registro* pop(Pila* p) {
    if (!p->tope) return NULL;
    NodoPila* nodo = p->tope;
    Registro* dato = nodo->dato;
    p->tope = nodo->siguiente;
    free(nodo);
    return dato;
}

int esta_vacia(Pila* p) { return p->tope == NULL; }

void liberar_pila(Pila* p) { while (!esta_vacia(p)) pop(p); }

// --- CSV ---
int cargar_datos_csv(DatosMaestros *datos) {
    FILE *archivo = fopen(NOMBREDE_ARCHIVO, "r");
    if (!archivo) {
        perror("Error al abrir el archivo CSV");
        return 0;
    }

    char linea[LONGITUD_MAX_LINEA];
    fgets(linea, LONGITUD_MAX_LINEA, archivo); // descartar encabezado

    datos->registros = malloc(TAMANIO_BLOQUE_INICIAL * sizeof(Registro));
    if (!datos->registros) { perror("Error de memoria inicial"); fclose(archivo); return 0; }
    datos->cantidad = 0;
    datos->capacidad = TAMANIO_BLOQUE_INICIAL;

    while (fgets(linea, LONGITUD_MAX_LINEA, archivo)) {
        if (!asegurar_espacio(datos)) break;

        char *copia = malloc(strlen(linea) + 1);
        if (!copia) { perror("Error al duplicar línea"); break; }
        strcpy(copia, linea);

        Registro nuevo;
        char *token;

        token = strtok(copia, ",;\t");
        strncpy(nuevo.pais, token ? token : "", 49);
        nuevo.pais[49] = '\0';
        limpiar_espacios(nuevo.pais);

        token = strtok(NULL, ",;\t");
        nuevo.anio = token ? atoi(token) : 0;

        token = strtok(NULL, ",;\t");
        nuevo.recursos = obtener_valor_flotante(token);

        token = strtok(NULL, ",;\t\n");
        nuevo.reservas = obtener_valor_flotante(token);

        datos->registros[datos->cantidad++] = nuevo;
        free(copia);
    }

    fclose(archivo);
    mostrar_hora_actual();
    printf("Datos cargados. Total de registros: %d\n", datos->cantidad);
    return 1;
}

// --- TENDENCIA ---
void analizar_tendencia(DatosMaestros *datos) {
    if (datos->cantidad == 0) {
        printf("Error: no hay datos cargados.\n");
        return;
    }

    char pais_filtro[50];
    int anio_inicio, anio_fin;

    printf("Ingrese el pais a analizar: ");
    if (scanf("%49s", pais_filtro) != 1) return;
    limpiar_espacios(pais_filtro);

    printf("Ingrese año de inicio: ");
    if (scanf("%d", &anio_inicio) != 1) return;

    printf("Ingrese año de fin: ");
    if (scanf("%d", &anio_fin) != 1) return;

    if (anio_inicio > anio_fin) {
        printf("El año de inicio no puede ser mayor al de fin.\n");
        return;
    }

    Pila pila;
    inicializar_pila(&pila);

    for (int i = 0; i < datos->cantidad; i++) {
        Registro *r = &datos->registros[i];
        if (strcasecmp(r->pais, pais_filtro) == 0 && r->anio >= anio_inicio && r->anio <= anio_fin)
            push(&pila, r);
    }

    if (esta_vacia(&pila)) {
        printf("No se encontraron registros para ese país y rango de años.\n");
        return;
    }

    Registro *reciente = pop(&pila);
    Registro *anterior;

    printf("\nTendencia de %s\n", pais_filtro);
    printf("Año_Anterior | Año_Reciente | Recursos | ΔRecursos | Reservas | ΔReservas\n");
    printf("--------------------------------------------------------------------------\n");

    while (!esta_vacia(&pila)) {
        anterior = pop(&pila);
        float var_rec = reciente->recursos - anterior->recursos;
        float var_res = reciente->reservas - anterior->reservas;
        printf("%d\t|\t%d\t|\t%.2f\t|\t%.2f\t|\t%.2f\t|\t%.2f\n",
               anterior->anio, reciente->anio,
               reciente->recursos, var_rec,
               reciente->reservas, var_res);
        reciente = anterior;
    }

    liberar_pila(&pila);
}

// --- TOP N ---
void intercambiar_registro(Registro *a, Registro *b) {
    Registro tmp = *a; *a = *b; *b = tmp;
}

void ordenar_bubble_sort(DatosMaestros *lista, int criterio) {
    int n = lista->cantidad;
    for (int i = 0; i < n - 1; i++)
        for (int j = 0; j < n - i - 1; j++) {
            Registro *A = &lista->registros[j];
            Registro *B = &lista->registros[j + 1];
            int swap = 0;
            if (criterio == 1 && A->recursos < B->recursos) swap = 1;
            else if (criterio == 2 && A->reservas < B->reservas) swap = 1;
            if (swap) intercambiar_registro(A, B);
        }
}

void calcular_top_n(DatosMaestros *datos) {
    if (datos->cantidad == 0) {
        printf("Error: no hay datos cargados.\n");
        return;
    }

    int anio, n, criterio;
    printf("Ingrese año a analizar: ");
    if (scanf("%d", &anio) != 1) return;
    printf("Ingrese N (cantidad de países): ");
    if (scanf("%d", &n) != 1 || n < 1) return;
    printf("Criterio (1: Recursos, 2: Reservas): ");
    if (scanf("%d", &criterio) != 1 || (criterio != 1 && criterio != 2)) return;

    DatosMaestros aux = {malloc(TAMANIO_BLOQUE_INICIAL * sizeof(Registro)), 0, TAMANIO_BLOQUE_INICIAL};
    if (!aux.registros) { perror("Error de memoria"); return; }

    for (int i = 0; i < datos->cantidad; i++) {
        if (datos->registros[i].anio == anio) {
            if (!asegurar_espacio(&aux)) break;
            aux.registros[aux.cantidad++] = datos->registros[i];
        }
    }

    if (aux.cantidad == 0) {
        printf("No hay datos para el año %d.\n", anio);
        free(aux.registros);
        return;
    }

    ordenar_bubble_sort(&aux, criterio);
    printf("\nTop %d por %s en %d\n", n, criterio == 1 ? "Recursos" : "Reservas", anio);
    printf("Pos | País | Valor\n");
    printf("---------------------------\n");
    for (int i = 0; i < n && i < aux.cantidad; i++) {
        float valor = criterio == 1 ? aux.registros[i].recursos : aux.registros[i].reservas;
        printf("%d | %s | %.2f\n", i + 1, aux.registros[i].pais, valor);
    }

    free(aux.registros);
}

// --- MAIN ---
int main() {
	printf("+----------------------------------------------------------+\n");
	printf("|                                                          |\n");
	printf("|                  BIENVENIDO/A AL PROGRAMA                |\n");
	printf("|                       Dataset: Litio                     |\n");
	printf("|                  Reservas a nivel mundial                |\n");
	printf("|                                                          |\n");
	printf("+----------------------------------------------------------+\n");

    sleep(2);
    mostrar_hora_actual();
    printf("Iniciando programa...\n\n");

    DatosMaestros datos = {NULL, 0, 0};
    if (!cargar_datos_csv(&datos)) return 1;

    char opcion;
    do {
        printf("Menu desplegado con exito.\n");
		printf("\n");
		printf("_______________________[MENU DE OPCIONES]__________________________\n");
		printf("\n");
		printf("[A] Analizar tendencia. \n[B] Calcular top. \n[C] Mostrar hora actual.\n[S] Salir.\n");
		printf("___________________________________________________________________\n");
		printf("\n");
        if (scanf(" %c", &opcion) != 1) opcion = ' ';
        limpiar_buffer();
        opcion = tolower(opcion);

        switch (opcion) {
            case 'a': analizar_tendencia(&datos); break;
            case 'b': calcular_top_n(&datos); break;
            case 'c': mostrar_hora_actual(); printf("\n"); break;
            case 's': printf("Saliendo...\n"); break;
            default: printf("Opción inválida.\n");
        }
    } while (opcion != 's');

    free(datos.registros);
    return 0;
}

