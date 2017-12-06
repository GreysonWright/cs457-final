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
#include "Integer.h"

struct DATABASE {
	int fieldCount;
	DArray *store;
	DocumentStore *documentStore;
	void (*display)(FILE *, void *);
};

char *buildKeyValuePair(char *, int);
int countIntegers(int);

DataBase *newDataBase(void (*display)(FILE *file, void *value)) {
	DataBase *dataBase = malloc(sizeof *dataBase);
	dataBase->fieldCount = 0;
	dataBase->store = newDArray(display);
	dataBase->documentStore = newDocumentStore(display);
	dataBase->display = display;
	return dataBase;
}

void insertDataBase(DataBase *dataBase, char *fields) {
	Integer *docID = parseInteger(fields, "DocID");
	Document *document = insertDocumentStore(dataBase->documentStore, getInteger(docID));
	int vn = getCountDocument(document);
	Record *record = newRecord(fields);
	char *vnString = buildKeyValuePair("vn", vn);
	char *sysIDString = buildKeyValuePair("sysid", dataBase->fieldCount);
	appendFieldRecord(record, vnString);
	appendFieldRecord(record, sysIDString);
	insertDArray(dataBase->store, record);
	dataBase->fieldCount++;
}

char *buildKeyValuePair(char *key, int value) {
	char *keyValue = malloc(strlen(key) + countIntegers(value) + 2);
	sprintf(keyValue, "%s:%d", key, value);
	return keyValue;
}

int countIntegers(int value) {
	if (value == 0) {
		return 0;
	}
	return 1 + countIntegers(value / 10);
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
	DArray *resultArray = newDArray(dataBase->display);
	for (int i = 0; i < sizeDArray(dataBase->store); i++) {
		Record *record = getDArray(dataBase->store, i);
		if (strstr(getRecord(record), searchVal)) {
			insertDArray(resultArray, record);
		}
	}
	return resultArray;
}

void displayDataBase(FILE *outFile, DataBase *dataBase) {
	for (int i = 0; i < sizeDArray(dataBase->store); i++) {
		Record *record = getDArray(dataBase->store, i);
		printf("%s\n", getRecord(record));
	}
}
