//
//  DataBase.c
//  cs457-final
//
//  Created by Greyson Wright on 12/6/17.
//  Copyright © 2017 Greyson Wright. All rights reserved.
//

#include <stdlib.h>
#include <string.h>
#include "DataBase.h"
#include "Record.h"
#include "Document.h"
#include "DocumentStore.h"

struct DATABASE {
	DArray *store;
};

DataBase *newDataBase(void (*display)(FILE *file, void *value)) {
	DataBase *dataBase = malloc(sizeof *dataBase);
	dataBase->store = newDArray(display);
	return dataBase;
}

void insertDataBase(DataBase *dataBase, char *fields) {
	Record *record = newRecord(fields);
	insertDArray(dataBase->store, record);
}

DArray *andQueryDataBase(DataBase *dataBase, char *searchVal) {
	DArray *resultArray = newDArray(0);
	return resultArray;
}

DArray *rangedQueryDataBase(DataBase *dataBase, char *searchVal) {
	DArray *resultArray = newDArray(0);
	//	for (int i = 0; i < sizeDArray(darray); i++) {
	//		Record *record = getDArray(darray, i);
	//		if (strstr(record->fields, searchVal)) {
	//			insertDArray(resultArray, record);
	//		}
	//	}
	return resultArray;
}

DArray *queryDataBase(DataBase *dataBase, char *searchVal) {
	DArray *resultArray = newDArray(0);
	for (int i = 0; i < sizeDArray(dataBase->store); i++) {
		Record *record = getDArray(dataBase->store, i);
		if (strstr(getRecord(record), searchVal)) {
			insertDArray(resultArray, record);
		}
	}
	return resultArray;
}

void displayDataBase(FILE *outFile, DataBase *dataBase, char *fields) {
	for (int i = 0; i < sizeDArray(dataBase->store); i++) {
		Record *record = getDArray(dataBase->store, i);
		printf("%s\n", getRecord(record));
	}
}
