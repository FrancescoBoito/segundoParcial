#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ARCHIVO "nodos.dat"
#define TAM_UNIDAD 20

typedef struct Nodo {
    // Cada nodo guarda la direccion del dispositivo y su medicion y la direccion del proximo
    int direccionRS485;
    float medicion;
    char unidadDeMedida[TAM_UNIDAD];
    struct Nodo *siguiente;
} Nodo;

// Agrega un nodo nuevo al final de la lista enlazada
void agregarNodo(Nodo **lista, int direccion, float medicion, char unidad[]) {
    Nodo *nuevo = malloc(sizeof(Nodo));
    Nodo *actual;

    // Se revisa si malloc pudo reservar memoria
    if (nuevo == NULL) {
        printf("No se pudo reservar memoria.\n");
        return;
    }

    // Se copian los datos recibidos al nodo que se creo
    nuevo->direccionRS485 = direccion;
    nuevo->medicion = medicion;
    strncpy(nuevo->unidadDeMedida, unidad, TAM_UNIDAD - 1);
    nuevo->unidadDeMedida[TAM_UNIDAD - 1] = '\0';
    nuevo->siguiente = NULL;

    // Si no hay nodos, el nuevo pasa a ser el primero
    if (*lista == NULL) {
        *lista = nuevo;
    } else {
        actual = *lista;
        // Se recorre hasta encontrar el ultimo nodo
        while (actual->siguiente != NULL) {
            actual = actual->siguiente;
        }
        actual->siguiente = nuevo;
    }
}

void cargarNodoPorTeclado(Nodo **lista) {
    int direccion;
    float medicion;
    char unidad[TAM_UNIDAD];

    // Se piden los datos del sensor por teclado
    printf("Direccion RS485: ");
    scanf("%d", &direccion);
    printf("Medicion: ");
    scanf("%f", &medicion);
    printf("Unidad de medida (temp, hum, pres): ");
    scanf("%19s", unidad);

    agregarNodo(lista, direccion, medicion, unidad);
    printf("Nodo agregado.\n");
}

void mostrarNodos(Nodo *lista) {
    int numero = 1;

    // Verificamos si la lista tiene contenido

    if (lista == NULL) {
        printf("La lista esta vacia.\n");
        return;
    }

    // Se muestran los nodos uno por uno hasta llegar al final
    while (lista != NULL) {
        printf("Nodo %d - Direccion: %d | Medicion: %.2f | Unidad: %s\n",
               numero, lista->direccionRS485, lista->medicion,
               lista->unidadDeMedida);
        lista = lista->siguiente;
        numero++;
    }
}

// Muestra todos los datos del nodo que encontro la busqueda
void mostrarNodoEncontrado(Nodo *nodo, int numero) {
    printf("Coincidencia %d - Direccion: %d | Medicion: %.2f | Unidad: %s\n",
           numero, nodo->direccionRS485, nodo->medicion,
           nodo->unidadDeMedida);
}

// Busca nodos que tengan la misma direccion RS485
void buscarPorDireccion(Nodo *lista, int direccion) {
    int encontrados = 0;

    while (lista != NULL) {
        if (lista->direccionRS485 == direccion) {
            encontrados++; // Contamos cuantos encontramos
            mostrarNodoEncontrado(lista, encontrados);
        }
        lista = lista->siguiente;
    }

    if (encontrados == 0) {
        printf("No se encontro un nodo con la direccion %d.\n", direccion);
    }
}

// Busca nodos comparando la medicion guardada
void buscarPorMedicion(Nodo *lista, float medicion) {
    int encontrados = 0;

    while (lista != NULL) {
        // La medicion se compara de forma exacta en este ejercicio
        if (lista->medicion == medicion) {
            encontrados++;
            mostrarNodoEncontrado(lista, encontrados);
        }
        lista = lista->siguiente;
    }

    if (encontrados == 0) {
        printf("No se encontro un nodo con la medicion %.2f.\n", medicion);
    }
}

// Busca nodos que usen la misma unidad de medida
void buscarPorUnidad(Nodo *lista, char unidad[]) {
    int encontrados = 0;

    while (lista != NULL) {
        if (strcmp(lista->unidadDeMedida, unidad) == 0) {
            encontrados++; // Usamos string compare por el tipo de dato, nos devuelve cero si son iguales
            mostrarNodoEncontrado(lista, encontrados);
        }
        lista = lista->siguiente;
    }

    if (encontrados == 0) {
        printf("No se encontro un nodo con la unidad %s.\n", unidad);
    }
}

// Muestra las opciones y llama a la busqueda que corresponda
void menuBusqueda(Nodo *lista) {
    int criterio;
    int direccion;
    float medicion;
    char unidad[TAM_UNIDAD];

    if (lista == NULL) {
        printf("La lista esta vacia. No hay nodos para buscar.\n");
        return;
    }

    printf("\n--- BUSQUEDA DE NODOS ---\n");
    printf("1. Buscar por direccion RS485\n");
    printf("2. Buscar por medicion\n");
    printf("3. Buscar por unidad de medida\n");
    printf("Criterio: ");
    scanf("%d", &criterio);

    // Segun la opcion se pide un dato diferente
    // Igual al ejericio de los libros
    switch (criterio) {
        case 1:
            printf("Direccion RS485 a buscar: ");
            scanf("%d", &direccion);
            buscarPorDireccion(lista, direccion);
            break;
        case 2:
            printf("Medicion a buscar: ");
            scanf("%f", &medicion);
            buscarPorMedicion(lista, medicion);
            break;
        case 3:
            printf("Unidad de medida a buscar: ");
            scanf("%19s", unidad);
            buscarPorUnidad(lista, unidad);
            break;
        default:
            printf("Criterio de busqueda incorrecto.\n");
    }
}

// Guarda los datos en binario, pero no guarda el puntero de la lista xq el puntero puede cambiar
void guardarEnArchivo(Nodo *lista) {
    FILE *archivo = fopen(ARCHIVO, "wb");
    Nodo datos;

    if (archivo == NULL) {
        printf("No se pudo abrir el archivo para guardar.\n");
        return;
    }

    // Se escribe cada campo para que el puntero no quede guardado
    while (lista != NULL) {
        datos = *lista;
        datos.siguiente = NULL;
        fwrite(&datos.direccionRS485, sizeof(int), 1, archivo);
        fwrite(&datos.medicion, sizeof(float), 1, archivo);
        fwrite(datos.unidadDeMedida, sizeof(char), TAM_UNIDAD, archivo);
        lista = lista->siguiente;
    }

    fclose(archivo);
    printf("Datos guardados en %s.\n", ARCHIVO);
}

// Lee los registros del archivo y los agrega a la lista actual
void cargarDesdeArchivo(Nodo **lista) {
    FILE *archivo = fopen(ARCHIVO, "rb");
    int direccion;
    float medicion;
    char unidad[TAM_UNIDAD];
    int cantidad = 0;

    if (archivo == NULL) {
        printf("No se pudo abrir %s. Primero guarde datos.\n", ARCHIVO);
        return;
    }

    // El ciclo sigue mientras se puedan leer todos los datos de un nodo
    while (fread(&direccion, sizeof(int), 1, archivo) == 1 &&
           fread(&medicion, sizeof(float), 1, archivo) == 1 &&
           fread(unidad, sizeof(char), TAM_UNIDAD, archivo) == TAM_UNIDAD) {
        unidad[TAM_UNIDAD - 1] = '\0';
        agregarNodo(lista, direccion, medicion, unidad);
        cantidad++;
    }

    fclose(archivo);
    printf("Se agregaron %d nodos desde el archivo.\n", cantidad);
}

void liberarLista(Nodo **lista) {
    Nodo *auxiliar;

    // Se libera cada nodo para evitar dejar memoria ocupada usando una variable auxiliar
    while (*lista != NULL) {
        auxiliar = *lista;
        *lista = (*lista)->siguiente;
        free(auxiliar);
    }
}

int main(void) {
    Nodo *lista = NULL;
    int opcion;

    // El menu se repite hasta que el usuario elija salir
    // Acceso libre a todas las funcionalidades
    do {
        printf("\n--- RED RS485 ---\n");
        printf("1. Agregar nodo\n");
        printf("2. Mostrar nodos\n");
        printf("3. Guardar nodos en archivo\n");
        printf("4. Cargar nodos desde archivo\n");
        printf("5. Buscar nodos\n");
        printf("0. Salir\n");
        printf("Opcion: ");
        scanf("%d", &opcion);

        // Se ejecuta la accion elegida por el usuario
        switch (opcion) {
            case 1:
                cargarNodoPorTeclado(&lista);
                break;
            case 2:
                mostrarNodos(lista);
                break;
            case 3:
                guardarEnArchivo(lista);
                break;
            case 4:
                cargarDesdeArchivo(&lista);
                break;
            case 5:
                menuBusqueda(lista);
                break;
            case 0:
                liberarLista(&lista);
                printf("Programa finalizado.\n");
                break;
            default:
                printf("Opcion incorrecta.\n");
        }
    } while (opcion != 0);

    return 0;
}
