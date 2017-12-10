//
//  Record.c
//  cs457-final
//
//  Created by Greyson Wright on 12/5/17.
//  Copyright © 2017 Greyson Wright. All rights reserved.
//

#include <stdlib.h>
#include <string.h>
#include "Record.h"

struct RECORD {
	int isDuplicate;
	char *fields;
};

Record *newRecord(char *fields) {
	Record *record = malloc(sizeof *record);
	record->isDuplicate = 0;
	record->fields = fields;
	return record;
}

int compareRecord(void *left, void *right) {
	return strcmp(getRecord(left), getRecord(right));
}

void displayRecord(FILE *file, void *record) {
	Record *rec = record;
	fprintf(file, "%s", getRecord(rec));
}

char *getRecord(Record *record) {
	return record->fields;
}

int getIsDuplicateRecord(Record *record) {
	return record->isDuplicate;
}

void appendFieldRecord(Record *record, char *keyValue) {
	char *fields = malloc(strlen(keyValue) + strlen(record->fields) + 2);
	strcpy(fields, keyValue);
	strcat(fields, " ");
	strcat(fields, record->fields);
	free(record->fields);
	record->fields = fields;
}

void setRecord(Record *record, char *fields) {
	record->fields = fields;
}

void markAsDuplicateRecord(Record *record) {
	record->isDuplicate = 1;
}
