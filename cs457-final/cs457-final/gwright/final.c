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
#include <ctype.h>
#include "DataBase.h"
#include "Record.h"
#include "darray.h"
#include "scanner.h"

char *stripWhiteSpace(char *);
int parseVersion(char *);
char *getBracketContents(char *, int);

int main(int argc, const char * argv[]) {
	DataBase *dataBase = newDataBase(displayRecord);
	FILE *dataFile = fopen("data.txt", "r");
	char *line = readLine(dataFile);
	while (!feof(dataFile)) {
		insertDataBase(dataBase, line);
		line = readLine(dataFile);
	}
	fclose(dataFile);
	line = 0;
	
	FILE *queryFile = fopen("queries.txt", "r");
	FILE *outFile = fopen("gwright.txt", "w+");
	line = readLine(queryFile);
	while (!feof(dataFile)) {
		fprintf(outFile, "%s\n", line);
		if (!strstr(line, "insert")) {
			line = stripWhiteSpace(line);
		}
		char *fullLine = malloc(strlen(line) + 1);
		strcpy(fullLine, line);
		char *collectionCommand = strtok(line, "[");
		if (strstr(collectionCommand, "final.")) {
			if (strstr(collectionCommand, "insert")) {
				char *string = collectionCommand;
				(void)strtok(string, "(");
				char *records = strtok(0, ")");
				insertDataBase(dataBase, records);
			} else if (strstr(collectionCommand, "count")) {
				char *field = getBracketContents(fullLine, 0);
				char *versionString = getBracketContents(fullLine, 1);
				int version = parseVersion(versionString);
				int count = countDataBase(dataBase, field, version);
				fprintf(outFile, "count_%s:%d\n", field, count);
			} else if (strstr(collectionCommand, "query")) {
				fullLine = fullLine + strlen(line);
				char *query = getBracketContents(fullLine, 0);
				char *fields = getBracketContents(fullLine, 1);
				char *versionString = getBracketContents(fullLine, 2);
				int version = parseVersion(versionString);
				DArray *results = queryDataBase(dataBase, query, version);
				displaySelectDataBase(outFile, results, fields);
			} else if (strstr(collectionCommand, "sort")) {
				char *field = getBracketContents(fullLine, 0);
				char *versionString = getBracketContents(fullLine, 1);
				int version = parseVersion(versionString);
				DArray *results = sortDataBase(dataBase, field, version);
				displaySelectDataBase(outFile, results, "");
			} else {
				fprintf(outFile, "Operation not supported.\n");
			}
		} else {
			fprintf(outFile, "Operation not supported.\n");
		}
		fprintf(outFile, "\n");
		line = readLine(dataFile);
	}
	fclose(outFile);
	fclose(queryFile);
	return 0;
}

char *stripWhiteSpace(char *token) {
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

int parseVersion(char *versionString) {
	if (versionString == 0) {
		return 1;
	}
	if (strlen(versionString) == 0) {
		return 0;
	}
	return atoi(versionString);
}

char *getBracketContents(char *source, int index) {
	long length = strlen(source);
	char *string = malloc(length + 1);
	char *contents = 0;
	strcpy(string, source);
	int count = 0;
	
	for (int i = 0; i < length; i++) {
		char c = source[i];
		if (c == '[') {
			if (count == index) {
				contents = strtok(string + i, "]");
				return contents + 1;
			}
			count++;
		}
	}
	return contents;
}
