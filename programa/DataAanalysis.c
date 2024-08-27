#include "DataAanalysis.h"

// Global variables
Sale **sales;
int numSales = 0;

//Main function
int main() {
    //Loads the data
    bool load;
    load = loadMemory("jsonFile/ventas.json", 0);

    //If theres an error during the process the program will stop
    if(!load) {
        printf("Ocurri%c un error al cargar los datos.\nPorfavor vuelva a iniciar el programa.", 162);
        return 1;
    }

    //Start the program
    menu();

    return 0;
}

//Functions
//Cleans the memory assigned to the sales array
void cleanMemorySales() {
    for (int i = 0; i < numSales; i++) {
        free(sales[i]->date);
        free(sales[i]->productName);
        free(sales[i]->category);
        free(sales[i]);
    }

    free(sales);
}

//Cleans the momory thats been assigned if an error occurs
void cleanMemoryJson(cJSON *json, char *contentFile) {
    cJSON_Delete(json);
    free(contentFile);

    if(numSales > 0) {
        cleanMemorySales();
    } else {
        if(sales[0] != NULL) {
            free(sales[0]);
        }

        free(sales);
    }
}

//Load the data in the json file to the struct
int loadMemory(char *path, int mode) {
    //Open the file nad read it
    FILE *file = fopen(path, "r");
    
    //Verify that the file was opened
    if(file == NULL) {
        fprintf(stderr, "No se pudo abrir el archivo de %s", path);
        return false;
    }

    //Determine the file length
    fseek(file, 0, SEEK_END);
    long lengthFile = ftell(file);

    //Verify if the file is empty
    if(lengthFile == 0) {
        fclose(file);
        return true;
    }

    fseek(file, 0, SEEK_SET);

    //Reserve the memory for the content
    char *contentFile = malloc(lengthFile + 1);

    //If the content is not empty the program continues
    if(contentFile != NULL) {
        fread(contentFile, 1, lengthFile, file);
        contentFile[lengthFile] = '\0';
    } else {
        fclose(file);
        return false;
    }

    fclose(file);

    //Parse the json file
    cJSON *json = cJSON_Parse(contentFile);

    //Verify if the parsing was done correctly
    if (json == NULL) {
        // Creates a pointer where the error was found
        const char *ptrError = cJSON_GetErrorPtr();

        if (ptrError != NULL) { 
            fprintf(stderr, "Error al analizar el archivo JSON: %s\n", ptrError);
        }

        free(contentFile); 
        return false;
    }

    //Verify if the json File comes with an array structure to prevent errors
    if(!cJSON_IsArray(json)) {
        fprintf(stderr, "El archivo json no viene con el formato de Array esperado.\n");
        cJSON_Delete(json);
        free(contentFile);
        return false;
    }

    //Asigned dynamic memory to the array of the struct
    int arraySize = cJSON_GetArraySize(json);

    if(mode == 0) {
        sales = malloc(arraySize * sizeof(Sale*));

        //Verify if the memory was assigned sucessfully
        if (sales == NULL) {
            fprintf(stderr, "Error al asignar memoria para las ventas.\n");
            cJSON_Delete(json);
            free(contentFile);
            return false;
        }

    } else {
        Sale **tempSales = realloc(sales, arraySize * sizeof(Sale*));

        //Verify if the memory was assigned sucessfully
        if (tempSales == NULL) {
            fprintf(stderr, "Error al re-asignar memoria para la importaci%cn.\n", 162);
            cJSON_Delete(json);
            free(contentFile);
            return false;
        }

        sales = tempSales;
    }

    //Iterates the array to save the content in the structure
    cJSON *item;
    cJSON_ArrayForEach(item, json) {
        //Asigned dynamic memory for each sale instance
        Sale *sale = malloc(sizeof(Sale));

        //Verify if the memory was asigned correctly
        if(sale == NULL) {
            fprintf(stderr, "Error al asignar memoria para las ventas.\n");
            cleanMemoryJson(json, contentFile);
            return false;
        }

        //venta_id
        cJSON *saleId = cJSON_GetObjectItem(item, "venta_id");
        //Verify the type of data to prevent errors
        if(cJSON_IsNumber(saleId)) {
            sale->saleId = saleId->valueint;
        } else {
            fprintf(stderr, "Ocurri%c un error al leer el tipo de dato de venta_id\n", 162);
            cleanMemoryJson(json, contentFile);
            return false;
        }

        //fecha
        cJSON *date = cJSON_GetObjectItem(item, "fecha");
        if(cJSON_IsString(date)) {
            //Asigned dynamic memory for the string
            int lenDate = strlen(date->valuestring);
            sale->date = malloc((lenDate + 1) * sizeof(char));

            //Verify if the memory was asigned correctly
            if (sale->date == NULL) {
                fprintf(stderr, "Error al asignar memoria\n");
                cleanMemoryJson(json, contentFile);
                return false;
            }

            strcpy(sale->date, date->valuestring);
        } else {
            fprintf(stderr, "Ocurri%c un error al leer el tipo de dato de fecha\n", 162);
            cleanMemoryJson(json, contentFile); 
            return false;
        }

        //producto_id
        cJSON *productId = cJSON_GetObjectItem(item, "producto_id");
        if(cJSON_IsNumber(productId)) {
            sale->productId = productId->valueint;
        } else {
            fprintf(stderr, "Ocurri%c un error al leer el tipo de dato de producto_id\n", 162);
            free(sale->date);
            cleanMemoryJson(json, contentFile);
            return false;
        }

        //producto_nombre
        cJSON *productName = cJSON_GetObjectItem(item, "producto_nombre");
        if(cJSON_IsString(productName)) {
            int lenProName = strlen(productName->valuestring);
            sale->productName = malloc((lenProName + 1) * sizeof(char));
            
            if (sale->productName == NULL) {
                fprintf(stderr, "Error al asignar memoria\n");
                free(sale->date);
                cleanMemoryJson(json, contentFile);
                return false;
            }

            strcpy(sale->productName, productName->valuestring);
        } else {
            fprintf(stderr, "Ocurri%c un error al leer el tipo de dato de producto_nombre\n", 162);
            free(sale->date);
            cleanMemoryJson(json, contentFile); 
            return false;
        }

        //categoria
        cJSON *category = cJSON_GetObjectItem(item, "categoria");
        if(cJSON_IsString(category)) {
            int lenCategory = strlen(category->valuestring);
            sale->category = malloc((lenCategory + 1) * sizeof(char));
            
            if (sale->category == NULL) {
                fprintf(stderr, "Error al asignar memoria\n");
                free(sale->date);
                free(sale->productName);
                cleanMemoryJson(json, contentFile);
                return false;
            }

            strcpy(sale->category, category->valuestring);

        } else {
            fprintf(stderr, "Ocurri%c un error al leer el tipo de dato de venta_id\n", 162);
            free(sale->date);
            free(sale->productName);
            cleanMemoryJson(json, contentFile); 
            return false;
        }

        //cantidad
        cJSON *quantity = cJSON_GetObjectItem(item, "cantidad");
        if(cJSON_IsNumber(quantity)) {
            //Save the value in the memory depending on the mode that has been chosen
            if(mode == 1) {
               if((quantity->valueint) == 0) {
                    printf("En la venta con el %d hace falta la cantidad\n", saleId->valueint);
                }
            }

            sale->quantity = quantity->valueint;

        } else {
            if(mode == 0) {
                fprintf(stderr, "Ocurri%c un error al leer el tipo de dato de cantidad\n", 162);
                free(sale->date);
                free(sale->productName);
                free(sale->category);
                cleanMemoryJson(json, contentFile);
                return false;
            } else{
                printf("En la venta con el id %d hace falta la cantidad\n", saleId->valueint);
                sale->quantity = 0;
            }
        }

        //precio_unitario
        cJSON *unitPrice = cJSON_GetObjectItem(item, "precio_unitario");
        if(cJSON_IsNumber(unitPrice)) {
            if(mode == 1) {
                if(unitPrice->valuedouble == 0.0) {
                    printf("En la venta con el id %d hace falta el precio unitario\n", saleId->valueint);
                }
            }

            sale->unitPrice = unitPrice->valuedouble;

        } else {
            if(mode == 0) {
                fprintf(stderr, "Ocurri%c un error al leer el tipo de dato de precio_unitario\n", 162);
                free(sale->date);
                free(sale->productName);
                free(sale->category);
                cleanMemoryJson(json, contentFile);
                return false;
            } else {
                printf("En la venta con el id %d hace falta el precio unitario\n", saleId->valueint);
                sale->unitPrice = 0.0;
            }
        }

        //total
        cJSON *total = cJSON_GetObjectItem(item, "total");
        if(cJSON_IsNumber(total)) {
            if(mode == 1) {
                if(total->valuedouble == 0.0) {
                    printf("En la venta con el id %d hace falta el total\n", saleId->valueint);
                }
            }
            
            sale->total = total->valuedouble;
        } else {
            if(mode == 0) {
                fprintf(stderr, "Ocurri%c un error al leer el tipo de dato de total\n", 162);
                free(sale->date);
                free(sale->productName);
                free(sale->category);
                cleanMemoryJson(json, contentFile); 
                return false;
            } else {
                printf("En la venta con el id %d hace falta el total\n", saleId->valueint);
                sale->total = 0.0;
            }

        }

        //Asigned the instance to the array
        sales[numSales] = sale;
        numSales++;

    }

    //Clean
    cJSON_Delete(json);
    free(contentFile);

    return true;
}

//Cleans the buffer to prevent errors
void cleanBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

//Main Menu
void menu() {
    char o;

    printf("       MEN%c       \n", 233);
    printf("------------------\n");
    printf("A: Importar datos\n");
    printf("B: Procesamiento de datos\n");
    printf("C: An%clisis de datos\n", 160);
    printf("D: An%clisis temporal\n", 160);
    printf("E: Estad%cstica\n", 161);
    printf("S: Salir\n");
    printf("Escriba la opci%cn deseada:\n", 162);

    //Convert the char to uppercase in case the user write it in lowercase
    o = toupper(getchar());
    cleanBuffer();
    printf("\n");

    //Switch to access to the functions of the different options
    switch (o)
    {
    case 'A':
        importData();
        break;
    
    case 'B':
        processData();
        break;

    case 'C':
        analyzeData();
        break;

    case 'D':
        temporalAnalysis();
        break;

    case 'E':
        estadistic();
        break;

    case 'S':
        exitProgram();
        break;
    
    default:
        printf("Opci%cn Invalida. Porfavor vuelva a intentarlo.\n\n", 162);
        menu();
        break;
    }

}

//Imports the data gien a json file to the memory
void importData() {
    //Submenu for the option
    bool continueImport = true;
    
    while(continueImport == true) {
        char o;

        printf("I: Importar datos\n");
        printf("V: Volver\n");
        printf("Escriba la opci%cn deseada:\n", 162);
        
        o = toupper(getchar());
        cleanBuffer();
        printf("\n");

        if(o == 'A') {
            char *path = NULL;
            size_t size = 0;
            bool load  = true;
            ssize_t lengthPath = 0;

            do
            {
                //Ask for the path of the json file
                printf("Porfavor ingrese la ruta del archivo:\n");
                lengthPath = getline(&path, &size, stdin);
                printf("\n");

                //Verify if the path is coorect
                if(path != NULL) {
                    //Peplace the line break for a /0
                    if (path[lengthPath - 1] == '\n') {
                    path[lengthPath - 1] = '\0';
                    }

                    printf("CARGANDO DATOS...\n");
                    load = loadMemory(path, 1);

                    if(load == false) {
                        printf("Ocurri%c un error al importar los datos\n", 162);
                    } else {
                        printf("\nDATOS IMPORTADOS EXITOSAMENTE\n");
                    }

                } else {
                    printf("\n");
                    printf("Ocurri%c un error al leer la ruta.\n", 162);
                }

            } while (path == NULL || load == false);

            free(path);
        } else {
            if(o == 'V') {
                break;
            } else {
                printf("Opci%cn Invalida. Porfavor vuelva a intentarlo.\n\n", 162);
            }
        }

    }

    menu();
}

void processData() {
    printf("Esto es una opcion");
}

void analyzeData() {
    printf("Esto es una opcion");
}

void temporalAnalysis() {
    printf("Esto es una opcion");
}

void estadistic() {
    printf("Esto es una opcion");
}

//Saves the data in memory to a json file for 
void exitProgram() {
    //Creates an array to the json object
    cJSON *jsonArray = cJSON_CreateArray();
    if(jsonArray == NULL) {
        fprintf(stderr, "Error al crear el array JSON.\n");
        printf("Vuelva a intentarlo.\n\n");
        menu();
        return;
    }

    //Iterate the sales array and adds them to the json file
    for(int i = 0; i < numSales; i++){
        //Creates an json object for the sale
        cJSON *jsonSale = cJSON_CreateObject();
        //Verify if the object was create correctly
        if(jsonSale == NULL) {
            cJSON_Delete(jsonArray);
            fprintf(stderr, "Error al crear el objeto JSON para la venta.\n");
            printf("Porfavor vuelvas a intentarlo.\n\n");
            menu();
            return;
        }

        //Adds the data to the json object
        cJSON_AddNumberToObject(jsonSale, "venta_id", sales[i]->saleId);
        cJSON_AddStringToObject(jsonSale, "fecha", sales[i]->date);
        cJSON_AddNumberToObject(jsonSale, "producto_id", sales[i]->productId);
        cJSON_AddStringToObject(jsonSale, "producto_nombre", sales[i]->productName);
        cJSON_AddStringToObject(jsonSale, "categoria", sales[i]->category);
        cJSON_AddNumberToObject(jsonSale, "cantidad", sales[i]->quantity);
        cJSON_AddNumberToObject(jsonSale, "precio_unitario", sales[i]->unitPrice);
        cJSON_AddNumberToObject(jsonSale, "total", sales[i]->total);

        //Adds the json object to the json array
        cJSON_AddItemToArray(jsonArray, jsonSale);
    }

    //Converts the JSON root to a json string
    char *jsonString = cJSON_Print(jsonArray);
    if (jsonString == NULL) {
        cJSON_Delete(jsonArray);
        fprintf(stderr, "Error al convertir el objeto JSON en una cadena.\n");
        printf("Porfavor vuelvas a intentarlo.\n\n");
        menu();
        return;
    }

    //Opens the JSON file
    FILE *jsonFile = fopen("jsonFile/ventas.json", "w");
    if (jsonFile == NULL) {
        perror("Error al abrir el archivo para guardar los datos JSON");
        printf("Porfavor vuelvas a intentarlo.\n\n");
        free(jsonString);
        cJSON_Delete(jsonArray);
        menu();
        return;
    }

    //Writes on the JSON file
    printf("GUARDANDO DATOS...\n");
    fprintf(jsonFile, "%s", jsonString);
    fclose(jsonFile);

    //Cleans memory
    free(jsonString);
    cJSON_Delete(jsonArray);
    cleanMemorySales();

    printf("ESTA SALEINDO...");
    exit(0);
}


/*
C:/Users/maryp/OneDrive/Documentos/TEC/SEMESTRE/LENGUAJES/historicoVentas.json
For more understanding of the cJSON libary this video was use for reference:
https://www.youtube.com/watch?v=0YVrLNhKVc8
https://www.it.uc3m.es/pbasanta/asng/course_notes/input_output_getline_es.html
https://www.geeksforgeeks.org/cjson-json-file-write-read-modify-in-c/
*/
