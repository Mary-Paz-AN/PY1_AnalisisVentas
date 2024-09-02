#ifndef DATAANALYSIS_H
#define DATAANALYSIS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h> 
#include <time.h>
#include <math.h>
#include <sys/types.h>
#include "cJSONLibary/cJSON.h"

//Global Struct
typedef struct 
{
    int saleId;
    char *date;
    int productId;
    char *productName;
    char *category;
    int quantity;
    int unitPrice;
    int total;
} Sale;

typedef struct 
{
    int day;
    int numTransaction;
} DayReport;

typedef struct 
{
    int month;
    DayReport *days;
    int numDays;
    int totalMonth;
} MonthlyReport;

typedef struct 
{
    int year;
    MonthlyReport *months;
    int monthCounter;
    int totalYear;
} YearlyReport;

typedef struct 
{
    char *name;
    int total;
} Category;

// Global variables
extern Sale **sales;
extern YearlyReport **years;
extern Category **categories;
extern int numSales;
extern int numYearly;
extern int numCategories;

//Program Functions
//Memory associate functions
void cleanMemorySales();
void cleanMemoryReports();
void cleanMemoryCategories();
void cleanMemoryJson(cJSON *json, char *contentFile);
void cleanBuffer();

//Menu
void menu();
void verifyInfoInMemory();

//Import data functions
int loadMemory(char *path, int mode);
void importData();

//Proceess data functions
void deleteDuplicates();
int mode();
int average();
void missingData();
void processData();

//Day, month, year functions
int getDay(const char *date);
int getMonth(const char *date);
int getYear(const char *date);
bool dayMonthYearSales();
char *intCharMonth(int month);
char *intCharDay(int day);

//Analyze data functions
int totalSales();
void printReport();
void analyzeData();

//Temporal analysis functions
void mostSalesMonth();
void mostActiveDay();
void growthDeclineRate();
void temporalAnalysis();

//Estadistic functions
void printCategories();
void swap(Category *a, Category *b);
int partition(int low, int high);
void quickSort(int low, int high);
bool salesXCategory();
void estadistic();

//Exit 
void exitProgram();

#endif

bool dayMonthYearSales() {   
    //If the structures are not empty, free the memory and restart the counter
    if(years != NULL) {
        cleanMemoryReports();
        numYearly = 0;
    }

    //Reserve memory for the structures
    years = malloc(1 * sizeof(YearlyReport*));
    if(years == NULL) {
        printf("Ocurri%c un error al asignar memoria.\n", 162);
        printf("Por favor vuelva a intentarlo.\n");
        return false;
    }

    //Save the data in the structures
    //Iterate through each sale to find the years and months
    for(int i = 0; i < numSales; i++) {
        bool yearExists = false;
        int yearIndex = 0;

        //Extract the year from the date
        int yearInt = getYear(sales[i]->date);

        for(int j = 0; j < numYearly; j++) {
            //Verify if the year was already added to the array of years
            if(yearInt == years[j]->year) {
                yearExists = true;
                yearIndex = j;
                break;
            }
        }

        //Add the year if it wasn’t in the array of years
        if(!yearExists) {
            //Reallocate memory for the array
            YearlyReport **tempYears = realloc(years, (numYearly + 1) * sizeof(YearlyReport*));
            if (tempYears == NULL) {
                printf("Ocurrió un error al reasignar memoria.\n");
                printf("Por favor vuelva a intentarlo.\n");
                return false;
            }

            years = tempYears;

            //Assign memory for the structure
            years[numYearly] = malloc(sizeof(YearlyReport));
            if (years[numYearly] == NULL) {
                printf("Ocurrió un error al asignar memoria.\n");
                printf("Por favor vuelva a intentarlo.\n");
                return false;
            }

            //Initialize the data
            yearIndex = numYearly;
            years[yearIndex]->year = yearInt;
            years[yearIndex]->months = malloc(12 * sizeof(MonthlyReport));
            if (years[yearIndex]->months == NULL) {
                printf("Ocurrió un error al asignar memoria para los meses.\n");
                printf("Por favor vuelva a intentarlo.\n");
                return false;
            }

            //Initialize the array for each month 1
            for (int m = 0; m < 12; m++) {
                years[yearIndex]->months[m].month = m + 1;
                years[yearIndex]->months[m].totalMonth = 0;
                years[yearIndex]->months[m].days = NULL;
                years[yearIndex]->months[m].numDays = 0;
            }

            years[yearIndex]->monthCounter = 12;
            years[yearIndex]->totalYear = 0;
            numYearly++;
        }

        //Gets the month to adjust the data for each month
        int monthIndex = getMonth(sales[i]->date) - 1;

        // Extract the day of the week from the date
        int dayInt = getDay(sales[i]->date);
        if(dayInt == -1) {
            return false;
        }

        bool dayExists = false;
        int numDays = years[yearIndex]->months[monthIndex].numDays;
        int dayIndex = 0;

        // Verify if the day already exists
        for (int k = 0; k < numDays; k++) {
            if (dayInt == years[yearIndex]->months[monthIndex].days[k].day) {
                dayExists = true;
                dayIndex = k;
                break;
            }
        }

        // If the day doesn't exist, create a new DayReport
        if (!dayExists) {
            DayReport *tempDays = realloc(years[yearIndex]->months[monthIndex].days, (numDays + 1) * sizeof(DayReport));
            if (tempDays == NULL) {
                printf("Ocurrió un error al reasignar memoria para los días.\n");
                printf("Por favor vuelva a intentarlo.\n");
                return false;
            }

            years[yearIndex]->months[monthIndex].days = tempDays;

            // Add the data to the days
            dayIndex = numDays;
            years[yearIndex]->months[monthIndex].days[dayIndex].day = dayInt;
            years[yearIndex]->months[monthIndex].days[dayIndex].numTransaction = 1;
            years[yearIndex]->months[monthIndex].numDays += 1;
        } else {
            years[yearIndex]->months[monthIndex].days[dayIndex].numTransaction += 1;
        }

        // Add the total made per month and year
        years[yearIndex]->months[monthIndex].totalMonth += sales[i]->total;
        years[yearIndex]->totalYear += sales[i]->total;
    }

    return true;
}