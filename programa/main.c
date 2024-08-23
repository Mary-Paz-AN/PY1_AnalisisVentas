#include <stdio.h>

void limpiarBuffer();
void menu();

int main() {
    menu();

    return 0;
}

//Funcionalidades
//Limpia el buffer para impedir errores
void limpiarBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

//Menú Principal
void menu() {
    char o;

    printf("       MENU       \n");
    printf("------------------\n");
    printf("A: Importar datos\n");
    printf("B: Procesamiento de datos\n");
    printf("C: Análisis de datos\n");
    printf("D: Análisis temporal\n");
    printf("E: Estadísticas\n");
    printf("S: Salir\n");
    printf("Escriba la opción deseada:\n");

    //Se pasa a mayuscula por si se escribe alguna de las letras en minuscula 
    o = toupper(getchar());
    limpiarBuffer();
    printf("\n");

    //Switch para acceder a las funcionalidades de las opciones del menu
    switch (o)
    {
    case 'A':
        f();
        break;
    
    case 'B':
        f();
        break;

    case 'C':
        f();
        break;

    case 'D':
        f();
        break;

    case 'E':
        f();
        break;

    case 'S':
        f();
        break;
    
    default:
        printf("Opción Invalida Porfavor vuelva a intentarlo.\n\n");
        menu();
        break;
    }

}
