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

typedef struct record {
	char *fields;
} Record;

Record *newRecord(char *fields) {
	Record *record = malloc(sizeof *record);
	record->fields = fields;
	return record;
}

int compare(Record *left, Record *right) {
	return strcmp(left->fields, right->fields);
}

DArray *query(DArray *, char *);
void displayResults(DArray *);

int main(int argc, const char * argv[]) {
	DArray *store = newDArray(0);
	
	char *searchVal = "";
	DArray *results = query(store, searchVal);
	displayResults(results);
	printf("\n");
	return 0;
}

DArray *query(DArray *darray, char *searchVal) {
	DArray *resultArray = newDArray(0);
	for (int i = 0; i < sizeDArray(darray); i++) {
		Record *record = getDArray(darray, i);
		if (strstr(record->fields, searchVal)) {
			insertDArray(resultArray, record);
		}
	}
	return resultArray;
}

void displayResults(DArray *results) {
	for (int i = 0; i < sizeDArray(results); i++) {
		Record *record = getDArray(results, i);
		printf("%s\n", record->fields);
	}
}
