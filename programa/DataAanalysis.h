#ifndef DATAANALYSIS_H
#define DATAANALYSIS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h> 
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
    int month;
    int totalMonth;
} MonthlyReport;

typedef struct 
{
    int year;
    MonthlyReport *months;
    int monthCounter;
    int totalYear;
} YearlyReport;

// Global variables
extern Sale **sales;
extern YearlyReport **years;
extern int numSales;
extern int numYearly;

//Program Functions
//Memory associate functions
void cleanMemorySales();
void cleanMemoryReports();
void cleanMemoryJson(cJSON *json, char *contentFile);
int loadMemory(char *path, int mode);
void cleanBuffer();
void menu();
void importData();
void processData();
int totalSales();
bool monthlyYearlySales();
void printReport();
void analyzeData();
void temporalAnalysis();
void estadistic();
void exitProgram();

#endif