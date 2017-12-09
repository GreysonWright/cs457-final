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
DArray *separateFields(char *);
int doesDarrayContainKeyValue(DArray *, char *);
char *convertToKeyValue(char *);
char *flattenRange(char *);
char *getKey(char *);
char *findKeyValue(char *, char *);
char *stripNotEqualOp(char *);
int min(int, int);
void sortDarray(DArray *, char *);
DArray *filterVersion(DArray *, DocumentStore *, int, void (*)(FILE *, void *));
DArray *searchDataBase(DataBase *, char *);
DArray *searchDarray(DArray *, char *query, void (*)(FILE *, void *));
DArray *splitFields(Record *);
DArray *andQuery(DataBase *, char *);
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

DArray *queryDataBase(DataBase *dataBase, char *query, int version) {
	DArray *results = 0;
	if (isAndQuery(query)) {
		results = andQuery(dataBase, query);
	}
	if (isRangedQuery(query)) {
		results = rangedQuery(dataBase, query);
	}
	results = basicQuery(dataBase, query);
	
	if (version > 0) {
		results = filterVersion(results, dataBase->documentStore, version, dataBase->display);
	}
	sortDarray(results, "vn");
	return results;
}

int isRangedQuery(char *query) {
	return !strstr(query, "<>") && (strstr(query, ">") || strstr(query, "<"));
}

int isAndQuery(char *query) {
	return strstr(query, ",") != 0;
}

DArray *andQuery(DataBase *dataBase, char *query) {
	DArray *resultArray = newDArray(dataBase->display);
	char *token = malloc(strlen(query));
	strcpy(token, query);
	
	DArray *keyValues = separateFields(query);
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

DArray *separateFields(char *source) {
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
	if (strstr(query, "=")) {
		query = convertToKeyValue(query);
	}
	DArray *resultArray = newDArray(dataBase->display);
	for (int i = 0; i < sizeDArray(dataBase->store); i++) {
		Record *record = getDArray(dataBase->store, i);
		char *key = getKey(query);
		if (!strstr(query, "<>")) {
			if (strstr(getRecord(record), query) || !strstr(getRecord(record), key)) {
				insertDArray(resultArray, record);
			}
		} else {
			char *strippedQuery = stripNotEqualOp(query);
			if (!strstr(getRecord(record), strippedQuery)) {
				insertDArray(resultArray, record);
			}
		}
	}
	return resultArray;
}

char *stripNotEqualOp(char *source) {
	char *keyValue = malloc(strlen(source));
	int count = 0;
	for (int i = 0; i < strlen(source); i++) {
		if (source[i] == '<') {
			keyValue[count++] = ':';
		} else if (source[i] != '>') {
			keyValue[count++] = source[i];
		}
	}
	return keyValue;
}

int countDataBase(DataBase *dataBase, char *query, int version) {
	if (version > 0) {
		int count = 0;
		for (int i = 0; i < sizeDocumentStore(dataBase->documentStore); i++) {
			Document *document = getDocumentStore(dataBase->documentStore, i);
			int docID = getIDDocument(document);
			char *keyValue = buildKeyValuePair("DocID", docID);
			DArray *results = searchDataBase(dataBase, keyValue);
			count += min(sizeDArray(results), version);
		}
		
		return count;
	}
	
	DArray *results = searchDataBase(dataBase, query);
	return sizeDArray(results);
}

DArray *searchDataBase(DataBase *dataBase, char *query) {
	DArray *resultArray = searchDarray(dataBase->store, query, dataBase->display);
	return resultArray;
}

DArray *searchDarray(DArray *darray, char *query, void (*display)(FILE *, void *)) {
	DArray *resultArray = newDArray(display);
	for (int i = 0; i < sizeDArray(darray); i++) {
		Record *record = getDArray(darray, i);
		if (strstr(getRecord(record), query)) {
			insertDArray(resultArray, record);
		}
	}
	return resultArray;
}

int min(int a, int b) {
	return a < b ? a : b;
}

DArray *sortDataBase(DataBase *dataBase, char *field, int version) {
	DArray *resultsArray = searchDataBase(dataBase, field);
	if (version > 0) {
		resultsArray = filterVersion(resultsArray, dataBase->documentStore, version, dataBase->display);
	}
	sortDarray(resultsArray, field);
	return resultsArray;
}

void sortDarray(DArray *darray, char *field) {
	for (int i = 0; i < sizeDArray(darray) - 1; i++) {
		for (int j = 0; j < sizeDArray(darray) - i - 1; j++) {
			Record *currentRecord = getDArray(darray, j);
			Integer *currentValue = parseInteger(getRecord(currentRecord), field);
			Record *nextRecord = getDArray(darray, j + 1);
			Integer *nextValue = parseInteger(getRecord(nextRecord), field);
			if (compareInteger(nextValue, currentValue) > 0) {
				setDArray(darray, j, nextRecord);
				setDArray(darray, j + 1, currentRecord);
			}
		}
	}
}

DArray *filterVersion(DArray *store, DocumentStore *documentStore, int version, void (*display)(FILE *, void *)) {
	DArray *newResults = newDArray(display);
	for (int i = 0; i < sizeDocumentStore(documentStore); i++) {
		Document *document = getDocumentStore(documentStore, i);
		int docID = getIDDocument(document);
		char *keyValue = buildKeyValuePair("DocID", docID);
		DArray *resultArray = searchDarray(store, keyValue, display);
		sortDarray(resultArray, "DocID");
		sortDarray(resultArray, "vn");
		for (int j = 0; j < min(version, sizeDArray(resultArray)); j++) {
			insertDArray(newResults, getDArray(resultArray, j));
		}
	}
	return newResults;
}

void displayDataBase(FILE *file, DataBase *dataBase) {
	for (int i = 0; i < sizeDArray(dataBase->store); i++) {
		Record *record = getDArray(dataBase->store, i);
		fprintf(file, "%s\n", getRecord(record));
	}
}

void displaySelectDataBase(FILE *file, DArray *results, char *fields) {
	for (int i = 0; i < sizeDArray(results); i++) {
		Record *record = getDArray(results, i);
		char *recordFields = getRecord(record);
		if (fields == 0) {
			fprintf(file, "%s", recordFields);
		} else {
			DArray *splitFields = separateFields(fields);
			for (int j = 0; j < sizeDArray(splitFields); j++) {
				char *currentField = getDArray(splitFields, j);
				if (strstr(recordFields, currentField)) {
					char *key = currentField;
					Integer *value = parseInteger(recordFields, key);
					char *keyValue = buildKeyValuePair(key, getInteger(value));
					fprintf(file, "%s ", keyValue);
				}
			}
		}
		fprintf(file, "\n");
	}
}
