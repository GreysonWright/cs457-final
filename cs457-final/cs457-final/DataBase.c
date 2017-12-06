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
int isRangedQuery(char *);
int isAndQuery(char *);
char *flattenRange(char *);
char *getKey(char *);
char *findKeyValue(char *, char *);
DArray *splitFields(Record *record);
DArray *andQuery(DataBase *darray, char *searchVal);
DArray *rangedQuery(DataBase *, char *);
DArray *basicQuery(DataBase *, char*);

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

DArray *queryDataBase(DataBase *dataBase, char *query) {
	if (isRangedQuery(query) && isAndQuery(query)) {
		return 0; //******
	}
	if (isRangedQuery(query)) {
		return rangedQuery(dataBase, query);
	}
	if (isAndQuery(query)) {
		return andQuery(dataBase, query);
	}
	return basicQuery(dataBase, query);
}

int isRangedQuery(char *query) {
	return strstr(query, ">") || strstr(query, "<");
}

int isAndQuery(char *query) {
	return strstr(query, ",") != 0;
}

DArray *andQuery(DataBase *dataBase, char *query) {
	DArray *resultArray = newDArray(dataBase->display);
	return resultArray;
}

DArray *rangedQuery(DataBase *dataBase, char *query) {
	DArray *resultArray = newDArray(dataBase->display);
	if (strstr(query, "<=")) {
		for (int i = 0; i < sizeDArray(dataBase->store); i++) {
			Record *record = getDArray(dataBase->store, i);
			char *queryKeyValue = flattenRange(query);
			char *queryKey = getKey(queryKeyValue);
			char *recordKeyValue = findKeyValue(getRecord(record), queryKey);
			if (recordKeyValue && strcmp(recordKeyValue, queryKeyValue) <= 0) {
				insertDArray(resultArray, record);
			}
		}
	} else if (strstr(query, ">=")) {
		for (int i = 0; i < sizeDArray(dataBase->store); i++) {
			Record *record = getDArray(dataBase->store, i);
			char *queryKeyValue = flattenRange(query);
			char *queryKey = getKey(queryKeyValue);
			char *recordKeyValue = findKeyValue(getRecord(record), queryKey);
			if (recordKeyValue && strcmp(recordKeyValue, queryKeyValue) >= 0) {
				insertDArray(resultArray, record);
			}
		}
	} else if (strstr(query, "<")) {
		for (int i = 0; i < sizeDArray(dataBase->store); i++) {
			Record *record = getDArray(dataBase->store, i);
			char *queryKeyValue = flattenRange(query);
			char *queryKey = getKey(queryKeyValue);
			char *recordKeyValue = findKeyValue(getRecord(record), queryKey);
			if (recordKeyValue && strcmp(recordKeyValue, queryKeyValue) < 0) {
				insertDArray(resultArray, record);
			}
		}
	} else {
		for (int i = 0; i < sizeDArray(dataBase->store); i++) {
			Record *record = getDArray(dataBase->store, i);
			char *queryKeyValue = flattenRange(query);
			char *queryKey = getKey(queryKeyValue);
			char *recordKeyValue = findKeyValue(getRecord(record), queryKey);
			if (recordKeyValue && strcmp(recordKeyValue, queryKeyValue) > 0) {
				insertDArray(resultArray, record);
			}
		}
	}
	return resultArray;
}

char *flattenRange(char *source) {
	char *keyValue = malloc(strlen(source));
	int count = 0;
	for (int i = 0; i < strlen(source); i++) {
		if (source[i] == '>' || source[i] == '<') {
			keyValue[count++] = ':';
		} else if (source[i] != '=') {
			keyValue[count++] = source[i];
		}
	}
	return keyValue;
}

char *getKey(char *source) {
	char *token = malloc(strlen(source));
	strcpy(token, source);
	
	char *key = strtok(token, ":");
	return key;
}

char *findKeyValue(char *source, char *key) {
	char *token = malloc(strlen(source));
	strcpy(token, source);
	
	char *keyValue = strtok(token, " ");
	
	while (keyValue) {
		if (strstr(keyValue, key)) {
			return keyValue;
		}
		keyValue = strtok(0, " ");
	}
	
	return 0;
}

DArray *basicQuery(DataBase *dataBase, char *query) {
	DArray *resultArray = newDArray(dataBase->display);
	for (int i = 0; i < sizeDArray(dataBase->store); i++) {
		Record *record = getDArray(dataBase->store, i);
		if (strstr(getRecord(record), query)) {
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
