<<<<<<< HEAD
int main() 
{
    char eleccionUsuario;
    FILE *fp;

    printf("Bienvenido al programa.\n");
    do 
    {
        printf("________________________________________________________\n");
        printf("[A] Leer archivos. \n[B] - Ordenar. \n[C] Imprimir.");
        printf("________________________________________________________\n");
        printf("A continuacion ingrese una opcion:\n> ");
        scanf("%c ", &eleccionUsuario);
        eleccionUsuario = tolower(eleccionUsuario);

        switch(eleccionUsuario) 
        {
            case 'a':
                break;
            case 'b':
                break;
            case 'c':
                break;
            default:
                printf("La opcion no corresponde.\n");
        }
    } while(eleccionUsuario != 's');

    return 0;
}
=======
#include <stdio.h>
>>>>>>> e9382bb8f4c1db8e6f77eb164238b7bdb68b97a9
