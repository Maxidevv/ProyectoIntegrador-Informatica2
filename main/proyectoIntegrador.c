#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

void leer_csv(const char *Produccion mundial de litio) {
    FILE *fp = fopen("Produccion mundial de litio.csv", "r");
    if (fp == NULL) {
        printf("No se pudo abrir el archivo.\n");
        return;
    }

    char linea[256];
    fgets(linea, sizeof(linea), fp);

    char pais[64];
    int anio;
    float toneladas;

    while (fgets(linea, sizeof(linea), fp)) {
        if (sscanf(linea, "%63[^;];%d;%f", pais, &anio, &toneladas) == 3) {
            printf("País: %s, Año: %d, Toneladas: %.6f\n", pais, anio, toneladas);
        }
    }

    fclose(fp);
}
int main() {
    leer_csv("main/resources/Produccion mundial de litio.csv");
    return 0;
}
void LitioTotal_anio(){//Esta funcion lo que hace es juntar la cantidad total de litio producido por paises, y ordenalos de mayor a menor(descendente)

}