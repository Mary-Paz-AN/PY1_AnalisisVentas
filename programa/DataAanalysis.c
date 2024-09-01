#include "DataAanalysis.h"

// Global variables
Sale **sales;
YearlyReport **years;
Category **categories;
int numSales = 0;
int numYearly = 0;
int numCategories = 0;

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


//Memory associate functions
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

//Cleans the memory assigned to the YearlyReport, MonthlyReport and DayReport
void cleanMemoryReports() {
    for(int i = 0; i < numYearly; i++) {

        //Free each element in MonthlyReport
        for(int j = 0; j < years[i]->monthCounter; j++) {
            //Free DayReport
            free(years[i]->months[j].days);
        }

        //Free MonthlyReport
        free(years[i]->months);

        //Free each element in years
        free(years[i]);
    }

    //Free YearlyReport
    free(years);
    years = NULL;
}

//Cleans the memory assigned to the Category
void cleanMemoryCategories() {
    for(int i = 0; i < numCategories; i++) {
        free(categories[i]->name);
        free(categories[i]);
    }

    free(categories);
    categories =NULL;
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
        verifyInfoInMemory();
        processData();
        break;

    case 'C':
        verifyInfoInMemory();
        analyzeData();
        break;

    case 'D':
        verifyInfoInMemory();
        temporalAnalysis();
        break;

    case 'E':
        verifyInfoInMemory();
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

//Verify if there are any import in memory before ejecuting functions other than importData and exit.
void verifyInfoInMemory() {
    if(sales == NULL) {
        printf("Todav%ca no hay importaciones en memoria.\n", 161);
        printf("Haga una y vuelva a intentarlo.\n");
        menu();
    }
}


//Import data functions
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
                if(unitPrice->valueint == 0) {
                    printf("En la venta con el id %d hace falta el precio unitario\n", saleId->valueint);
                }
            }

            sale->unitPrice = unitPrice->valueint;

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
                sale->unitPrice = 0;
            }
        }

        //total
        cJSON *total = cJSON_GetObjectItem(item, "total");
        if(cJSON_IsNumber(total)) {
            if(mode == 1) {
                if(total->valueint == 0) {
                    printf("En la venta con el id %d hace falta el total\n", saleId->valueint);
                }
            }
            
            sale->total = total->valueint;
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
                sale->total = 0;
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

//Imports the data gien a json file to the memory
void importData() {
    //Submenu for the option
    bool continueImport = true;
    
    while(continueImport == true) {
        char o;

        printf("A: Importar datos\n");
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


//Process data functions
void processData() {
    printf("Esto es una opcion");
}


//Day, month, year functions
//Calculate the day of the week in the date
int getDay(const char *date) {
    //Creates a tm structure. Intialize in 0
    struct tm tm = {0};
    
    //Analize the string of date
    sscanf(date, "%d-%d-%d", &tm.tm_year, &tm.tm_mon, &tm.tm_mday);

    //Adjust year and month
    tm.tm_year -= 1900;
    tm.tm_mon -=1;

    //Verify if te conversion works
    time_t t;
    t = mktime(&tm);
    if(t == -1) {
        return -1;
    }

    //Returns the day in index 0 = domingo - 6 = sabado. Add 1, 1 = domingo - 7 = sabado
    return tm.tm_wday + 1;
}

//Calculate the month in the date
int getMonth(const char *date) {
    //Creates a tm structure. Intialize in 0
    struct tm tm = {0};
    
    //Analize the string of date
    sscanf(date, "%d-%d-%d", &tm.tm_year, &tm.tm_mon, &tm.tm_mday);

    return tm.tm_mon;
}

//Calculate the years in the date
int getYear(const char *date) {
    //Creates a tm structure. Intialize in 0
    struct tm tm = {0};
    
    //Analize the string of date
    sscanf(date, "%d-%d-%d", &tm.tm_year, &tm.tm_mon, &tm.tm_mday);

    return tm.tm_year;
}

//Calculates the total of sales per month and year and the quantity of transaccions per day od the week
bool dayMonthYearSales() {   
    //If the structures are not empty free the memory and restart the counter
    if(years != NULL) {
        cleanMemoryReports();
        numYearly = 0;
    }

    //Reserve memory for the structures
    years = malloc(1 * sizeof(YearlyReport*));
    if(years == NULL) {
        printf("Ocurri%c un erorr a asignar memoria.\n", 162);
        printf("Porfavor vuelva a intentarlo.\n");
        return false;
    }

    //Save the data in the structures
    //Iterates each sale to find the years and months
    for(int i = 0; i < numSales; i++) {
        bool yearExists = false;
        int yearIndex = 0;

        //Extract the year from the date
        int yearInt = getYear(sales[i]->date);

        for(int j = 0; j < numYearly; j++) {
            //Verify if the year was already add to the array of years
            if(yearInt == years[j]->year) {
                yearExists = true;
                yearIndex = j;
                break;
            }
        }

        //Adds the year if wasn´t in years
        if(!yearExists) {
            //Reallocate memory for the array
            YearlyReport **tempYears = realloc(years, (numYearly + 1) * sizeof(YearlyReport*));
            if (tempYears == NULL) {
                printf("Ocurrió un error al reasignar memoria.\n");
                printf("Por favor vuelva a intentarlo.\n");
                return false;
            }

            years = tempYears;
            
            //Assigned memory for the structure
            years[numYearly] = malloc(sizeof(YearlyReport));
            if (years[numYearly] == NULL) {
                printf("Ocurrió un error al asignar memoria.\n");
                printf("Por favor vuelva a intentarlo.\n");
                return false;
            }

            //Initialize the data
            yearIndex = numYearly;
            years[yearIndex]->year = yearInt;
            years[yearIndex]->months = NULL;
            years[yearIndex]->monthCounter = 0;
            years[yearIndex]->totalYear = 0;
            numYearly++;
        }

        bool monthExist = false;
        int numMonths =years[yearIndex]->monthCounter;
        int monthIndex = 0;

        //Extract the month from the date 
        int monthInt = getMonth(sales[i]->date);

        //Verify if the month was already add
        for(int j = 0; j < numMonths; j++) {
            //Verify if the year was already add to the array of years
            if(monthInt == years[yearIndex]->months[j].month) {
                monthExist = true;
                monthIndex = j;
                break;
            }
        }

        //Adds the year if wasn´t in years
        if(!monthExist) {
            //Reallocate memory for the array
            MonthlyReport *tempMonths = realloc(years[yearIndex]->months, (numMonths + 1) * sizeof(MonthlyReport));
            if (tempMonths == NULL) {
                printf("Ocurrió un error al reasignar memoria para los meses.\n");
                printf("Por favor vuelva a intentarlo.\n");
                return false;
            }

            years[yearIndex]->months = tempMonths;

            //Adds data to the month array
            monthIndex = numMonths; 
            years[yearIndex]->months[monthIndex].month = monthInt;
            years[yearIndex]->months[monthIndex].totalMonth = 0;
            years[yearIndex]->months[monthIndex].days = NULL;
            years[yearIndex]->months[monthIndex].numDays = 0;
            years[yearIndex]->monthCounter += 1;
        }

        bool dayExists = false;
        int numDays = years[yearIndex]->months[monthIndex].numDays;
        int dayIndex = 0;

        //Extract the day of the week from the date
        int dayInt = getDay(sales[i]->date);

        if(dayInt == -1) {
            return false;
        }

        //Verify if the day already exist
        for (int k = 0; k < numDays; k++) {
            if (dayInt == years[yearIndex]->months[monthIndex].days[k].day) {
                dayExists = true;
                dayIndex = k;
                break;
            }
        }

        // Si no existe, se crea un nuevo DayReport
        if (!dayExists) {
            DayReport *tempDays = realloc(years[yearIndex]->months[monthIndex].days, (numDays + 1) * sizeof(DayReport));
            if (tempDays == NULL) {
                printf("Ocurrió un error al reasignar memoria para los días.\n");
                printf("Por favor vuelva a intentarlo.\n");
                return false;
            }

            years[yearIndex]->months[monthIndex].days = tempDays;

            //Add the data to the days
            dayIndex = numDays; 
            years[yearIndex]->months[monthIndex].days[dayIndex].day = dayInt;
            years[yearIndex]->months[monthIndex].days[dayIndex].numTransaction = 1;
            years[yearIndex]->months[monthIndex].numDays += 1;
        } else{
            years[yearIndex]->months[monthIndex].days[dayIndex].numTransaction += 1;
        }

        //Adds the total made per month and year
        years[yearIndex]->months[monthIndex].totalMonth += sales[i]->total;
        years[yearIndex]->totalYear += sales[i]->total;
    }

    return true;
}

//Converts the number of the month to the name
char *intCharMonth(int month) {
    //Switch month names
    switch (month)
    {
    case 1:
        return "Enero";
        break;

    case 2:
        return "Febrero";
        break;

    case 3:
        return "Marzo";
        break;

    case 4:
        return "Abril";
        break;

    case 5:
        return "Mayo";
        break;

    case 6:
        return "Junio";
        break;
    
    case 7:
        return "Julio";
        break;

    case 8:
        return "Agosto";
        break;

    case 9:
        return "Septiembre";
        break;

    case 10:
        return "Octubre";
        break;

    case 11:
        return "Noviembre";
        break;

    case 12:
        return "Diciembre";
        break;
    
    default:
        break;
    }
}

//Converts the number of the day to the name
char *intCharDay(int day) {
    //Switch month names
    switch (day)
    {
    case 1:
        return "Domingo";
        break;

    case 2:
        return "Lunes";
        break;

    case 3:
        return "Martes";
        break;

    case 4:
        return "Miercoles";
        break;

    case 5:
        return "Jueves";
        break;

    case 6:
        return "Viernes";
        break;
    
    case 7:
        return "Sabado";
        break;

    default:
        break;
    }
}


//Anallyze dat functions
//Calculates the total of sales
int totalSales() {
    int total = 0;

    //Iterates the sales array to find the totalof each sale
    for(int i = 0; i < numSales; i++) {
        total += sales[i]->total;
    }

    return total;
}

//Prints the sale made monthly and yearly
void printReport () {
    printf("------------------------------------------------\n");
    printf("An%clisis de ventas hechas mensual y anualmente\n", 160);
    //Iterates the array of years
    for(int i = 0; i < numYearly; i++) {
        printf("------------------------------------------------\n");

        //Year
        printf("A%co %d\n", 164, years[i]->year);

        //Months
        for(int j = 0; j < years[i]->monthCounter; j++) {
            //Prints the month and the total per month
            printf("  %s: %d\n", intCharMonth(years[i]->months[j].month), years[i]->months[j].totalMonth);
        }

        printf("Total: %d\n", years[i]->totalYear);
    }

    printf("------------------------------------------------\n");
    printf("\n");
}

//Makes different estadistical analyses of sales
void analyzeData() {
    char o;
    int totalSale = 0;
    bool laoadData;

    //Submenu for the option
    printf("A: Ver total de ventas\n");
    printf("B: Ver total de ventas mensuales y anuales\n");
    printf("V: Volver\n");
    printf("Escriba la opci%cn deseada:\n", 162);
    
    o = toupper(getchar());
    cleanBuffer();
    printf("\n");

    //Switch to access to the diferent options
    switch (o)
    {
    case 'A':
        totalSale = totalSales();
        printf("Total de las ventas importadas: %d\n\n", totalSale);
        analyzeData();
        break;
    
    case 'B':
        laoadData = dayMonthYearSales(); 
        if(laoadData){ printReport(); }
        analyzeData();
        break;

    case 'V':
        menu();
        break;

    default:
        printf("Opci%cn Invalida. Porfavor vuelva a intentarlo.\n\n", 162);
        analyzeData();
        break;
    }

}


//Temporal analysis functions
//Calculates and prints the month with most sales in the year that the user ask for
void mostSalesMonth() {
    int year;
    int actualYear;
    int month;
    int total = 0;

    //Gets the actual year
    time_t t = time(NULL);
    struct tm *actual = localtime(&t);
    actualYear = actual->tm_year + 1900;

    //Ask for the year
    printf("Escriba el a%co deseado para el an%clisis:\n", 164, 160);
    scanf("%d", &year);
    
    //Limit of the year
    if(year < 1990 || year > actualYear) {
        printf("El a%co puesto no es valido. Porfavor vuelva a intentarlo.\n", 164);
        return;
    }

    //Iterate the year
    for(int i = 0; i < numYearly; i++) {
        //Verif if the year exist
        if(years[i]->year == year) {
            //Iterates the month array
            for(int j = 0; j < years[i]->monthCounter; j++) {
                //If the total is greater than or equal to replace the values
                if(years[i]->months[j].totalMonth > total) {
                    year = years[i]->year;
                    month = years[i]->months[j].month;
                    total = years[i]->months[j].totalMonth;
                }
            }
        }
    }

    //Print the moth with the most sales in the year
    printf("Mes con mayor venta\n");
    printf("-------------------\n");
    printf("%s %d: %d\n\n", intCharMonth(month), year, total);
}

//Calculates and prints the most active day depending on number of transaccions and quantity
void mostActiveDay() {
    int year;
    int actualYear;
    int month;
    int day;
    int transactions = 0;

    //Gets the actual year
    time_t t = time(NULL);
    struct tm *actual = localtime(&t);
    actualYear = actual->tm_year + 1900;

    //Ask for the year
    printf("Escriba el a%co deseado para el an%clisis:\n", 164, 160);
    scanf("%d", &year);

    //Limit of the year
    if(year < 1900 || year > actualYear) {
        printf("El a%co puesto no es valido. Porfavor vuelva a intentarlo.\n", 164);
        return;
    }

    printf("Escriba el mes deseado para el an%clisis (Escriba el n%cmero 1-12):\n", 160, 163);
    scanf("%d", &month);

    //Limit of the month
    if(month <= 0 || month > 12) {
        printf("El mes puesto no es valido. Porfavor vuelva a intentarlo.\n");
        return;
    }

    //Iterate the year
    for(int i = 0; i < numYearly; i++) {
        //Verify if the year exist
        if(years[i]->year == year) {
            //Iterates the month array
            for(int j = 0; j < years[i]->monthCounter; j++) {
                //Verify if the month exist
                if(years[i]->months[j].month == month) {
                    //Iterates the day array
                    for(int k = 0; k < years[i]->months[j].numDays; k++) {
                        //If the transaction quantity is greater than or equal to replace the values
                        printf("%d", years[i]->months[j].days[k].numTransaction);
                        if(years[i]->months[j].days[k].numTransaction > transactions) {
                            transactions = years[i]->months[j].days[k].numTransaction;
                            day = years[i]->months[j].days[k].day;
                        }
                    }
                }
            }
        }
    }

    printf("Dia de la semana con m%cs activo\n", 160);
    printf("-------------------------------\n");
    printf("%s de %s del %d\n", intCharDay(day), intCharMonth(month), year);
    printf("Cantidad de transacciones: %d\n\n", transactions);
}

//Calculates the rate of growth or decline by trimester
void growthDeclineRate() {
    printf("HELLO OI\n");
}

//Analyzes sales over a specific period of time
void temporalAnalysis() {
    char o;
    bool loadData;

    //Submenu for the option
    printf("A: Mes con mayor venta\n");
    printf("B: Dia de la semana m%cs activo\n", 160);
    printf("C: Tasa de crecimiento o decrecimiento trimestral\n");
    printf("V: Volver\n");
    printf("Escriba la opci%cn deseada:\n", 162);
    
    o = toupper(getchar());
    cleanBuffer();
    printf("\n");

    //Switch to access to the diferent options
    switch (o)
    {
    case 'A':
        //Calculate sales made per year and month
        loadData = dayMonthYearSales();
        if(loadData) { mostSalesMonth(); }
        temporalAnalysis();
        break;
    
    case 'B':
        //Calculate sales made per year and month
        loadData = dayMonthYearSales();
        if(loadData) { mostActiveDay(); }
        temporalAnalysis();
        break;

    case 'C':
        growthDeclineRate();
        temporalAnalysis();
        break;

    case 'V':
        menu();
        break;

    default:
        printf("Opci%cn Invalida. Porfavor vuelva a intentarlo.\n\n", 162);
        temporalAnalysis();
        break;
    }
}


//Estadistic functions
//Prints the top 5 categories 
void printCategories() {
    //If the number of categories is less than 5 then just print numCategories
    printf("Top %d categor%cas m%cs vendidas\n", (numCategories >= 5 ? 5 : numCategories), 161, 160);
    printf("-------------------------------\n");

    for (int i = 0; i < (numCategories >= 5 ? 5 : numCategories); i++) {
        printf("%d. %s: %d\n", (i + 1), categories[i]->name, categories[i]->total);
    }

    printf("\n");
}

//Swap the categories
void swap(Category *a, Category *b) {
    //Temporal variable to save a data
    Category t = *a;

    //Copy the values
    *a = *b;
    *b = t;
}

//Reorders the aray with a pivot
int partition(int low, int high) {
    //The pivot that would be use is the total of the last element
    int pivot = categories[high]->total;
    int i = low - 1;

    //Compares the value with the pivot
    for(int j = low; j < high; j++) {
        if(categories[j]->total > pivot) {
            i++;
            swap(categories[i], categories[j]);
        }
    }

    //Place the pivot in it´s final position
    swap(categories[i + 1], categories[high]);
    return i +1;
}

//Ordes the category array from more earn to less earn with QuickSort
void quickSort(int low, int high) {
    if(low <high) {
        int index = partition(low, high);

        //Orders the left sublist
        quickSort(low, index -1);

        //Orders the rigth sublist
        quickSort(index + 1, high);
    }
}

//Creates a array to know the total for each category
bool salesXCategory() {
    //If the structures are not empty free the memory and restart the counters
    if(categories != NULL) {
        cleanMemoryCategories();
        numCategories = 0;
    }

    //Reserve memory for the structures
    categories = malloc(1 * sizeof(Category*));
    if(categories == NULL) {
        printf("Ocurri%c un erorr a asignar memoria.\n", 162);
        printf("Porfavor vuelva a intentarlo.\n");
        return false;
    }

    //Save the data in the structures
    //Iterates each sale to find the categories
    for(int i = 0; i < numSales; i++) {
        bool categoryExist = false;
        int categoryIndex = 0;

        for(int j = 0; j < numCategories; j++) {
            //Verify if the category was already add to the array of categories
            if(strcmp(sales[i]->category, categories[j]->name) == 0) {
                categoryExist = true;
                categoryIndex = j;
                break;
            }
        }

        //Adds the year if wasn´t in years
        if(!categoryExist) {
            //Reallocate memory for the array
            Category **tempCategories = realloc(categories, (numCategories + 1) * sizeof(Category*));
            if (tempCategories == NULL) {
                printf("Ocurrió un error al reasignar memoria.\n");
                printf("Por favor vuelva a intentarlo.\n");
                return false;
            }

            categories = tempCategories;
            
            //Assigned memory for the structure
            categories[numCategories] = malloc(sizeof(Category));
            if (categories[numCategories] == NULL) {
                printf("Ocurrió un error al asignar memoria.\n");
                printf("Por favor vuelva a intentarlo.\n");
                return false;
            }

            //Assigned memory to the name
            categories[numCategories]->name = malloc((strlen(sales[i]->category) + 1) * sizeof(char));


            //Initialize the data
            categoryIndex = numCategories;
            strcpy(categories[categoryIndex]->name, sales[i]->category);
            categories[categoryIndex]->total = 0;
            numCategories++;
        }

        //Adds the total made per category
        categories[categoryIndex]->total += sales[i]->total;
    }

    return true;
}

//Prints the top 5 categories that have been sale
void estadistic() {
    //Sub menu
    char o;
    bool loadData;

    printf("A: Top 5 categor%cas\n", 161);
    printf("V: Volver\n");
    printf("Escriba la opci%cn deseada:\n", 162);
    
    o = toupper(getchar());
    cleanBuffer();
    printf("\n");

    //Switch for different options
    switch (o)
    {
    case 'A':
        loadData = salesXCategory();
        if(loadData) {
            quickSort(0, numCategories - 1);
            printCategories();
        }
        estadistic();
        break;
    case 'V':
        menu();

    default:
        printf("Opci%cn Invalida. Porfavor vuelva a intentarlo.\n\n", 162);
        estadistic();
        break;
    }
    
}


//Exit
//Saves the data in memory to a json file for 
void exitProgram() {
    if(sales == NULL) {
        exit(0);
    }

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
    cleanMemoryReports();
    cleanMemorySales();
    cleanMemoryCategories();

    printf("ESTA SALEINDO...");
    exit(0);
}


/*
C:/Users/maryp/OneDrive/Documentos/TEC/SEMESTRE/LENGUAJES/historicoVentas.json
For more understanding of the cJSON libary this video was use for reference:
https://www.youtube.com/watch?v=0YVrLNhKVc8
https://www.it.uc3m.es/pbasanta/asng/course_notes/input_output_getline_es.html
https://www.geeksforgeeks.org/cjson-json-file-write-read-modify-in-c/
https://www.geeksforgeeks.org/quick-sort-algorithm/
https://nextscenario.com/es/formula-del-porcentaje-de-crecimiento-un-analisis-detallado/#:~:text=La%20f%C3%B3rmula%20para%20calcular%20el,100%20para%20obtener%20el%20porcentaje.
*/
