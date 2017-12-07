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
DArray *separateQueries(char *source);
int doesDarrayContainKeyValue(DArray *, char *);
char *convertToKeyValue(char *source);
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
	if (isAndQuery(query)) {
		return andQuery(dataBase, query);
	}
	if (isRangedQuery(query)) {
		return rangedQuery(dataBase, query);
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
	char *token = malloc(strlen(query));
	strcpy(token, query);
	
	DArray *keyValues = separateQueries(query);
	for (int i = 0; i < sizeDArray(keyValues); i++) {
		char *keyValue = getDArray(keyValues, i);
		DArray *tmp = newDArray(dataBase->display);
		if (isRangedQuery(keyValue)) {
			tmp = rangedQuery(dataBase, keyValue);
		} else {
			keyValue = convertToKeyValue(keyValue);
			tmp = basicQuery(dataBase, keyValue);
		}
		
		for (int i = 0; i < sizeDArray(tmp); i++) {
			insertDArray(resultArray, getDArray(tmp, i));
		}
	}
	
	for (int i = 0; i < sizeDArray(resultArray); i++) {
		Record *iRecord = getDArray(resultArray, i);
		Integer *isysID = parseInteger(getRecord(iRecord), "sysid");
		for (int j = i + 1; j < sizeDArray(resultArray); j++) {
			Record *jRecord = getDArray(resultArray, j);
			Integer *jsysID = parseInteger(getRecord(jRecord), "sysid");
			printf("i:%d j:%d\n", getInteger(isysID), getInteger(jsysID));
			if (compareInteger(isysID, jsysID) == 0 && j != i) {
				markAsDuplicateRecord(jRecord);
			}
		}
	}
	
	DArray *newResultArray = newDArray(dataBase->display);
	for (int i = 0; i < sizeDArray(resultArray); i++) {
		Record *record = getDArray(resultArray, i);
		char *keyValue = findKeyValue(getRecord(record), "sysid");
		if (getIsDuplicateRecord(record) && !doesDarrayContainKeyValue(newResultArray, keyValue)) {
			insertDArray(newResultArray, record);
		}
	}
	
	return newResultArray;
}

DArray *separateQueries(char *source) {
	char *token = malloc(strlen(source));
	strcpy(token, source);
	
	DArray *resultArray = newDArray(0);
	char *keyValue = strtok(token, ",");
	while (keyValue) {
		insertDArray(resultArray, keyValue);
		keyValue = strtok(0, ",");
	}
	
	return resultArray;
}

int doesDarrayContainKeyValue(DArray *darray, char *keyValue) {
	for (int i = 0; i < sizeDArray(darray); i++) {
		Record *record = getDArray(darray, i);
		if (strstr(getRecord(record), keyValue)) {
			return 1;
		}
	}
	return 0;
}

char *convertToKeyValue(char *source) {
	char *keyValue = malloc(strlen(source));
	int count = 0;
	for (int i = 0; i < strlen(source); i++) {
		if (source[i] == '=') {
			keyValue[count++] = ':';
		} else {
			keyValue[count++] = source[i];
		}
	}
	return keyValue;
}

DArray *rangedQuery(DataBase *dataBase, char *query) {
	DArray *resultArray = newDArray(dataBase->display);
	if (strstr(query, "<=")) {
		for (int i = 0; i < sizeDArray(dataBase->store); i++) {
			Record *record = getDArray(dataBase->store, i);
			char *queryKeyValue = flattenRange(query);
			char *queryKey = getKey(queryKeyValue);
			char *recordKeyValue = findKeyValue(getRecord(record), queryKey);
			if (!recordKeyValue || (recordKeyValue && strcmp(recordKeyValue, queryKeyValue) <= 0)) {
				insertDArray(resultArray, record);
			}
		}
	} else if (strstr(query, ">=")) {
		for (int i = 0; i < sizeDArray(dataBase->store); i++) {
			Record *record = getDArray(dataBase->store, i);
			char *queryKeyValue = flattenRange(query);
			char *queryKey = getKey(queryKeyValue);
			char *recordKeyValue = findKeyValue(getRecord(record), queryKey);
			if (!recordKeyValue || (recordKeyValue && strcmp(recordKeyValue, queryKeyValue) >= 0)) {
				insertDArray(resultArray, record);
			}
		}
	} else if (strstr(query, "<")) {
		for (int i = 0; i < sizeDArray(dataBase->store); i++) {
			Record *record = getDArray(dataBase->store, i);
			char *queryKeyValue = flattenRange(query);
			char *queryKey = getKey(queryKeyValue);
			char *recordKeyValue = findKeyValue(getRecord(record), queryKey);
			if (!recordKeyValue || (recordKeyValue && strcmp(recordKeyValue, queryKeyValue) < 0)) {
				insertDArray(resultArray, record);
			}
		}
	} else {
		for (int i = 0; i < sizeDArray(dataBase->store); i++) {
			Record *record = getDArray(dataBase->store, i);
			char *queryKeyValue = flattenRange(query);
			char *queryKey = getKey(queryKeyValue);
			char *recordKeyValue = findKeyValue(getRecord(record), queryKey);
			if (!recordKeyValue || (recordKeyValue && strcmp(recordKeyValue, queryKeyValue) > 0)) {
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
		char *key = getKey(query);
		if (strstr(getRecord(record), query) || strstr(getRecord(record), key) == 0) {
			insertDArray(resultArray, record);
		}
	}
	return resultArray;
}

int countDataBase(DataBase *dataBase, char *query) {
	DArray *resultsArray = basicQuery(dataBase, query);
	return sizeDArray(resultsArray);
}

void displayDataBase(FILE *outFile, DataBase *dataBase) {
	for (int i = 0; i < sizeDArray(dataBase->store); i++) {
		Record *record = getDArray(dataBase->store, i);
		printf("%s\n", getRecord(record));
	}
}
