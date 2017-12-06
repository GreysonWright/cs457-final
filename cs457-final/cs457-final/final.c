//
//  main.c
//  cs457-final
//
//  Created by Greyson Wright on 12/2/17.
//  Copyright © 2017 Greyson Wright. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "darray.h"
#include "Record.h"
#include "Integer.h"
#include "Document.h"
#include "DocumentStore.h"

void insert(DArray *, char *);
Integer *parse(char *, char *);
DArray *andQuery(DArray *darray, char *searchVal);
DArray *rangedQuery(DArray *, char *);
DArray *query(DArray *, char *);
void writeResults(DArray *, char*);

int main(int argc, const char * argv[]) {
	DArray *store = newDArray(displayRecord);
	char *searchVal = "";
	char *resultFields = "";
//	DArray *results = query(store, searchVal);
//	writeResults(results, resultFields);
	printf("\n");
	return 0;
}

void insert(DArray *store, char *fields) {
	Record *record = newRecord(fields);
	insertDArray(store, record);
}

Integer *parse(char *source, char *key) {
	char *token = malloc(strlen(source));
	strcpy(token, source);
	
	char *pkey = strtok(token, ":");
	char *pval = strtok(0, " ");
	
	while (pkey) {
		if (strcmp(pkey, key) == 0) {
			return newInteger(atoi(pval));
		}
		pkey = strtok(0, ":");
		pval = strtok(0, " ");
	}
	
	return nullInteger();
}

DArray *andQuery(DArray *darray, char *searchVal) {
	DArray *resultArray = newDArray(0);
	return resultArray;
}

DArray *rangedQuery(DArray *darray, char *searchVal) {
	DArray *resultArray = newDArray(0);
	//	for (int i = 0; i < sizeDArray(darray); i++) {
	//		Record *record = getDArray(darray, i);
	//		if (strstr(record->fields, searchVal)) {
	//			insertDArray(resultArray, record);
	//		}
	//	}
	return resultArray;
}

DArray *query(DArray *darray, char *searchVal) {
	DArray *resultArray = newDArray(0);
	for (int i = 0; i < sizeDArray(darray); i++) {
		Record *record = getDArray(darray, i);
		if (strstr(getRecord(record), searchVal)) {
			insertDArray(resultArray, record);
		}
	}
	return resultArray;
}

void writeResults(DArray *results, char *fields) {
	for (int i = 0; i < sizeDArray(results); i++) {
		Record *record = getDArray(results, i);
		printf("%s\n", getRecord(record));
	}
}
