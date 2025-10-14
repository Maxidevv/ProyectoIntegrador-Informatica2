int main() 
{
    char eleccionUsuario;
    FILE *fp;

        printf("========================================================================\n");
        printf("                                                      B I E N V E N I D O / A                                                                        \n");
        printf("                                                 P R O Y E C T O   C:   L I T I O                                                                 \n");
        printf("========================================================================\n");
    do 
    {
        printf("=============================================\n");
        printf("[A] Leer archivos. \n[B] - Ordenar. \n[C] Imprimir.");
        printf("=============================================\n");
        printf("\n");
        printf("A continuacion ingrese una opcion:\n> ");
        scanf("%c ", &eleccionUsuario);
        eleccionUsuario = tolower(eleccionUsuario);

        switch(eleccionUsuario) 
        {
            case 'a':
                printf("Usted ingreso a la opcion A.\n");
                leer_csv();
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
