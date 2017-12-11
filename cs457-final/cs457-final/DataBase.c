//
//  DataBase.c
//  cs457-final
//
//  Created by Greyson Wright on 12/6/17.
//  Copyright © 2017 Greyson Wright. All rights reserved.
//

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
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
char *addSpacePadding(char *);
char *addKeyPadding(char *);
int min(int, int);
void sortDarray(DArray *, char *);
char *stripWhiteSpaceDataBase(char *);
DArray *separateRecords(char *);
char *removeKeyPadding(char *);
DArray *findNonExistingField(DataBase *, char *);
DArray *filterVersion(DArray *, DocumentStore *, int, void (*)(FILE *, void *));
DArray *searchDataBase(DataBase *, char *);
DArray *searchDarray(DArray *, char *, void (*)(FILE *, void *));
DArray *splitFields(Record *);
DArray *andQuery(DataBase *, char *);
DArray *rangedQuery(DataBase *, char *);
DArray *basicQuery(DataBase *, char*);
DArray *andRangedQuery(DataBase *, char *);
DArray *andBasicQuery(DataBase *, char *);

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
	char *sysIDString = buildKeyValuePair("sysid", dataBase->fieldCount);
	char *vnString = buildKeyValuePair("vn", vn);
	appendFieldRecord(record, vnString);
	appendFieldRecord(record, sysIDString);
	char *paddedFields = getRecord(record);
	paddedFields = addSpacePadding(paddedFields);
	setRecord(record, paddedFields);
	insertDArray(dataBase->store, record);
	dataBase->fieldCount++;
//	free(sysIDString);
//	free(vnString);
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
	if (query == 0) {
		results = basicQuery(dataBase, " sysid:");
	} else if (isAndQuery(query)) {
		results = andQuery(dataBase, query);
	} else if (isRangedQuery(query)) {
		results = rangedQuery(dataBase, query);
	} else {
		query = addSpacePadding(query);
		results = basicQuery(dataBase, query);
	}
	
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
	char *token = malloc(strlen(query) + 1);
	strcpy(token, query);
	
	DArray *keyValues = separateFields(query);
	for (int i = 0; i < sizeDArray(keyValues); i++) {
		char *keyValue = getDArray(keyValues, i);
		keyValue = addSpacePadding(keyValue);
		DArray *tmp = newDArray(dataBase->display);
		if (isRangedQuery(keyValue)) {
			tmp = rangedQuery(dataBase, keyValue);
		} else {
			keyValue = convertToKeyValue(keyValue);
			tmp = basicQuery(dataBase, keyValue);
		}
		
		for (int j = 0; j < sizeDArray(tmp); j++) {
			Record *record = getDArray(tmp, j);
			insertDArray(resultArray, record);
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

DArray *findNonExistingField(DataBase *dataBase, char *query) {
	DArray *resultArray = newDArray(dataBase->display);
	for (int i = 0; i < sizeDArray(dataBase->store); i++) {
		Record *record = getDArray(dataBase->store, i);
		char *key = getKey(query);
		if (!strstr(getRecord(record), key)) {
			insertDArray(resultArray, record);
		}
	}
	return resultArray;
}

DArray *separateFields(char *source) {
	char *token = malloc(strlen(source) + 1);
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
	char *keyValue = malloc(strlen(source) + 1);
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
			queryKey = stripWhiteSpaceDataBase(queryKey);
			queryKey = addKeyPadding(queryKey);
			char *recordKeyValue = findKeyValue(getRecord(record), queryKey);
			Integer *recordValue = 0;
			Integer *queryValue = 0;
			if (recordKeyValue) {
				char *key = removeKeyPadding(queryKey);
				recordValue = parseInteger(recordKeyValue, key);
				queryValue = parseInteger(queryKeyValue, key);
			}
			if (recordValue && queryValue && compareInteger(recordValue, queryValue) <= 0) {
				insertDArray(resultArray, record);
			}
		}
	} else if (strstr(query, ">=")) {
		for (int i = 0; i < sizeDArray(dataBase->store); i++) {
			Record *record = getDArray(dataBase->store, i);
			char *queryKeyValue = flattenRange(query);
			char *queryKey = getKey(queryKeyValue);
			queryKey = stripWhiteSpaceDataBase(queryKey);
			queryKey = addKeyPadding(queryKey);
			char *recordKeyValue = findKeyValue(getRecord(record), queryKey);
			Integer *recordValue = 0;
			Integer *queryValue = 0;
			if (recordKeyValue) {
				char *key = removeKeyPadding(queryKey);
				recordValue = parseInteger(recordKeyValue, key);
				queryValue = parseInteger(queryKeyValue, key);
			}
			if (recordValue && queryValue && compareInteger(recordValue, queryValue) >= 0) {
				insertDArray(resultArray, record);
			}
		}
	} else if (strstr(query, "<")) {
		for (int i = 0; i < sizeDArray(dataBase->store); i++) {
			Record *record = getDArray(dataBase->store, i);
			char *queryKeyValue = flattenRange(query);
			char *queryKey = getKey(queryKeyValue);
			queryKey = stripWhiteSpaceDataBase(queryKey);
			queryKey = addKeyPadding(queryKey);
			char *recordKeyValue = findKeyValue(getRecord(record), queryKey);
			Integer *recordValue = 0;
			Integer *queryValue = 0;
			if (recordKeyValue) {
				char *key = removeKeyPadding(queryKey);
				recordValue = parseInteger(recordKeyValue, key);
				queryValue = parseInteger(queryKeyValue, key);
			}
			if (recordValue && queryValue && compareInteger(recordValue, queryValue) < 0) {
				insertDArray(resultArray, record);
			}
		}
	} else {
		for (int i = 0; i < sizeDArray(dataBase->store); i++) {
			Record *record = getDArray(dataBase->store, i);
			char *queryKeyValue = flattenRange(query);
			char *queryKey = getKey(queryKeyValue);
			queryKey = stripWhiteSpaceDataBase(queryKey);
			queryKey = addKeyPadding(queryKey);
			char *recordKeyValue = findKeyValue(getRecord(record), queryKey);
			Integer *recordValue = 0;
			Integer *queryValue = 0;
			if (recordKeyValue) {
				char *key = removeKeyPadding(queryKey);
				recordValue = parseInteger(recordKeyValue, key);
				queryValue = parseInteger(queryKeyValue, key);
			}
			if (recordValue && queryValue && compareInteger(recordValue, queryValue) > 0) {
				insertDArray(resultArray, record);
			}
		}
	}
	return resultArray;
}

char *removeKeyPadding(char *string) {
	long length = strlen(string);
	int count = 0;
	char *newString = malloc(length + 1);
	for (int i = 0; i < length; i++) {
		if (string[i] != ':' && !isspace(string[i])) {
			newString[count++] = string[i];
		}
	}
	return newString;
}

char *flattenRange(char *source) {
	char *keyValue = malloc(strlen(source) + 1);
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
	char *token = malloc(strlen(source) + 1);
	strcpy(token, source);
	
	char *key = strtok(token, ":");
	return key;
}

char *findKeyValue(char *source, char *key) {
	char *token = malloc(strlen(source) + 1);
	strcpy(token, source);
	
	char *keyValue = strtok(token, " ");
	
	while (keyValue) {
		keyValue = addSpacePadding(keyValue);
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
		if (!strstr(query, "<>")) {
			if (strstr(getRecord(record), query)) {
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

char *addSpacePadding(char *string) {
	char *newString = malloc(strlen(string) + 3);
	sprintf(newString, " %s ", string);
	return newString;
}

char *addKeyPadding(char *string) {
	char *newString = malloc(strlen(string) + 3);
	sprintf(newString, " %s:", string);
	return newString;
}

char *stripNotEqualOp(char *source) {
	char *keyValue = malloc(strlen(source) + 1);
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
			keyValue = addSpacePadding(keyValue);
			DArray *results = searchDataBase(dataBase, keyValue);
			count += min(sizeDArray(results), version);
		}
		
		return count;
	}
	
	query = addKeyPadding(query);
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
	field = addKeyPadding(field);
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
			char *key = removeKeyPadding(field);
			Integer *currentValue = parseInteger(getRecord(currentRecord), key);
			Record *nextRecord = getDArray(darray, j + 1);
			Integer *nextValue = parseInteger(getRecord(nextRecord), key);
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
		keyValue = addSpacePadding(keyValue);
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
			DArray *splitFields = separateRecords(getRecord(record));
			Integer *vnVal = parseInteger(recordFields, "vn");
			char *vnKeyValue = buildKeyValuePair("vn", getInteger(vnVal));
			fprintf(file, "%s ", vnKeyValue);
			for (int j = 0; j < sizeDArray(splitFields); j++) {
				char *currentField = getDArray(splitFields, j);
				if (!strstr(currentField, "vn")) {
					fprintf(file, "%s ", currentField);
				}
			}
//			free(vnKeyValue);
//			free(vnVal);
		} else {
			DArray *splitFields = separateFields(fields);
			Integer *vnVal = parseInteger(recordFields + 1, "vn");
			char *vnKeyValue = buildKeyValuePair("vn", getInteger(vnVal));
			fprintf(file, "%s ", vnKeyValue);
			for (int j = 0; j < sizeDArray(splitFields); j++) {
				char *currentField = getDArray(splitFields, j);
				if (strstr(recordFields, currentField)) {
					char *key = stripWhiteSpaceDataBase(currentField);
					Integer *value = parseInteger(recordFields, key);
					char *keyValue = buildKeyValuePair(key, getInteger(value));
					fprintf(file, "%s ", keyValue);
				}
			}
		}
		fprintf(file, "\n");
	}
}

DArray *separateRecords(char *source) {
	char *token = malloc(strlen(source) + 1);
	strcpy(token, source);
	
	DArray *resultArray = newDArray(0);
	char *keyValue = strtok(token, " ");
	while (keyValue) {
		insertDArray(resultArray, keyValue);
		keyValue = strtok(0, " ");
	}
	
	return resultArray;
}

char *stripWhiteSpaceDataBase(char *token) {
	long tokenLen = strlen(token);
	char *newToken = malloc(tokenLen + 1);
	int count = 0;
	for (int i = 0; i < tokenLen; i++) {
		if (!isspace(token[i])) {
			newToken[count++] = token[i];
		}
	}
	return newToken;
}
