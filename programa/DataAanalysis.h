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
