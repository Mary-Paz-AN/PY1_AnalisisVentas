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
    char *month;
    int totalMonth;
} MonthlyReport;

typedef struct 
{
    char *year;
    MonthlyReport reportM;
    int totalYear;
} YearlyReport;

// Global variables
extern Sale **sales;
extern MonthlyReport **reportM;
extern YearlyReport **reportY;
extern int numSales;
extern int numYearly;
extern int numMonthly;

// Function prototypes
void cleanMemorySales();
void cleanMemoryJson(cJSON *json, char *contentFile);
void cleanBuffer();
int loadMemory(char *path, int mode);
void menu();
void importData();
void processData();
int totalSales();
void monthlyYearlySales();
void analyzeData();
void temporalAnalysis();
void estadistic();
void exitProgram();

#endif 