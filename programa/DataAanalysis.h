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
    float unitPrice;
    float total;
} Sale;

// Global variables
extern Sale **sales;
extern int numSales;

// Function prototypes
int loadMemory(char *path, int mode);
void cleanMemorySales();
void cleanMemoryJson(cJSON *json, char *contentFile);
void cleanBuffer();
void menu();
void importData();
void processData();
void analyzeData();
void temporalAnalysis();
void estadistic();
void exitProgram();

#endif 